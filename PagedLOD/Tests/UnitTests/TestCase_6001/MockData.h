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
		GPUTaskSet.emplace_back(std::make_shared<SGPUTaskDelGeoBuffer>("0.bin"));
		GPUTaskSet.emplace_back(std::make_shared<SGPUTaskDelTexBuffer>("0.bint"));
		GPUTaskSet.emplace_back(std::make_shared<SGPUTaskGenGeoBuffer>("1.bin"));
		GPUTaskSet.emplace_back(std::make_shared<SGPUTaskGenTexBuffer>("1.bint"));

		return GPUTaskSet;
	}

	const std::vector<std::shared_ptr<SGPUResponse>> getExceptResultSet()
	{
		std::vector<std::shared_ptr<SGPUResponse>> GPUResponse;
		//maybe not 2
		GPUResponse.emplace_back(std::make_shared<SGPUGeoBufferResponse>("1.bin", 2));
		GPUResponse.emplace_back(std::make_shared<SGPUTexBufferResponse>("1.bint", 2));

		return GPUResponse;
	}
}