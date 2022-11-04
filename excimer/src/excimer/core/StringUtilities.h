#pragma once

namespace Excimer
{
    namespace StringUtilities
    {
        template <typename T>
        static std::string ToString(const T& input)
        {
            return std::to_string(input);
        }
    }
}