#pragma once
#include "PagedLOD/Tile.h"
#include "PagedLOD/Common.h"
#include <GLM/glm.hpp>

#include <vector>

namespace MockData
{
	const std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> getTileNodeSet()
	{
		std::vector<std::vector<std::shared_ptr<CTileNode>>> RenderingTileNodeSet(2);
		RenderingTileNodeSet[0].emplace_back(CTileNode("Tile_1_7"));
		RenderingTileNodeSet[0].emplace_back(CTileNode("Tile_1_8"));
		RenderingTileNodeSet[0].emplace_back(CTileNode("Tile_1_5"));
		RenderingTileNodeSet[0].emplace_back(CTileNode("Tile_1_6"));
		RenderingTileNodeSet[1].emplace_back(CTileNode("Tile_2_1"));
		RenderingTileNodeSet[1].emplace_back(CTileNode("Tile_2_2"));
		RenderingTileNodeSet[1].emplace_back(CTileNode("Tile_2_3"));
		RenderingTileNodeSet[1].emplace_back(CTileNode("Tile_2_4"));

		return RenderingTileNodeSet;
	}

	std::map<std::string, std::shared_ptr<SMemoryMeshBuffer>> getMemoryBufferMap()
	{
		std::map<std::string, std::shared_ptr<SMemoryMeshBuffer>> MemoryMeshBufferMap;
		MemoryMeshBufferMap.emplace_back("Tile_2_1.bin", );
		MemoryMeshBufferMap.emplace_back("Tile_2_4.bin", );
		MemoryMeshBufferMap.emplace_back("Tile_1_5.bin", );
		MemoryMeshBufferMap.emplace_back("Tile_1_6.bin", );

		return MemoryMeshBufferMap;
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