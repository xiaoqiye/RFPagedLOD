#pragma once
#include "PagedLOD/Tile.h"
#include "PagedLOD/Common.h"
#include <GLM/glm.hpp>

#include <vector>

namespace MockData
{
	hivePagedLOD::CGPUThread createCGPUThread()
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

		hivePagedLOD::CGPUThread Kernal(pPipelineGM2G, pPipelineG2GM, pPipelineFromGPUThread);

		return Kernal;
	}

	const std::vector<std::shared_ptr<SGPUTask>> getGPUTaskSet()
	{
		std::vector<std::shared_ptr<SGPUTask>> GPUTaskSet;
		std::shared_ptr<SGPUTaskRender> TaskRender;
		//todo: mock geo handle and tex id

		GPUTaskSet.emplace_back(TaskRender);

		return GPUTaskSet;
	}
}