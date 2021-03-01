-- premake5.lua

workspace "SimpleECS"
   configurations { "Debug", "Release" }
   platforms { "Win32", "x64" }

project "SimpleECS"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   includedirs { 
	"$(SolutionDir)include",
   	"$(SolutionDir)thirdparty\\GLFW\\include",
   	"$(SolutionDir)thirdparty\\Glew\\include",
   	"$(SolutionDir)thirdparty\\PortAudio\\include",
   	"$(SolutionDir)thirdparty\\glm",
	"$(SolutionDir)thirdparty\\Lua\\include",
   }

   libdirs {
   	"$(SolutionDir)thirdparty\\GLFW\\lib",
   	"$(SolutionDir)thirdparty\\Glew\\lib",
   	"$(SolutionDir)thirdparty\\PortAudio\\lib",
   }

   links { 
	"glew32", 
	"glew32s", 
	"glfw3", 
	"glfw3dll", 
	"portaudio_x64",
	"opengl32",
	"glu32",
   }

   targetdir("$(SolutionDir)_build\\$(Platform)_$(Configuration)")
   objdir("!$(SolutionDir)_int\\$(Platform)_$(Configuration)")
   --basedir("..")

   files {".gitignore", "include\\**.h", "include\\**.hpp", "src\\**.cpp", "_misc\\**", "assets\\**" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"