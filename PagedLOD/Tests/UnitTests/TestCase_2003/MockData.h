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

		////Tile1
		LoadCostMap.insert("1.bin", hivePagedLOD::STileNodeLoadCost(100, 100, 10));
		LoadCostMap.insert("2.bin", hivePagedLOD::STileNodeLoadCost(200, 200, 15));
		LoadCostMap.insert("3.bin", hivePagedLOD::STileNodeLoadCost(300, 150, 20));
		LoadCostMap.insert("4.bin", hivePagedLOD::STileNodeLoadCost(200, 100, 20));
		LoadCostMap.insert("5.bin", hivePagedLOD::STileNodeLoadCost(300, 100, 23));
		LoadCostMap.insert("6.bin", hivePagedLOD::STileNodeLoadCost(300, 100, 21));
		LoadCostMap.insert("7.bin", hivePagedLOD::STileNodeLoadCost(400, 200, 35));
		LoadCostMap.insert("8.bin", hivePagedLOD::STileNodeLoadCost(450, 200, 36));
		LoadCostMap.insert("9.bin", hivePagedLOD::STileNodeLoadCost(500, 150, 25));
		LoadCostMap.insert("10.bin", hivePagedLOD::STileNodeLoadCost(550, 100, 33));
		LoadCostMap.insert("11.bin", hivePagedLOD::STileNodeLoadCost(400, 150, 24));
		LoadCostMap.insert("12.bin", hivePagedLOD::STileNodeLoadCost(450, 150, 26));
		LoadCostMap.insert("13.bin", hivePagedLOD::STileNodeLoadCost(400, 150, 31));
		LoadCostMap.insert("14.bin", hivePagedLOD::STileNodeLoadCost(500, 200, 43));
		LoadCostMap.insert("15.bin", hivePagedLOD::STileNodeLoadCost(500, 200, 45));
		LoadCostMap.insert("16.bin", hivePagedLOD::STileNodeLoadCost(600, 100, 40));
		LoadCostMap.insert("17.bin", hivePagedLOD::STileNodeLoadCost(550, 200, 50));
		LoadCostMap.insert("18.bin", hivePagedLOD::STileNodeLoadCost(650, 150, 49));
		return LoadCostMap;
	}

	const std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>> getPreferredTileNodeSet()
	{
		////7,8,5,6
		std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>> PreferredTileNodeSet(1);
		PreferredTileNodeSet[0].emplace_back(hivePagedLOD::CTileNode("7.bin"));
		PreferredTileNodeSet[0].emplace_back(hivePagedLOD::CTileNode("8.bin"));
		PreferredTileNodeSet[0].emplace_back(hivePagedLOD::CTileNode("5.bin"));
		PreferredTileNodeSet[0].emplace_back(hivePagedLOD::CTileNode("6.bin"));

		return PreferredTileNodeSet;
	}

	const std::vector<std::pair<std::string, bool>> getUpdateDataSet()
	{
		std::vector<std::pair<std::string, bool>> UpdateTileNodeLoadCostSet;
		UpdateTileNodeLoadCostSet.emplace_back(1, false);
		UpdateTileNodeLoadCostSet.emplace_back(4, true);
		return UpdateTileNodeLoadCostSet;
	}
}