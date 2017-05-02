-- premake5.lua
workspace "Capsule"
   configurations { "Debug", "Release" }
   architecture "x86_64"
   location "generated"

project "Capsule"
   kind "ConsoleApp"
   language "C"
	targetdir ("build/bin/%{prj.name}/%{cfg.longname}")
	objdir ("build/bbj/%{prj.name}/%{cfg.longname}")

   files { "**.h", "**.c" }
   flags { "C99" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"