#pragma once
#include "PagedLOD/Tile.h"
#include "PagedLOD/Common.h"
#include <GLM/glm.hpp>

#include <vector>

namespace MockData
{
	std::map<std::string, hivePagedLOD::STileNodeLoadCost> getTileNodeLoadCostMap()
	{
		std::map<std::string, hivePagedLOD::STileNodeLoadCost> LoadCostMap;
		LoadCostMap.insert("1", hivePagedLOD::STileNodeLoadCost(100, 100, 10, true, true, 0));
		LoadCostMap.insert("2", hivePagedLOD::STileNodeLoadCost(200, 200, 15, true, true, 0));
		LoadCostMap.insert("3", hivePagedLOD::STileNodeLoadCost(300, 150, 20, true, true, 0));
		LoadCostMap.insert("4", hivePagedLOD::STileNodeLoadCost(200, 100, 20, false, true, 200 + 100));
		LoadCostMap.insert("5", hivePagedLOD::STileNodeLoadCost(300, 100, 23, false, true, 300 + 100));
		LoadCostMap.insert("6", hivePagedLOD::STileNodeLoadCost(300, 100, 21, true, true, 0));
		LoadCostMap.insert("7", hivePagedLOD::STileNodeLoadCost(400, 200, 35, false, false, 400 + 200));
		LoadCostMap.insert("8", hivePagedLOD::STileNodeLoadCost(450, 200, 36, false, false, 450 + 200));
		LoadCostMap.insert("9", hivePagedLOD::STileNodeLoadCost(500, 150, 25, true, true, 0));
		LoadCostMap.insert("10", hivePagedLOD::STileNodeLoadCost(550, 100, 33, true, true, 0));
		return LoadCostMap;
	}

	const std::vector<std::pair<std::string, bool>> getUpdateDataSet()
	{
		std::vector<std::pair<std::string, bool>> UpdateTileNodeLoadCostSet;
		UpdateTileNodeLoadCostSet.emplace_back(1, false);
		UpdateTileNodeLoadCostSet.emplace_back(4, true);
		UpdateTileNodeLoadCostSet.emplace_back(7, true);
		UpdateTileNodeLoadCostSet.emplace_back(10, false);
		return UpdateTileNodeLoadCostSet;
	}

	const std::vector<std::tuple<bool, bool, unsigned int>> getExceptResultSet()
	{
		std::vector<std::tuple<bool, bool, unsigned int>>ResultSet;
		ResultSet.emplace_back(false, false, 100 + 100);
		ResultSet.emplace_back(true, false, 0);
		ResultSet.emplace_back(true, false, 0);
		ResultSet.emplace_back(true, true, 0);
		ResultSet.emplace_back(false, true, 300);
		ResultSet.emplace_back(true, true, 0);
		ResultSet.emplace_back(true, true, 0);
		ResultSet.emplace_back(false, true, 450);
		ResultSet.emplace_back(true, true, 0);
		ResultSet.emplace_back(false, false, 550 + 100);
		return ResultSet;
	}

	hivePagedLOD::RenderingTileNodeGenerator createRenderingTileNodeGenerator()
	{
		auto pPipelineP2R = std::make_shared<hivePagedLOD::CPipelinePreferred2RenderingTileNodeGenerator>();
		pPipelineP2R->initInputBuffer();
		pPipelineP2R->initOutputBuffer();
		auto pPipelineM2R = std::make_shared<hivePagedLOD::CPipelineMemoryBufferManager2RenderingTileNodeGenerator>();
		pPipelineM2R->initInputBuffer();
		pPipelineM2R->initOutputBuffer();
		auto pPipelineR2M = std::make_shared<hivePagedLOD::CPipelineRenderingTileNodeGenerator2MemoryBufferManager>();
		pPipelineR2M->initInputBuffer();
		pPipelineR2M->initOutputBuffer();
		
		hivePagedLOD::RenderingTileNodeGenerator Kernal(pPipelineP2R, pPipelineM2R, pPipelineR2M);

		return Kernal;
	}
}