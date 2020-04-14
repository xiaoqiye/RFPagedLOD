#include "pch.h"
#include "MockData.h"

TEST(TestCaseName, TestName) 
{
	auto GPUThread = MockData::createCGPUThread();
	auto GPUTaskSet = MockData::getGPUTaskSet();

	GPUThread.render(GPUTaskSet);
}