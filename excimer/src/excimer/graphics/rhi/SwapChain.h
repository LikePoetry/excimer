#pragma once
namespace Excimer
{
	class Window;

	namespace Graphics
	{
		class SwapChain
		{
		public:
			virtual ~SwapChain() = default;

			static SwapChain* Create(uint32_t width, uint32_t height);

			virtual bool Init(bool vsync, Window* window) = 0;
			virtual bool Init(bool vsync) = 0;

			virtual uint32_t GetCurrentBufferIndex() const = 0;

		protected:
			static SwapChain* (*CreateFunc)(uint32_t, uint32_t);
		};
	}
}