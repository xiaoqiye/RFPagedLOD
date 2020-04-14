#include "pch.h"
#include "MockData.h"

TEST(TestProcessBufferAndBuildResponse, processBufferAndBuildResponse)
{
	auto GPUThread = MockData::createCGPUThread();
	auto GPUTaskSet = MockData::getGPUTaskSet();

	std::vector<std::shared_ptr<SGPUBufferResponse>> GPUResponseSet;
	GPUThread.processBufferAndBuildResponse(GPUTaskSet, GPUResponseSet);

	auto ExcpetResult = MockData::getExceptResultSet();
	EXPECT_EQ(GPUResponseSet[0]->getVertexArrayHandle(), ExcpetResult[0]->getVertexArrayHandle());
	EXPECT_EQ(GPUResponseSet[0]->getTextureID(), ExcpetResult[0]->getTextureID());
}