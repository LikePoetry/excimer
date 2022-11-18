#pragma once
// APP
#include "excimer/core/Core.h"
#include "excimer/core/ExLog.h"
#include "excimer/core/Application.h"
#include "excimer/scene/SceneManager.h"
#include "excimer/scene/Scene.h"

// Graphics
#include "excimer/graphics/RHI/Texture.h"
#include "excimer/graphics/RHI/GraphicsContext.h"
#include "excimer/graphics/RHI/Framebuffer.h"
#include "excimer/graphics/RHI/Shader.h"
#include "excimer/graphics/RHI/IMGUIRenderer.h"
#include "excimer/graphics/Mesh.h"
#include "excimer/graphics/Model.h"
#include "excimer/graphics/Material.h"
#include "excimer/graphics/Sprite.h"
#include "excimer/graphics/GBuffer.h"
#include "excimer/graphics/Terrain.h"
#include "excimer/graphics/Light.h"
#include "excimer/graphics/Environment.h"
#include "excimer/graphics/MeshFactory.h"
#include "excimer/graphics/AnimatedSprite.h"

// Entity
#include "excimer/scene/Component/Components.h"
#include "excimer/scene/EntityManager.h"
//#include "excimer/scene/EntityFactory.h"
// Cameras
#include "excimer/graphics/Camera/ThirdPersonCamera.h"
#include "excimer/graphics/Camera/FPSCamera.h"
#include "excimer/graphics/Camera/Camera2D.h"
#include "excimer/graphics/Camera/Camera.h"
// Maths
#include "excimer/maths/Maths.h"
#include "excimer/maths/Transform.h"

// Audio
//#include "Audio/AudioManager.h"
//#include "Audio/Sound.h"
//#include "Audio/SoundNode.h"

// System
#include "excimer/core/VFS.h"
#include "excimer/core/OS/FileSystem.h"
#include "excimer/core/StringUtilities.h"
#include "excimer/core/CoreSystem.h"
#include "excimer/core/EXLog.h"
#include "excimer/core/OS/Input.h"
#include "excimer/core/OS/OS.h"

// Scripting
//#include "Scripting/Lua/LuaScriptComponent.h"
//#include "Scripting/Lua/LuaManager.h"

// Utilities
#include "excimer/utilities/LoadImage.h"
#include "excimer/utilities/Timer.h"
#include "excimer/maths/Random.h"
#include "excimer/utilities/TimeStep.h"

#include "excimer/imgui/ImGuiManager.h"
//#include "excimer/graphics/Renderers/GridRenderer.h"
#include "excimer/graphics/Renderers/RenderGraph.h"

// External
#include <imgui/imgui.h>

