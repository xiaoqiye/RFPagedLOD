#include "pch.h"
#include "MockData.h"

TEST(TestProcessResponseAndBuildBufferSet, processResponseAndBuildBufferSet)
{
	auto GPUBufferManager = MockData::createGPUBufferMangager();
	auto MemoryTask = MockData::getMemoryTask();
	auto GPUMeshBufferMap = MockData::getGPUMeshBufferMap();

	GPUBufferManager.setGPUBufferMap(GPUMeshBufferMap);

	std::vector<std::shared_ptr<SGPUMeshBuffer>> GPUMeshBufferSet;
	auto GPUResponseSet = MockData::getGPUResponse();
	GPUBufferManager.processResponseAndBuildBufferSet(GPUResponseSet, MemoryTask, GPUMeshBufferSet);

	EXPECT_EQ(GPUMeshBufferSet[0]->CGPUTextureBuffer->getTextureID(), 1);
	EXPECT_EQ(GPUMeshBufferSet[0]->CGPUGeometryBuffer->getVertexArrayHandle(), 1);

	EXPECT_EQ(GPUMeshBufferSet[1]->CGPUTextureBuffer->getTextureID(), 2);
	EXPECT_EQ(GPUMeshBufferSet[1]->CGPUGeometryBuffer->getVertexArrayHandle(), 2);

	EXPECT_EQ(GPUMeshBufferSet[2]->CGPUTextureBuffer->getTextureID(), 3);
	EXPECT_EQ(GPUMeshBufferSet[2]->CGPUGeometryBuffer->getVertexArrayHandle(), 2);

	EXPECT_EQ(GPUMeshBufferSet[3]->CGPUTextureBuffer->getTextureID(), 30);
	EXPECT_EQ(GPUMeshBufferSet[3]->CGPUGeometryBuffer->getVertexArrayHandle(), 40);
}