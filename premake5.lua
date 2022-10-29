function SetRecommendedSettings()
	filter ()
	editandcontinue "Off"
	warnings "Off"
end

IncludeDir = {}
IncludeDir["spdlog"] = "excimer/external/spdlog/include"
IncludeDir["external"] = "excimer/external"

workspace "excimer"
    architecture "x64"
    startproject "editor"

    configurations{
        "Debug"
    }

    outputdir="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "external"
	require("excimer/external/spdlog/premake5")
		SetRecommendedSettings()
	filter {}

group ""

--静态链接库，核心文件
project "excimer"
	location "excimer"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	editandcontinue "Off"

	targetdir("bin/" ..outputdir.. "/%{prj.name}")
	objdir("bin-int/" ..outputdir.. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.external}",
		"%VULKAN_SDK%/include"
	}

	libdirs 
	{ 
		"spdlog",
		"%VULKAN_SDK%/lib" 
	}

	links
	{
		"vulkan-1.lib"
	}

	filter 'architecture:x86_64'
	defines { "USE_VMA_ALLOCATOR"}

	filter { "files:excimer/external/**"}
		warnings "Off"

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"
		disablewarnings { 4307 }
		characterset ("Unicode")
		pchheader "hzpch.h"
		pchsource "excimer/src/hzpch.cpp"
	

		defines
		{
			"SLIGHT_PLATFORM_WINDOWS",
			"SLIGHT_RENDER_API_OPENGL",
			"SLIGHT_RENDER_API_VULKAN",
			"VK_USE_PLATFORM_WIN32_KHR",
			"WIN32_LEAN_AND_MEAN",
			"_CRT_SECURE_NO_WARNINGS",
			"_DISABLE_EXTENDED_ALIGNED_STORAGE",
			"_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
			"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
			"SLIGHT_IMGUI",
			"SLIGHT_OPENAL",
			"SLIGHT_VOLK",
			"SLIGHT_USE_GLFW_WINDOWS"
		}

		buildoptions
		{
		"/MP", "/bigobj"
		}

		filter 'files:excimer/external/**.cpp'
			flags  { 'NoPCH' }
		filter 'files:excimer/external/**.c'
			flags  { 'NoPCH' }


	disablewarnings { 4307 }

	filter "configurations:Debug"
		defines {"TRACY_ENABLE","TRACY_ON_DEMAND"}
		runtime "Debug"
		symbols "on"
		optimize "Off"

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
		"%{IncludeDir.external}",
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
		defines {"TRACY_ENABLE","TRACY_ON_DEMAND"}
		runtime "Debug"
		symbols "on"    
		defines
		{
			"WIN32_LEAN_AND_MEAN",
		}