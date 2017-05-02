/* main.c 
Capsule is a bytecode experiment by Sam "Gyro" Cutlip. Enjoy!
Requires C99. 
All rights reserved by Samuel Cutlip. 
(An open-source release is pending, when I decide on a license.)*/

#include <assert.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define REGISTER_COUNT 256
#define regi_t uint8_t //Register index type. Directly linked to how many registers we can have. Must be unsigned
#define instID_t uint8_t //Instruction ID type. Directly linked to how many instructions we can have. Must be unsigned.
#define word_t uint64_t //Size of each register.

inline void check_register(regi_t index) {
	assert(index < REGISTER_COUNT);
}

typedef struct {
	size_t size;
	uint8_t* data;
} ByteBuffer;

ByteBuffer* make_buffer(size_t size) {
	uint8_t* dat = malloc( size);
	ByteBuffer* ret = (ByteBuffer*) malloc(sizeof(ByteBuffer));
	ret->size = size;
	ret->data = dat;
	return ret;
}
ByteBuffer* buffer_from(void* dat, size_t size) {
	ByteBuffer* ret = (ByteBuffer*) malloc(sizeof(ByteBuffer));
	ret->size = size;
	ret->data = dat;
	return ret;
}

typedef struct {
	word_t registers[REGISTER_COUNT];
	uint64_t program_counter;
	ByteBuffer* program;
	bool keep_running; //Set to false when reaching program_counter == program->size, or by QIT.
} CapsuleContext;

inline void assign_register(CapsuleContext* context, regi_t index, word_t value) {
	context->registers[index] = value;
}
inline word_t get_register(CapsuleContext* context, regi_t index) {
	return context->registers[index];
}
inline void incr_register(CapsuleContext* context, regi_t index) {
	++(context->registers[index]);
}
inline void decr_register(CapsuleContext* context, regi_t index) {
	--(context->registers[index]);
}

void init_context(CapsuleContext* context) { 
	memset(context->registers, 0, REGISTER_COUNT);
	context->program_counter = 0;
	context->keep_running = true;
}

CapsuleContext* new_context(ByteBuffer* program) {
	CapsuleContext* ctx = malloc(sizeof(CapsuleContext));
	init_context(ctx);
	ctx->program = program; 
	return ctx;
}

typedef enum {
	NOP, //No operation. Does nothing.
	INC, //Increment. Follow it by a register number to increment (+ 1) that register.
	DEC, //Decrement. Follow it by a register number to decrement (- 1) that register.
	DBG, //Debug. Print this register.
	QIT, //Quit. Terminates the program.
	LDC,  //Load constant. Loads argument 2, a constant, into argument 1, a register ID.
	CPY,  //Copy. Copy register specified by argument 2, into register in argument 1. Equivalent to MOV.
	JNZ, //Jump not zero. Set program counter to the argument 2 if the register (argument 1) is not 0.
	JWZ //Jump when zero. Set program counter to the argument 2 if the register (argument 1) is 0.
	} CapsuleInstruction;
	
	
/*In the future, I will make a more packed program data format. 
	8-bit instruction ID, 8-bit register ID, and 64 bits of argument (which could be another register ID).
	There is probably packing here.
	Flags may become a thing further down along the line.*/ 
typedef struct {
	instID_t instruction;
	regi_t reg;
	word_t argument;
} Line; //Byte code or code point might be a more proper term for this. For now, "line" is true enough. 

Line mkline(CapsuleInstruction inst, regi_t reg, word_t arg) {
	Line ret;
	ret.instruction = inst;
	ret.reg = reg;
	ret.argument = arg;
	return ret;
}
inline size_t byte_to_line(size_t size) {
	return (size / sizeof(Line));
}
inline size_t line_to_byte(uint64_t line_number) {
	return (line_number * sizeof(Line));
}

void process_instruction(CapsuleContext* context, Line line) {
	switch(line.instruction) {
		case NOP:
			break;
		case INC:
			context->registers[line.reg] = context->registers[line.reg] + 1;
			break;
		case DEC:
			context->registers[line.reg] = context->registers[line.reg] - 1;
			break;
		case DBG:
			printf("%i \n", context->registers[line.reg]);
			break;
		case QIT:
			context->keep_running = false;
			break;
		case LDC:
			context->registers[line.reg] = line.argument;
			break;
		case CPY:
			check_register(line.argument);
			context->registers[line.reg] = context->registers[line.argument];
			break;
		case JNZ:
			assert(line.argument < byte_to_line(context->program->size));
			if (context->registers[line.reg] != 0) {
				context->program_counter = line.argument;
			}
			break;
		case JWZ:
			assert(line.argument < byte_to_line(context->program->size));
			if (context->registers[line.reg] == 0) {
				context->program_counter = line.argument;
			}
			break;
		default:
			printf("Default case. Shouldn't reach this.\n");
			break;
	}
}

void run_context(CapsuleContext* context) {
	//We will be copying to this a lot. Could do it indirectly? I don't know how to CPU cache optimize, but this seems better.
	Line line;
	//Loop the program
	printf("Starting loop...\n");
	while (context->keep_running) {
		//printf("Program counter: %i \n", context->program_counter);
		//printf("Value of register 1: %i \n", context->registers[1]);
		//Get our line.
		uint8_t* line_ptr = context->program->data + line_to_byte(context->program_counter);
		line = *((Line*)line_ptr);
		//printf("Line instruction ID: %i \n", line.instruction);
		
		process_instruction(context, line);
		
		//Go to our next instruction.
		++context->program_counter;
		//Have we hit the end of the program (or beyond)?
		if(context->program_counter >= byte_to_line(context->program->size)) {
			context->keep_running = false;
		}
	}	
}

ByteBuffer* Assemble(char* src) {

}

int main(void) {
	Line program[] = {
		mkline(NOP, 0, 0),
		mkline(NOP, 0, 0),
		mkline(NOP, 0, 0),
		mkline(LDC, 1, 20),
		mkline(DBG, 1, 0),
		mkline(DEC, 1, 0),
		mkline(JNZ, 1, 3),
		mkline(DBG, 1, 0)};
	ByteBuffer* program_buffer = buffer_from(program, sizeof(program));
	CapsuleContext* context = new_context(program_buffer);
	run_context(context);
	printf("Goodbye, world!\n");
	return 0;
}