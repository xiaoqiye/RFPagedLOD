#include "pch.h"
#include "MockData.h"

TEST(TestCaseName, TestName) 
{
	auto pPipelineM2GM = std::make_shared<hivePagedLOD::CPipelineMemoryBufferManager2GPUBufferManager>();
	pPipelineM2GM->initInputBuffer();
	pPipelineM2GM->initOutputBuffer();
	auto pPipelineGM2GT = std::make_shared<hivePagedLOD::CPipelineGPUBufferManager2GPUThread>();
	pPipelineGM2GT->initInputBuffer();
	pPipelineGM2GT->initOutputBuffer();
	auto pPipelineGT2GM = std::make_shared<hivePagedLOD::CPipelineGPUThread2GPUBufferManager>();
	pPipelineGT2GM->initInputBuffer();
	pPipelineGT2GM->initOutputBuffer();

	hivePagedLOD::CGPUBufferManager GPUBufferManager(pPipelineM2GM, pPipelineGM2GT, pPipelineGT2GM);

	auto MemoryTask = MockData::getMemoryTask();
	pPipelineM2GM->try_push(MemoryTask);

	std::shared_ptr<SMemoryTask> OutputMemoryTask;
	pPipelineM2GM->try_pop(OutputMemoryTask);

	std::vector<std::shared_ptr<SGPUTask>> BufferTask;
	GPUBufferManager.processMemoryTask(OutputMemoryTask, BufferTask);
	EXPECT_EQ(BufferTask.size(), 8);

	GPUBufferManager.sendRequest(BufferTask);

	auto GPUResponse = MockData::getGPUResponse();
	pPipelineGT2GM->try_push(GPUResponse);
	std::vector<std::shared_ptr<SGPUBufferResponse>> OutputResponse;
	pPipelineGT2GM->try_pop(OutputResponse);

	std::vector<std::shared_ptr<SGPUMeshBuffer>> GPUMeshBufferSet;
	GPUBufferManager.processResponseAndBuildBufferSet(OutputResponse, MemoryTask, GPUMeshBufferSet);
	EXPECT_EQ(GPUMeshBufferSet.size(), 4);

	std::vector<std::shared_ptr<SGPUTask>> RenderTask;
	RenderTask.emplace_back(std::make_shared<SGPUTaskRender>(GPUMeshBufferSet));
	GPUBufferManager.sendRequest(RenderTask);
}