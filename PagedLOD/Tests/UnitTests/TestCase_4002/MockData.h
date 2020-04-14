#pragma once
#include "PagedLOD/Tile.h"
#include "PagedLOD/Common.h"
#include <GLM/glm.hpp>

#include <vector>

namespace MockData
{
	hivePagedLOD::CGPUBufferManager createGPUBufferMangager()
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


		hivePagedLOD::CGPUBufferManager Kernal(pPipelineM2GM, pPipelineGM2GT, pPipelineGT2GM);

		return Kernal;
	}

	const std::shared_ptr<hivePagedLOD::SMemoryTask> getMemoryTask()
	{
		hivePagedLOD::SMemoryTask MemoryTask;
		MemoryTask.BufferSetToShow.emplace_back("1.bin", hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToShow.emplace_back("2_0.bin", hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToShow.emplace_back("2_1.bin", hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToShow.emplace_back("3.bin", hivePagedLOD::SMemoryMeshBuffer());

		return MemoryTask;
	}
	const std::vector<std::shared_ptr<SGPUBufferResponse>> getGPUResponse()
	{
		std::vector<std::shared_ptr<SGPUBufferResponse>> GPUResponse;
		GPUResponse.emplace_back(std::make_shared<SGPUGeoBufferResponse>("1.bin", 1));
		GPUResponse.emplace_back(std::make_shared<SGPUGeoBufferResponse>("2_0.bin", 2));
		GPUResponse.emplace_back(std::make_shared<SGPUGeoBufferResponse>("2_1.bin", 3));
		GPUResponse.emplace_back(std::make_shared<SGPUGeoBufferResponse>("3.bin", 30));

		GPUResponse.emplace_back(std::make_shared<SGPUTexBufferResponse>("1.bint", 1));
		GPUResponse.emplace_back(std::make_shared<SGPUTexBufferResponse>("2.bint", 2));
		GPUResponse.emplace_back(std::make_shared<SGPUTexBufferResponse>("3.bint", 40));

		return GPUResponse;
	}
	const std::map<std::string, std::shared_ptr<hivePagedLOD::SGPUMeshBuffer>> getGPUMeshBufferMap()
	{
		std::map<std::string, std::shared_ptr<hivePagedLOD::SGPUMeshBuffer>> GPUMeshBufferMap;

		return GPUMeshBufferMap;
	}
}