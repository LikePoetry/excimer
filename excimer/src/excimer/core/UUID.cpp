#include "hzpch.h"
#include "UUID.h"

#include "excimer/maths/Random.h"

namespace Excimer
{
    UUID::UUID()
    {
        // Random uint64_t for now
        m_UUID = Random64::Rand(0, std::numeric_limits<uint64_t>::max());
    }

    UUID::UUID(uint64_t uuid)
        : m_UUID(uuid)
    {
    }

    UUID::UUID(const UUID& other)
        : m_UUID(other.m_UUID)
    {
    }
}