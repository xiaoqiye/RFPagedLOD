#pragma once
#include "PagedLOD/Tile.h"
#include "PagedLOD/Common.h"
#include <GLM/glm.hpp>

#include <vector>

namespace MockData
{
	const std::vector<std::string> getLostTileNodeSet()
	{
		std::vector<std::string>LostTileNodeSet;
		LostTileNodeSet.emplace_back("1.bin");
		LostTileNodeSet.emplace_back("2_0.bin");
		LostTileNodeSet.emplace_back("2_1.bin");
		LostTileNodeSet.emplace_back("3.bin");

		return LostTileNodeSet;
	}

	hivePagedLOD::CTileNodeLoader createTileNodeLoader()
	{

		auto pPipelineM2L = std::make_shared<hivePagedLOD::CPipelineMemoryBufferManager2TileNodeLoader>();
		pPipelineM2L->initInputBuffer();
		pPipelineM2L->initOutputBuffer();
		auto pPipelineL2M = std::make_shared<hivePagedLOD::CPipelineTileNodeLoader2MemoryBufferManager>();
		pPipelineL2M->initInputBuffer();
		pPipelineL2M->initOutputBuffer();

		hivePagedLOD::CTileNodeLoader Kernal(pPipelineM2L, pPipelineL2M);

		return Kernal;
	}

	const std::vector<std::shared_ptr<SMemoryMeshBuffer>> getExceptResultSet()
	{
		std::vector<std::shared_ptr<SMemoryMeshBuffer>> ResultSet;
		ResultSet.emplace_back(std::make_shared<SMemoryMeshBuffer>(std::make_shared<SGeometry>("1.bin"), std::make_shared<STexture>("1.bint")));
		ResultSet.emplace_back(std::make_shared<SMemoryMeshBuffer>(std::make_shared<SGeometry>("2_0.bin"), std::make_shared<STexture>("2.bint")));
		ResultSet.emplace_back(std::make_shared<SMemoryMeshBuffer>(std::make_shared<SGeometry>("2_1.bin"), std::make_shared<STexture>("2.bint")));
		ResultSet.emplace_back(std::make_shared<SMemoryMeshBuffer>(std::make_shared<SGeometry>("3.bin"), std::make_shared<STexture>("3.bint")));

		return ResultSet;
	}
}