function SetRecommendedSettings()
	filter ()
	editandcontinue "Off"
	warnings "Off"
end

IncludeDir = {}
IncludeDir["spdlog"] = "excimer/external/spdlog/include"
IncludeDir["external"] = "excimer/external"
IncludeDir["GLFW"] = "excimer/external/glfw/include/"

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
	require("excimer/external/GLFWpremake5")
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
		"%{IncludeDir.GLFW}",
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

	defines
	{
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
			"_CRT_SECURE_NO_WARNINGS",
			"_DISABLE_EXTENDED_ALIGNED_STORAGE",
		}
		
		links
		{
			"glfw",
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
	editandcontinue "Off"

	targetdir("bin/" ..outputdir.. "/%{prj.name}")
	objdir("bin-int/" ..outputdir.. "/%{prj.name}")
	

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"excimer/external/Tracy/TracyClient.cpp"
    }

    includedirs
	{
        "excimer/src",
		"%{IncludeDir.external}",
		"%{IncludeDir.GLFW}",
        "%VULKAN_SDK%/include",
    }

    links
    {
        "excimer",
		"glfw",
        "vulkan-1.lib"
    }

    libdirs 
	{ 
		"%VULKAN_SDK%/lib" 
	}

	defines
	{
	}

	filter { "files:excimer/external/**"}
	warnings "Off"

filter 'architecture:x86_64'
	defines { "USE_VMA_ALLOCATOR"}

filter "system:windows"
	cppdialect "C++17"
	staticruntime "On"
	systemversion "latest"
	conformancemode "off"

	defines
		{
			"WIN32_LEAN_AND_MEAN",
		}

	disablewarnings { 4307 }
filter "configurations:Debug"
		defines {"TRACY_ENABLE","TRACY_ON_DEMAND"}
		symbols "On"
		runtime "Debug"
		optimize "Off"
		