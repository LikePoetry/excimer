#include "hzpch.h"

#pragma warning(push, 0)
#include <Tracy/TracyClient.cpp>

#define VOLK_IMPLEMENTATION
#include <vulkan/volk/volk.h>

#undef IMGUI_IMPL_VULKAN_NO_PROTOTYPES
#undef VK_NO_PROTOTYPES
#include <imgui/backends/imgui_impl_vulkan.cpp>

#include <imgui/misc/freetype/imgui_freetype.cpp>

#define STB_DEFINE
#include <stb/stb.h>

#include <stb/stb_vorbis.c>

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb/stb_rect_pack.h>

#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

#include <OpenFBX/miniz.c>
#include <OpenFBX/ofbx.cpp>
#pragma warning(pop)