#pragma once
#include "GPUBufferInterface.h"
#include "PipelineMemoryBufferManager2GPUThread.h"
#include "GPUBufferManager.h"
#include "GPUTask.h"
#include "PipelineGPUThread2FrameGenerator.h"

#include <thread>

namespace hivePagedLOD
{
	class CGPUThread
	{
	public:
		CGPUThread() = delete;
		CGPUThread(const std::shared_ptr<CPipelineMemoryBufferManager2GPUThread>& vInputPipelineFromMemoryBufferManager,
			const std::shared_ptr<CPipelineGPUThread2FrameGenerator>& vOutputPipelineToFrameGenerator,
			const std::shared_ptr<IGPUBufferInterface>& vGPUInterface);
		void start();
		void join() { _ASSERTE(m_Thread.joinable()); m_Thread.join(); }
		void setFinishSignal(bool vFinish) { m_Close = vFinish; }

		void doFirstFrame(unsigned int vWindowWidth, unsigned int vWindowHeight, unsigned int vMultiFrameCount);
		void clearGPUBuffers();

		std::shared_ptr<CGPUBufferManager>& fetchGPUBufferManager() { return m_pGPUBufferManager; }
		std::shared_ptr<IGPUBufferInterface>& fetchGPUInterface() { return m_pGPUInterface; }
	private:
		void __processTaskQueue();

		void __generateGeometryBuffer(const std::shared_ptr<SGPUTask>& vTask) const;
		void __generateTextureBuffer(const std::shared_ptr<SGPUTask>& vTask) const;
		void __deleteTextureBuffer(const std::shared_ptr<SGPUTask>& vTask) const;
		void __deleteGeometryBuffer(const std::shared_ptr<SGPUTask>& vTask) const;

		void __mainLoop(const std::shared_ptr<SGPUTask>& vTask);
		void __init(const std::shared_ptr<SGPUTask>& vTask) const;

		std::thread m_Thread;
		bool m_Close = false;

		std::shared_ptr<CPipelineMemoryBufferManager2GPUThread> m_pInputPipelineFromMemoryBufferManager;
		std::shared_ptr<CPipelineGPUThread2FrameGenerator> m_pOutputPipelineToFrameGenerator;
		std::shared_ptr<IGPUBufferInterface> m_pGPUInterface;

		std::shared_ptr<CGPUBufferManager> m_pGPUBufferManager;
	};
}
