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

        bool StringContains(const std::string& string, const std::string& chars);
    }
}