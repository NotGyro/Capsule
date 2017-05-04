Built-in Type System
======
* Higher-level type systems built upon this one must be able to define Plain Old Data types / custom types which are always pass-by-copy rather than pass-by-reference. Let's not be Java.
* Do not try to cram something into a register which would could never physically fit into 2 64-bit registers. Read: Do not try to put the whole definition of a type in a register, haah.
	+ Type ID? Or reference to type table? Perhaps the type system is not even built-in register-level.
* Do not assume Object Oriented. Thus: Lua-like system. Table of "whatever."
* The interface between contexts / modules is *not* likely to be low-level enough for registers to matter.
	+ Calling convention, though! Maintain consistent call / return.
* Length of allocation is an important one. Very important! Also what is / is not a pointer.
* Optional table of contents? Can point to object-specific one, can point to consistent one for type. Has a mutable flag for that reason.