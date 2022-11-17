#include "hzpch.h"
#include "ImGuiManager.h"
#include "excimer/core/os/Input.h"
#include "excimer/core/os/Window.h"
#include "excimer/core/Application.h"
#include "excimer/graphics/rhi/IMGUIRenderer.h"

#include "excimer/core/VFS.h"
#include "ImGuiUtilities.h"


#include "IconsMaterialDesignIcons.h"

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include <imgui/imgui.h>
#include <imgui/Plugins/ImGuizmo.h>
#include <imgui/Plugins/ImGuiAl/fonts/MaterialDesign.inl>
#include <imgui/Plugins/ImGuiAl/fonts/RobotoRegular.inl>
#include <imgui/Plugins/ImGuiAl/fonts/RobotoBold.inl>
#include <imgui/misc/freetype/imgui_freetype.h>

#define USING_GLFW

#ifdef USING_GLFW
#include <GLFW/glfw3.h>
#endif

namespace Excimer
{

}