#include "hzpch.h"
#include "Memory.h"
#include "Allocators/BinAllocator.h"
#include "Allocators/DefaultAllocator.h"
#include "Allocators/StbAllocator.h"

namespace Excimer
{
    Allocator* const Memory::MemoryAllocator = new DefaultAllocator();

    void* Memory::AlignedAlloc(size_t size, size_t alignment)
    {
        void* data;

        data = _aligned_malloc(size, alignment);

        return data;
    }

    void Memory::AlignedFree(void* data)
    {
        _aligned_free(data);

    }

    void* Memory::NewFunc(std::size_t size, const char* file, int line)
    {
        if (MemoryAllocator)
            return MemoryAllocator->Malloc(size, file, line);
        else
            return malloc(size);
    }

    void Memory::DeleteFunc(void* p)
    {
        if (MemoryAllocator)
            return MemoryAllocator->Free(p);
        else
            return free(p);
    }

    void Memory::LogMemoryInformation()
    {
        if (MemoryAllocator)
            return MemoryAllocator->Print();
    }
}

#ifdef CUSTOM_MEMORY_ALLOCATOR

void* operator new(std::size_t size)
{
    void* result = Excimer::Memory::NewFunc(size, __FILE__, __LINE__);
    if (result == nullptr)
    {
        throw std::bad_alloc();
    }
#if defined(EXCIMER_PROFILE) && defined(TRACY_ENABLE)
    TracyAlloc(result, size);
#endif
    return result;
}

void* operator new[](std::size_t size)
{
    void* result = Excimer::Memory::NewFunc(size, __FILE__, __LINE__);
    if (result == nullptr)
    {
        throw std::bad_alloc();
    }
#if defined(EXCIMER_PROFILE) && defined(TRACY_ENABLE)
    TracyAlloc(result, size);
#endif
    return result;
}

void operator delete(void* p) throw()
{
#if defined(EXCIMER_PROFILE) && defined(TRACY_ENABLE)
    TracyFree(p);
#endif
    Excimer::Memory::DeleteFunc(p);
}

void operator delete[](void* p) throw()
{
#if defined(EXCIMER_PROFILE) && defined(TRACY_ENABLE)
    TracyFree(p);
#endif
    Excimer::Memory::DeleteFunc(p);
}
#endif
