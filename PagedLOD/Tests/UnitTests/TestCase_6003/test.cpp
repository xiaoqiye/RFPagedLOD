#include "pch.h"

TEST(TestGPUThread, TestWork) 
{
	auto pPipelineGM2G = std::make_shared<hivePagedLOD::CPipelineGPUBufferManager2GPUThread>();
	pPipelineGM2G->initInputBuffer();
	pPipelineGM2G->initOutputBuffer();
	auto pPipelineG2GM = std::make_shared<hivePagedLOD::CPipelineGPUThread2GPUBufferManager>();
	pPipelineG2GM->initInputBuffer();
	pPipelineG2GM->initOutputBuffer();
	auto pPipelineFromGPUThread = std::make_shared<hivePagedLOD::CPipelineFromGPUThread>();
	pPipelineFromGPUThread->initInputBuffer();
	pPipelineFromGPUThread->initOutputBuffer();

	hivePagedLOD::CGPUThread GPUThread(pPipelineGM2G, pPipelineG2GM, pPipelineFromGPUThread);

	auto GPUBufferTaskSet = MockData::getGPUBufferTaskSet();

	pPipelineGM2G->try_push(GPUBufferTaskSet);
	std::vector<std::shared_ptr<SGPUTask>> OutputBufferTaskSet;
	pPipelineGM2G->try_pop(OutputBufferTaskSet);

	std::vector<std::shared_ptr<SGPUBufferResponse>> GPUResponseSet;
	GPUThread.processBufferAndBuildResponse(GPUBufferTaskSet, GPUResponseSet);

	pPipelineG2GM->try_push(GPUResponseSet);

	auto GPURenderTaskSet = MockData::getGPURenderTaskSet();

	OutputBufferTaskSet.clear();
	pPipelineGM2G->try_pop(OutputBufferTaskSet);
	GPUThread.render(OutputBufferTaskSet);

	std::shared_ptr<SFrameState> FrameState;
	GPUThread.updateFrameState(FrameState);

	pPipelineFromGPUThread->try_push(FrameState);
}