#include "hzpch.h"
#include "StringUtilities.h"
#include <windows.h>
#include <DbgHelp.h>
#include <iomanip>

namespace Excimer
{
    namespace StringUtilities
    {

        bool StringContains(const std::string& string, const std::string& chars)
        {
            return string.find(chars) != std::string::npos;
        }
    }
}