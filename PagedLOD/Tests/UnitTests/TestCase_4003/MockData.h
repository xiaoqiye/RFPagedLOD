#pragma once
#include "PagedLOD/Tile.h"
#include "PagedLOD/Common.h"
#include <GLM/glm.hpp>

#include <vector>

namespace MockData
{
	const std::shared_ptr<hivePagedLOD::SMemoryTask> getMemoryTask()
	{
		hivePagedLOD::SMemoryTask MemoryTask;
		MemoryTask.BufferSetToShow.emplace_back("1.bin", hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToShow.emplace_back("2_0.bin", hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToShow.emplace_back("2_1.bin", hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToShow.emplace_back("3.bin", hivePagedLOD::SMemoryMeshBuffer());

		MemoryTask.BufferSetToDelete.emplace_back("a_0.bin", hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToDelete.emplace_back("b_0.bin", hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToDelete.emplace_back("b_1.bin", hivePagedLOD::SMemoryMeshBuffer());
		MemoryTask.BufferSetToDelete.emplace_back("b_2.bin", hivePagedLOD::SMemoryMeshBuffer());

		return MemoryTask;
	}

	const std::map<std::string, std::shared_ptr<hivePagedLOD::SGPUMeshBuffer>> getGPUMeshBufferMap()
	{
		std::map<std::string, std::shared_ptr<hivePagedLOD::SGPUMeshBuffer>> GPUMeshBufferMap;
		GPUMeshBufferMap["1.bin"] = std::make_shared<hivePagedLOD::SGPUMeshBuffer>  (std::make_shared<CGPUTextureBuffer>(1), std::make_shared<CGPUGeometryBuffer>(1));
		GPUMeshBufferMap["2_0.bin"] = std::make_shared<hivePagedLOD::SGPUMeshBuffer>(std::make_shared<CGPUTextureBuffer>(2), std::make_shared<CGPUGeometryBuffer>(2));
		GPUMeshBufferMap["a_0.bin"] = std::make_shared<hivePagedLOD::SGPUMeshBuffer>(std::make_shared<CGPUTextureBuffer>(3), std::make_shared<CGPUGeometryBuffer>(3));
		GPUMeshBufferMap["a_1.bin"] = std::make_shared<hivePagedLOD::SGPUMeshBuffer>(std::make_shared<CGPUTextureBuffer>(3), std::make_shared<CGPUGeometryBuffer>(4));
		GPUMeshBufferMap["b_0.bin"] = std::make_shared<hivePagedLOD::SGPUMeshBuffer>(std::make_shared<CGPUTextureBuffer>(4), std::make_shared<CGPUGeometryBuffer>(5));
		GPUMeshBufferMap["b_1.bin"] = std::make_shared<hivePagedLOD::SGPUMeshBuffer>(std::make_shared<CGPUTextureBuffer>(4), std::make_shared<CGPUGeometryBuffer>(6));
		GPUMeshBufferMap["b_2.bin"] = std::make_shared<hivePagedLOD::SGPUMeshBuffer>(std::make_shared<CGPUTextureBuffer>(4), std::make_shared<CGPUGeometryBuffer>(7));
		return GPUMeshBufferMap;
	}

	const std::vector<std::shared_ptr<SGPUBufferResponse>> getGPUResponse()
	{
		std::vector<std::shared_ptr<SGPUBufferResponse>> GPUResponse;
		GPUResponse.emplace_back(std::make_shared<SGPUGeoBufferResponse>("2_1.bin", 8));
		GPUResponse.emplace_back(std::make_shared<SGPUGeoBufferResponse>("3.bin", 9));

		GPUResponse.emplace_back(std::make_shared<SGPUTexBufferResponse>("3.bint", 5));
		return GPUResponse;
	}
}