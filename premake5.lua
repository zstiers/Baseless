workspace "baseless"
	configurations { "Debug", "Hybrid", "Release" }
	editAndContinue "Off"
	location ".build"
	platforms { "Win32", "Win64" }
    startproject "tests"
	targetdir "%{wks.location}/.bin/%{cfg.buildcfg} (%{cfg.architecture})/%{prj.name}"
    
	-- Special for actions
	filter "action:vs*"
		platforms { "Win32", "Win64" }
    
	-- Special for configurations
	filter "configurations:Debug"
		defines { "DEBUG" }
		flags { "Symbols" }
		libdirs { "ext/lib/Debug (%{cfg.architecture})" }
		runtime "Debug"
        
	filter "configurations:Hybrid"
		defines { "DEBUG" }
		flags { "Symbols" }
		libdirs { "ext/lib/Release (%{cfg.architecture})" }
		runtime "Release"
        
	filter "configurations:Release"
		defines { "NDEBUG" }
		libdirs { "ext/lib/Release (%{cfg.architecture})" }
		optimize "On"
		runtime "Release"
        
	-- Special for platforms
	filter "platforms:Win32"
		system "Windows"
		architecture "x32"

	filter "platforms:Win64"
		system "Windows"
		architecture "x64"
    
	-- Reset filters
	filter {}
    
	project "baseless"
		-- Basics
		kind "StaticLib"
		language "C++"
		location "%{wks.location}/%{prj.name}"
    
		-- Files, with a nice default vpath (visual studio filters)
		files { "inc/**.*", "src/**.*", "ext/**.*" }
		vpaths { ["*"] = { "inc/*", "src/*" } }
    
		-- Reset filters
		filter {}

	-- Tests
	project "tests"
		-- Basics
		kind "ConsoleApp"
		language "C++"
		location "%{wks.location}/%{prj.name}"

		-- Files, with a nice default vpath (visual studio filters)
		files{ "tests/**.*" }
		vpaths{ ["*"] = "tests.*" }

		-- Reset filters
		filter {}

		-- External linkage
		includedirs "inc"
		links { "baseless" }