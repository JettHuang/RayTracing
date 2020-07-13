-- premake5.lua project solution script

cfg_systemversion = "latest" -- "10.0.17763.0"   -- To use the latest version of the SDK available

-- solution
workspace "RayTracingProject"
    configurations { "Debug", "Release" }
    platforms { "Win32", "Win64" }

    location "Build"
    defines { "_CRT_SECURE_NO_WARNINGS" }
    systemversion(cfg_systemversion)
	
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        targetsuffix("_d")

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"    

    filter "platforms:Win32"
        architecture "x32"

    filter "platforms:Win64"
        architecture "x64"

    filter {}

    targetdir("Build/Bin")  
    objdir("Build/Obj/%{prj.name}/%{cfg.buildcfg}")
	debugdir ("Build/Bin");
	
-- project RayTracing
project "RayTracing"
    language "C++"
    kind "ConsoleApp"

    includedirs {
        "./RayTracing/common",
		"./RayTracing/scene"
    }
    files {
        "./RayTracing/common/*.h",
        "./RayTracing/common/*.cc",
		"./RayTracing/scene/*.h",
		"./RayTracing/scene/*.cc",
		"./RayTracing/raytracing.h",
		"./RayTracing/raytracing.cc"
    }
