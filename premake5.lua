function SetRecommendedSettings()
	filter ()
	editandcontinue "Off"
	warnings "Off"
end

IncludeDir = {}
IncludeDir["spdlog"] = "excimer/external/spdlog/include"
IncludeDir["external"] = "excimer/external"
IncludeDir["GLFW"] = "excimer/external/glfw/include/"
IncludeDir["stb"] = "excimer/external/stb"
IncludeDir["glm"] = "excimer/external/glm"
IncludeDir["vulkan"] = "excimer/external/vulkan/"
IncludeDir["SpirvCross"] = "excimer/external/SPIRV-Cross"


workspace "excimer"
    architecture "x64"
    startproject "editor"

    configurations{
        "Debug"
    }

    outputdir="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "external"
	require("excimer/external/SPIRVCrosspremake5")
		SetRecommendedSettings()
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
		"%{IncludeDir.stb}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.SpirvCross}",
		"%{IncludeDir.vulkan}"
	}

	libdirs 
	{ 
		"spdlog",
	}

	links
	{
		"SpirvCross",
	}

	defines
	{
		"GLM_FORCE_INTRINSICS",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE"
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
		"excimer/external/Tracy/TracyClient.cpp",
		"excimer/external/vulkan/volk/volk.c"


    }

    includedirs
	{
        "excimer/src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.external}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.SpirvCross}",
		"%{IncludeDir.vulkan}"
    }

    links
    {
        "excimer",
		"glfw",
		"SpirvCross",
    }

    libdirs 
	{ 
	}

	defines
	{
		"GLM_FORCE_INTRINSICS",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE"
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
		