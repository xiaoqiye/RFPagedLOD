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
		MemoryTask.BufferSetToShow.emplace_back("1.bin",	hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToShow.emplace_back("2_0.bin",	hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToShow.emplace_back("2_1.bin",	hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToShow.emplace_back("3.bin",	hivePagedLOD::SMemoryMeshBuffer());

		MemoryTask.BufferSetToDelete.emplace_back("a_0.bin", hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToDelete.emplace_back("b_0.bin", hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToDelete.emplace_back("b_1.bin", hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToDelete.emplace_back("b_2.bin", hivePagedLOD::SMemoryMeshBuffer());

		return MemoryTask;
	}

	const std::map<std::string, std::shared_ptr<hivePagedLOD::SGPUMeshBuffer>> getGPUMeshBufferMap()
	{
		std::map<std::string, std::shared_ptr<hivePagedLOD::SGPUMeshBuffer>> GPUMeshBufferMap;
		GPUMeshBufferMap["1"] =   std::make_shared<hivePagedLOD::SGPUMeshBuffer>();
		GPUMeshBufferMap["2_0"] = std::make_shared<hivePagedLOD::SGPUMeshBuffer>();
		GPUMeshBufferMap["a_0"] = std::make_shared<hivePagedLOD::SGPUMeshBuffer>();
		GPUMeshBufferMap["a_1"] = std::make_shared<hivePagedLOD::SGPUMeshBuffer>();
		GPUMeshBufferMap["b_0"] = std::make_shared<hivePagedLOD::SGPUMeshBuffer>();
		GPUMeshBufferMap["b_1"] = std::make_shared<hivePagedLOD::SGPUMeshBuffer>();
		GPUMeshBufferMap["b_2"] = std::make_shared<hivePagedLOD::SGPUMeshBuffer>();
		return GPUMeshBufferMap;
	}

	const std::vector<std::shared_ptr<SGPUTask>> getExpectResult()
	{
		std::vector<std::shared_ptr<SGPUTask>> ResultSet;
		ResultSet.emplace_back(SGPUTaskGenGeoBuffer("2_1.bin"));
		ResultSet.emplace_back(SGPUTaskGenGeoBuffer("3.bin"));
		ResultSet.emplace_back(SGPUTaskGenTexBuffer("3.bint"));

		ResultSet.emplace_back(SGPUTaskDelGenBuffer("a_0.bin"));
		ResultSet.emplace_back(SGPUTaskDelGenBuffer("b_0.bin"));
		ResultSet.emplace_back(SGPUTaskDelGenBuffer("b_1.bin"));
		ResultSet.emplace_back(SGPUTaskDelGenBuffer("b_2.bin"));
		ResultSet.emplace_back(SGPUTaskDelTexBuffer("b.bint"));

		return ResultSet;
	}
}