#include "pch.h"
#include "MockData.h"

TEST(TestProcessBufferAndSendBufferRequest, processBufferAndSendBufferRequest)
{
	auto GPUBufferManager = MockData::createGPUBufferMangager();
	auto MemoryTask = MockData::getMemoryTask();
	auto GPUMeshBufferMap = MockData::getGPUMeshBufferMap();

	GPUBufferManager.setGPUBufferMap(GPUMeshBufferMap);

	std::vector<std::shared_ptr<SGPUTask>> BufferTask;
	GPUBufferManager.processMemoryTask(MemoryTask, BufferTask);

	auto ExcpetResult = MockData::getExpectResult();

	EXPECT_EQ(ExcpetResult[0]->TaskType, EGPUTaskType::GEN_GEOMETRY_BUFFER);
	EXPECT_EQ(ExcpetResult[1]->TaskType, EGPUTaskType::GEN_GEOMETRY_BUFFER);
	EXPECT_EQ(ExcpetResult[2]->TaskType, EGPUTaskType::GEN_TEXTURE_BUFFER);

	EXPECT_EQ(ExcpetResult[3]->TaskType, EGPUTaskType::DEL_GEOMETRY_BUFFER);
	EXPECT_EQ(ExcpetResult[4]->TaskType, EGPUTaskType::DEL_GEOMETRY_BUFFER);
	EXPECT_EQ(ExcpetResult[5]->TaskType, EGPUTaskType::DEL_GEOMETRY_BUFFER);
	EXPECT_EQ(ExcpetResult[6]->TaskType, EGPUTaskType::DEL_GEOMETRY_BUFFER);
	EXPECT_EQ(ExcpetResult[7]->TaskType, EGPUTaskType::DEL_TEXTURE_BUFFER);
}