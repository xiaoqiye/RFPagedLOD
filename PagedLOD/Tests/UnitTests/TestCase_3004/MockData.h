#pragma once
#include "PagedLOD/Tile.h"
#include "PagedLOD/Common.h"
#include <GLM/glm.hpp>

#include <vector>

namespace MockData
{
	const std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>> getRenderingTileNodeSet()
	{
		std::vector<std::vector<std::shared_ptr<CTileNode>>> RenderingTileNodeSet(2);
		RenderingTileNodeSet[0].emplace_back(CTileNode("Tile_1_7.bin"));
		RenderingTileNodeSet[0].emplace_back(CTileNode("Tile_1_8.bin"));
		RenderingTileNodeSet[0].emplace_back(CTileNode("Tile_1_5.bin"));
		RenderingTileNodeSet[0].emplace_back(CTileNode("Tile_1_6.bin"));
		RenderingTileNodeSet[1].emplace_back(CTileNode("Tile_2_1.bin"));
		RenderingTileNodeSet[1].emplace_back(CTileNode("Tile_2_2.bin"));
		RenderingTileNodeSet[1].emplace_back(CTileNode("Tile_2_3.bin"));
		RenderingTileNodeSet[1].emplace_back(CTileNode("Tile_2_4.bin"));

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
}