workspace "excimer"
    architecture "x64"
    startproject "editor"

    configurations{
        "Debug"
    }

    outputdir="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group ""

--静态链接库，核心文件
project "excimer"
	location "excimer"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	characterset ("MBCS")

	targetdir("bin/" ..outputdir.. "/%{prj.name}")
	objdir("bin-int/" ..outputdir.. "/%{prj.name}")

	pchheader "excimer/src/hzpch.h"
	pchsource "excimer/src/hzpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%VULKAN_SDK%/include"
	}

	libdirs 
	{ 
		"%VULKAN_SDK%/lib" 
	}

	links
	{
		"vulkan-1.lib"
	}

	flags { "NoPCH" }
	filter "system:windows"
		systemversion "latest"

		defines
		{
			GLFW_INCLUDE_NONE
		}


	filter "configurations:Debug"
		defines ""
		runtime "Debug"
		symbols "on"

-- 测试工具项目
project "editor"
    location "editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	characterset ("MBCS")

	targetdir("bin/" ..outputdir.. "/%{prj.name}")
	objdir("bin-int/" ..outputdir.. "/%{prj.name}")
	

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
    }

    includedirs
	{
        "excimer/src",
        "%VULKAN_SDK%/include",
    }

    links
    {
        "excimer",
        "vulkan-1.lib"
    }

    libdirs 
	{ 
		"%VULKAN_SDK%/lib" 
	}

    filter "system:windows"
		systemversion "latest"

    filter "configurations:Debug"
		defines ""
		runtime "Debug"
		symbols "on"    