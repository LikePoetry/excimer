#pragma once
#include <stdint.h>
#include <atomic>
#include <functional>

struct JobDispatchArgs
{
	uint32_t jobIndex;
	uint32_t groupID;
	uint32_t groupIndex;		// group index relative to dispatch 
	bool isFirstJobInGroup;		// is the current job the first one in the group
	bool isLastJobInGroup;		
	void* sharedmemory;
};

namespace Excimer
{
	namespace System
	{
		namespace JobSystem
		{
			void OnInit(uint32_t maxThreadCount = ~0u);

			uint32_t GetThreadCount();

			struct Context
			{
				std::atomic<uint32_t> counter{ 0 };
			};

			// ���һ���첽ִ�е���ҵ���κο����̶߳���ִ�д���ҵ��
			void Excute(Context& ctx, const std::function<void(JobDispatchArgs)>& task);

			// ��һ����ҵ����Ϊ�����ҵ������ִ��
		   //	jobCount	: how many jobs to generate for this task.
		   //	groupSize	: how many jobs to execute per thread. Jobs inside a group execute serially. It might be worth to increase for small jobs
		   //	func		: receives a JobDispatchArgs as parameter
			void Dispatch(Context& ctx, uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs)>& task, size_t sharedmemory_size = 0);

			// Check if any threads are working currently or not
			bool IsBusy(const Context& ctx);

			// Wait until all threads become idle
			void Wait(const Context& ctx);
		}
	}
}