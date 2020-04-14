#pragma once
#include "PagedLOD/Tile.h"
#include "PagedLOD/Common.h"
#include <GLM/glm.hpp>

#include <vector>

namespace MockData
{
	const std::vector<std::string> getLostNodeSet()
	{
		std::vector<std::string> LostTileNodeSet;
		LostTileNodeSet.emplace_back("2_0");
		LostTileNodeSet.emplace_back("2_1");
		LostTileNodeSet.emplace_back("5_1");
		return LostTileNodeSet;
	}

	const std::vector<std::string> getExpiredNodeSet()
	{
		std::vector<std::string> ExpiredTileNodeSet;
		ExpiredTileNodeSet.emplace_back("a");
		ExpiredTileNodeSet.emplace_back("d_1");
		return ExpiredTileNodeSet;
	}

	const std::vector<std::pair<std::string, bool>> getExpectResult()
	{
		std::vector<std::pair<std::string, bool>>Result;
		Result.emplace_back("2_0", true);
		Result.emplace_back("2_1", true);
		Result.emplace_back("5_1", true);
		Result.emplace_back("a", false);
		Result.emplace_back("d_1", false);
		return Result;
	}

	hivePagedLOD::CMemoryBufferManager createMemoryBufferManager()
	{
		auto pPipelineR2M = std::make_shared<hivePagedLOD::CPipelineRenderingTileNodeGenerator2MemoryBufferManager>();
		pPipelineR2M->initInputBuffer();
		pPipelineR2M->initOutputBuffer();
		auto pPipelineM2GM = std::make_shared<hivePagedLOD::CPipelineMemoryBufferManager2GPUBufferManager>();
		pPipelineM2GM->initInputBuffer();
		pPipelineM2GM->initOutputBuffer();
		auto pPipelineM2L = std::make_shared<hivePagedLOD::CPipelineMemoryBufferManager2TileNodeLoader>();
		pPipelineM2L->initInputBuffer();
		pPipelineM2L->initOutputBuffer();
		auto pPipelineM2R = std::make_shared<hivePagedLOD::CPipelineMemoryBufferManager2RenderingTileNodeGenerator>();
		pPipelineM2R->initInputBuffer();
		pPipelineM2R->initOutputBuffer();


		hivePagedLOD::CMemoryBufferManager Kernal(pPipelineR2M, pPipelineM2GM, pPipelineM2L, pPipelineM2R);

		return Kernal;
	}
}