#pragma once

#include "excimer/core/os/KeyCodes.h"

#include <GLFW/glfw3.h>

#include <map>

namespace Excimer
{
    namespace GLFWKeyCodes
    {
        static Excimer::InputCode::Key GLFWToExcimerKeyboardKey(uint32_t glfwKey)
        {
            static std::map<uint32_t, Excimer::InputCode::Key> keyMap = {
                { GLFW_KEY_A, Excimer::InputCode::Key::A },
                { GLFW_KEY_B, Excimer::InputCode::Key::B },
                { GLFW_KEY_C, Excimer::InputCode::Key::C },
                { GLFW_KEY_D, Excimer::InputCode::Key::D },
                { GLFW_KEY_E, Excimer::InputCode::Key::E },
                { GLFW_KEY_F, Excimer::InputCode::Key::F },
                { GLFW_KEY_G, Excimer::InputCode::Key::G },
                { GLFW_KEY_H, Excimer::InputCode::Key::H },
                { GLFW_KEY_I, Excimer::InputCode::Key::I },
                { GLFW_KEY_J, Excimer::InputCode::Key::J },
                { GLFW_KEY_K, Excimer::InputCode::Key::K },
                { GLFW_KEY_L, Excimer::InputCode::Key::L },
                { GLFW_KEY_M, Excimer::InputCode::Key::M },
                { GLFW_KEY_N, Excimer::InputCode::Key::N },
                { GLFW_KEY_O, Excimer::InputCode::Key::O },
                { GLFW_KEY_P, Excimer::InputCode::Key::P },
                { GLFW_KEY_Q, Excimer::InputCode::Key::Q },
                { GLFW_KEY_R, Excimer::InputCode::Key::R },
                { GLFW_KEY_S, Excimer::InputCode::Key::S },
                { GLFW_KEY_T, Excimer::InputCode::Key::T },
                { GLFW_KEY_U, Excimer::InputCode::Key::U },
                { GLFW_KEY_V, Excimer::InputCode::Key::V },
                { GLFW_KEY_W, Excimer::InputCode::Key::W },
                { GLFW_KEY_X, Excimer::InputCode::Key::X },
                { GLFW_KEY_Y, Excimer::InputCode::Key::Y },
                { GLFW_KEY_Z, Excimer::InputCode::Key::Z },

                { GLFW_KEY_0, Excimer::InputCode::Key::D0 },
                { GLFW_KEY_1, Excimer::InputCode::Key::D1 },
                { GLFW_KEY_2, Excimer::InputCode::Key::D2 },
                { GLFW_KEY_3, Excimer::InputCode::Key::D3 },
                { GLFW_KEY_4, Excimer::InputCode::Key::D4 },
                { GLFW_KEY_5, Excimer::InputCode::Key::D5 },
                { GLFW_KEY_6, Excimer::InputCode::Key::D6 },
                { GLFW_KEY_7, Excimer::InputCode::Key::D7 },
                { GLFW_KEY_8, Excimer::InputCode::Key::D8 },
                { GLFW_KEY_9, Excimer::InputCode::Key::D9 },

                { GLFW_KEY_F1, Excimer::InputCode::Key::F1 },
                { GLFW_KEY_F2, Excimer::InputCode::Key::F2 },
                { GLFW_KEY_F3, Excimer::InputCode::Key::F3 },
                { GLFW_KEY_F4, Excimer::InputCode::Key::F4 },
                { GLFW_KEY_F5, Excimer::InputCode::Key::F5 },
                { GLFW_KEY_F6, Excimer::InputCode::Key::F6 },
                { GLFW_KEY_F7, Excimer::InputCode::Key::F7 },
                { GLFW_KEY_F8, Excimer::InputCode::Key::F8 },
                { GLFW_KEY_F9, Excimer::InputCode::Key::F9 },
                { GLFW_KEY_F10, Excimer::InputCode::Key::F10 },
                { GLFW_KEY_F11, Excimer::InputCode::Key::F11 },
                { GLFW_KEY_F12, Excimer::InputCode::Key::F12 },

                { GLFW_KEY_MINUS, Excimer::InputCode::Key::Minus },
                { GLFW_KEY_DELETE, Excimer::InputCode::Key::Delete },
                { GLFW_KEY_SPACE, Excimer::InputCode::Key::Space },
                { GLFW_KEY_LEFT, Excimer::InputCode::Key::Left },
                { GLFW_KEY_RIGHT, Excimer::InputCode::Key::Right },
                { GLFW_KEY_UP, Excimer::InputCode::Key::Up },
                { GLFW_KEY_DOWN, Excimer::InputCode::Key::Down },
                { GLFW_KEY_LEFT_SHIFT, Excimer::InputCode::Key::LeftShift },
                { GLFW_KEY_RIGHT_SHIFT, Excimer::InputCode::Key::RightShift },
                { GLFW_KEY_ESCAPE, Excimer::InputCode::Key::Escape },
                { GLFW_KEY_KP_ADD, Excimer::InputCode::Key::A },
                { GLFW_KEY_COMMA, Excimer::InputCode::Key::Comma },
                { GLFW_KEY_BACKSPACE, Excimer::InputCode::Key::Backspace },
                { GLFW_KEY_ENTER, Excimer::InputCode::Key::Enter },
                { GLFW_KEY_LEFT_SUPER, Excimer::InputCode::Key::LeftSuper },
                { GLFW_KEY_RIGHT_SUPER, Excimer::InputCode::Key::RightSuper },
                { GLFW_KEY_LEFT_ALT, Excimer::InputCode::Key::LeftAlt },
                { GLFW_KEY_RIGHT_ALT, Excimer::InputCode::Key::RightAlt },
                { GLFW_KEY_LEFT_CONTROL, Excimer::InputCode::Key::LeftControl },
                { GLFW_KEY_RIGHT_CONTROL, Excimer::InputCode::Key::RightControl },
                { GLFW_KEY_TAB, Excimer::InputCode::Key::Tab }
            };

            return keyMap[glfwKey];
        }

        static Excimer::InputCode::MouseKey GLFWToExcimerMouseKey(uint32_t glfwKey)
        {

            static std::map<uint32_t, Excimer::InputCode::MouseKey> keyMap = {
                { GLFW_MOUSE_BUTTON_LEFT, Excimer::InputCode::MouseKey::ButtonLeft },
                { GLFW_MOUSE_BUTTON_RIGHT, Excimer::InputCode::MouseKey::ButtonRight },
                { GLFW_MOUSE_BUTTON_MIDDLE, Excimer::InputCode::MouseKey::ButtonMiddle }
            };

            return keyMap[glfwKey];
        }
    }
}
