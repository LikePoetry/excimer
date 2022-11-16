#include "hzpch.h"
#include "Random.h"

namespace Excimer
{
    Random32 Random32::Rand = Random32(Random32::RandSeed());
    Random64 Random64::Rand = Random64(Random64::RandSeed());

    uint32_t Random32::RandSeed()
    {
        std::random_device randDevice;
        return randDevice();
    }

    uint64_t Random64::RandSeed()
    {
        std::random_device randDevice;
        uint64_t value = randDevice();
        return (value << 32) | randDevice();

    }
}
