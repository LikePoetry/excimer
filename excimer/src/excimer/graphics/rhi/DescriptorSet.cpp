#include "hzpch.h"
#include "excimer/core/Core.h"
#include "excimer/core/ExLog.h"
#include "DescriptorSet.h"

namespace Excimer
{
	namespace Graphics
	{
        DescriptorSet* (*DescriptorSet::CreateFunc)(const DescriptorDesc&) = nullptr;

        DescriptorSet* DescriptorSet::Create(const DescriptorDesc& desc)
        {
            EXCIMER_ASSERT(CreateFunc, "No DescriptorSet Create Function");

            return CreateFunc(desc);
        }
	}
}