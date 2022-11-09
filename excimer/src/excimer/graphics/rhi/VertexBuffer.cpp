#include "hzpch.h"
#include "VertexBuffer.h"

namespace Excimer
{
    namespace Graphics
    {
        VertexBuffer* (*VertexBuffer::CreateFunc)(const BufferUsage&) = nullptr;

        VertexBuffer* VertexBuffer::Create(const BufferUsage& usage)
        {
            EXCIMER_ASSERT(CreateFunc, "No VertexBuffer Create Function");

            return CreateFunc(usage);
        }
    }
}