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
    kind "StaticLib"
    defines { "RAYTRACING_STATICLIB" }

    includedirs {
        "RayTracing/Include"
    }
    files {
        "./RayTracing/Include/*.h",
        "./RayTracing/Source/*.cc"
    }

-- project RealtimeViewer
project "RTViewer"
    language "C++"
    kind "ConsoleApp"

	--defines { "RAYTRACING_STATICLIB" }
	--dependson { "RayTracing" }
	--links { "RayTracing" }
	
-- third library cflags and libs
	includedirs { "./ThirdParty/SDL/include" }
	libdirs { 
		"./ThirdParty/SDL/lib/%{cfg.architecture:gsub('x86_64', 'x64')}",
		"./ThirdParty/SOIL"
	}
	links { 
		"glu32",
		"opengl32",
		"SDL2",
		"SDL2main",
		"nfd",
		"SOIL"
	}
	dependson { "nfd" }
	
	postbuildcommands {
		-- Copy the SDL2 dll to the Bin folder.
		'{COPY} "%{path.getabsolute("./ThirdParty/SDL/lib/" .. cfg.architecture:gsub("x86_64", "x64") .. "/SDL2.dll")}" "%{cfg.targetdir}"',
		--'{COPY} "%{path.getabsolute("./ThirdParty/imgui/misc/fonts/*.ttf")}" "%{cfg.targetdir}"',
		--'{COPY} "%{path.getabsolute("./Resource/*.ttf")}" "%{cfg.targetdir}"'
	}

    includedirs {
        "RayTracing/Include",
        "RealtimeViewer/Include",
		"./ThirdParty/imgui",
		"./ThirdParty/nfd",
		"./ThirdParty/SOIL",
    }
	
	vpaths { 
	    ["imgui"] = {
						"./ThirdParty/imgui/*.h", 
						"./ThirdParty/imgui/*.cpp"
					}
	}
	
    files {
        "./RealtimeViewer/Include/*.h",
        "./RealtimeViewer/Source/*.cpp",
		"./ThirdParty/imgui/*.h",
		"./ThirdParty/imgui/*.cpp"
    }


include("./ThirdParty/nfd/premake5.lua")
