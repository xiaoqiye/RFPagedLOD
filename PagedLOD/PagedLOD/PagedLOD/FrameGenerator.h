#pragma once
#include "PipelineFrameGenerator2PreferredTileNodeGenerator.h"
#include "PipelineGPUThread2FrameGenerator.h"

#include <thread>

namespace hivePagedLOD
{
	class CFrameGenerator
	{
	public:
		CFrameGenerator() = delete;
		CFrameGenerator(const std::shared_ptr<CPipelineGPUThread2FrameGenerator>& vInputPipelineFromMemoryBufferManager,
			const std::shared_ptr<CPipelineFrameGenerator2PreferredTileNodeGenerator>& vOutputPipelineToFrameState);
		void start();
		void join() { _ASSERTE(m_Thread.joinable()); m_Thread.join(); }

		bool getEndFlag() const { return m_Close; }
		void setEndFlag(bool vFlag) { m_Close = vFlag; }

		uintmax_t getFrameID() const { return m_FrameID; }

		bool hasFrameState(std::shared_ptr<IFrameState>& voFrameState);
	private:
		void __workByFrameState();

		std::shared_ptr<CPipelineGPUThread2FrameGenerator> m_pInputPipelineFromGPUThread;
		std::shared_ptr<CPipelineFrameGenerator2PreferredTileNodeGenerator> m_pOutputPipelineToPreferred;

		std::shared_ptr<IFrameState> m_FrameState;
		bool m_HasFrameState = false;
		std::thread m_Thread;
		bool m_Close = false;
		uintmax_t m_FrameID = 0;
	};
}
