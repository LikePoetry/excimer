function SetRecommendedSettings()
	filter ()
	editandcontinue "Off"
	warnings "Off"
end

IncludeDir = {}
IncludeDir["entt"] = "excimer/external/entt/src/"
IncludeDir["spdlog"] = "excimer/external/spdlog/include"
IncludeDir["external"] = "excimer/external"
IncludeDir["GLFW"] = "excimer/external/glfw/include/"
IncludeDir["lua"] = "excimer/external/lua/src/"
IncludeDir["stb"] = "excimer/external/stb"
IncludeDir["glm"] = "excimer/external/glm"
IncludeDir["vulkan"] = "excimer/external/vulkan/"
IncludeDir["SpirvCross"] = "excimer/external/SPIRV-Cross"
IncludeDir["msdf_atlas_gen"] = "excimer/external/msdf-atlas-gen/msdf-atlas-gen"
IncludeDir["msdfgen"] = "excimer/external/msdf-atlas-gen/msdfgen"
IncludeDir["freetype"] = "excimer/external/freetype/include"
IncludeDir["ImGui"] = "excimer/external/imgui/"
IncludeDir["cereal"] = "excimer/external/cereal/include"


workspace "excimer"
    architecture "x64"
    startproject "editor"

    configurations{
        "Debug"
    }

    outputdir="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "external"
	require("excimer/external/lua/premake5")
		SetRecommendedSettings()
	require("excimer/external/imguipremake5")
		SetRecommendedSettings()
	require("excimer/external/freetype/premake5")
		SetRecommendedSettings()
	require("excimer/external/SPIRVCrosspremake5")
		SetRecommendedSettings()
	require("excimer/external/spdlog/premake5")
		SetRecommendedSettings()
	require("excimer/external/GLFWpremake5")
		SetRecommendedSettings()
	require("excimer/external/msdf-atlas-gen/premake5")
		SetRecommendedSettings()
	require("excimer/external/ModelLoaders/meshoptimizer/premake5")
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
		"%{IncludeDir.entt}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.external}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.lua}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.SpirvCross}",
		"%{IncludeDir.msdfgen}",
		"%{IncludeDir.msdf_atlas_gen}",
		"%{IncludeDir.freetype}",
		"%{IncludeDir.cereal}",
		"%{IncludeDir.vulkan}"
	}

	libdirs 
	{ 
		"spdlog",
	}

	links
	{
		"lua",
		"SpirvCross",
		"freetype",
		"imgui",
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
		"IMGUI_USER_CONFIG=\"../src/excimer/imgui/ImConfig.h\"",
			"_CRT_SECURE_NO_WARNINGS",
			"_DISABLE_EXTENDED_ALIGNED_STORAGE",
			"EXCIMER_ENGINE"
		}
		
		links
		{
			"glfw",
			"meshoptimizer",
			"msdf-atlas-gen"
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
		"%{prj.name}/src/**.cpp"
    }

    includedirs
	{
        "excimer/src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.lua}",
		"%{IncludeDir.external}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.SpirvCross}",
		"%{IncludeDir.msdfgen}",
		"%{IncludeDir.msdf_atlas_gen}",
		"%{IncludeDir.freetype}",
		"%{IncludeDir.cereal}",
		"%{IncludeDir.vulkan}"
    }

    links
    {
        "excimer",
		"glfw",
		"lua",
		"imgui",
		"SpirvCross",
		"freetype",
		"meshoptimizer",
		"msdf-atlas-gen"
    }

    libdirs 
	{ 
	}

	defines
	{
		"IMGUI_USER_CONFIG=\"../../excimer/src/excimer/imgui/ImConfig.h\"",
		"GLM_FORCE_INTRINSICS",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE",
		"EXCIMER_ENGINE"
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
		