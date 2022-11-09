#pragma once
#include "excimer/maths/Maths.h"
#include "excimer/core/Core.h"
#include "excimer/core/ExLog.h"
#include "DescriptorSet.h"

namespace Excimer
{
	namespace Graphics
	{
		struct EXCIMER_EXPORT BufferElement
		{
			std::string name;
			RHIFormat format = RHIFormat::R32G32B32A32_Float;
			uint32_t offset = 0;
			bool Normalised = false;
		};

		class EXCIMER_EXPORT BufferLayout
		{
		private:
			uint32_t m_Size;
			std::vector<BufferElement> m_Layout;

		public:
			BufferLayout();

			template <typename T>
			void Push(const std::string& name, bool Normalised = false)
			{
				EXCIMER_ASSERT(false, "Unkown type!");
			}

			inline const std::vector<BufferElement>& GetLayout() const
			{
				return m_Layout;
			}
			inline uint32_t GetStride() const
			{
				return m_Size;
			}

		private:
			void Push(const std::string& name, RHIFormat format, uint32_t size, bool Normalised);
		};

		template <>
		void EXCIMER_EXPORT BufferLayout::Push<float>(const std::string& name, bool Normalised);
		template <>
		void EXCIMER_EXPORT BufferLayout::Push<uint32_t>(const std::string& name, bool Normalised);
		template <>
		void EXCIMER_EXPORT BufferLayout::Push<uint8_t>(const std::string& name, bool Normalised);
		template <>
		void EXCIMER_EXPORT BufferLayout::Push<glm::vec2>(const std::string& name, bool Normalised);
		template <>
		void EXCIMER_EXPORT BufferLayout::Push<glm::vec3>(const std::string& name, bool Normalised);
		template <>
		void EXCIMER_EXPORT BufferLayout::Push<glm::vec4>(const std::string& name, bool Normalised);
	}
}