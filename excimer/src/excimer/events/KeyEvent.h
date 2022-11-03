#pragma once

#include "Event.h"
#include "excimer/core/os/KeyCodes.h"

#include <sstream>

namespace Excimer
{

    class EXCIMER_EXPORT KeyEvent : public Event
    {
    public:
        inline Excimer::InputCode::Key GetKeyCode() const { return m_KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

    protected:
        KeyEvent(Excimer::InputCode::Key keycode)
            : m_KeyCode(keycode)
        {
        }

        Excimer::InputCode::Key m_KeyCode;
    };

    class EXCIMER_EXPORT KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(Excimer::InputCode::Key keycode, int repeatCount)
            : KeyEvent(keycode)
            , m_RepeatCount(repeatCount)
        {
        }

        inline int GetRepeatCount() const { return m_RepeatCount; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << uint32_t(m_KeyCode) << " (" << m_RepeatCount << " repeats)";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        int m_RepeatCount;
    };

    class EXCIMER_EXPORT KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(Excimer::InputCode::Key keycode)
            : KeyEvent(keycode)
        {
        }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << uint32_t(m_KeyCode);
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class EXCIMER_EXPORT KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent(Excimer::InputCode::Key keycode, char character)
            : KeyEvent(keycode)
            , Character(character)
        {
        }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << uint32_t(m_KeyCode);
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)

            char Character;

    private:
    };
}
