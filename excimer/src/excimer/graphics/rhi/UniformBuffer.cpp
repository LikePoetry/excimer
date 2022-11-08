#include "hzpch.h"
#include "UniformBuffer.h"

#include "excimer/core/Core.h"
#include "excimer/core/ExLog.h"
namespace Excimer
{
	namespace Graphics
	{
        UniformBuffer* (*UniformBuffer::CreateFunc)() = nullptr;
        UniformBuffer* (*UniformBuffer::CreateDataFunc)(uint32_t, const void*) = nullptr;

        UniformBuffer* UniformBuffer::Create()
        {
            EXCIMER_ASSERT(CreateFunc, "No UniformBuffer Create Function");

            return CreateFunc();
        }

        UniformBuffer* UniformBuffer::Create(uint32_t size, const void* data)
        {
            EXCIMER_ASSERT(CreateFunc, "No UniformBuffer Create Function");

            return CreateDataFunc(size, data);
        }
	}
}