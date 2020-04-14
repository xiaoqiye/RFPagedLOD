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
		LoadCostMap.insert("1", hivePagedLOD::STileNodeLoadCost(100, 100, 10));
		LoadCostMap.insert("2", hivePagedLOD::STileNodeLoadCost(200, 200, 15));
		LoadCostMap.insert("3", hivePagedLOD::STileNodeLoadCost(300, 150, 20));
		LoadCostMap.insert("4", hivePagedLOD::STileNodeLoadCost(200, 100, 20));
		LoadCostMap.insert("5", hivePagedLOD::STileNodeLoadCost(300, 100, 23));
		LoadCostMap.insert("6", hivePagedLOD::STileNodeLoadCost(300, 100, 21));
		LoadCostMap.insert("7", hivePagedLOD::STileNodeLoadCost(400, 200, 35));
		LoadCostMap.insert("8", hivePagedLOD::STileNodeLoadCost(450, 200, 36));
		LoadCostMap.insert("9", hivePagedLOD::STileNodeLoadCost(500, 150, 25));
		LoadCostMap.insert("10", hivePagedLOD::STileNodeLoadCost(550, 100, 33));
		LoadCostMap.insert("11", hivePagedLOD::STileNodeLoadCost(400, 150, 24));
		LoadCostMap.insert("12", hivePagedLOD::STileNodeLoadCost(450, 150, 26));
		LoadCostMap.insert("13", hivePagedLOD::STileNodeLoadCost(400, 150, 31));
		LoadCostMap.insert("14", hivePagedLOD::STileNodeLoadCost(500, 200, 43));
		LoadCostMap.insert("15", hivePagedLOD::STileNodeLoadCost(500, 200, 45));
		LoadCostMap.insert("16", hivePagedLOD::STileNodeLoadCost(600, 100, 40));
		LoadCostMap.insert("17", hivePagedLOD::STileNodeLoadCost(550, 200, 50));
		LoadCostMap.insert("18", hivePagedLOD::STileNodeLoadCost(650, 150, 49));

		////Tile2
		LoadCostMap.insert("a", hivePagedLOD::STileNodeLoadCost(100, 100, 10));
		LoadCostMap.insert("b", hivePagedLOD::STileNodeLoadCost(200, 200, 15));
		LoadCostMap.insert("c", hivePagedLOD::STileNodeLoadCost(300, 150, 20));
		LoadCostMap.insert("d", hivePagedLOD::STileNodeLoadCost(100, 100, 20));
		LoadCostMap.insert("e", hivePagedLOD::STileNodeLoadCost(100, 100, 23));
		LoadCostMap.insert("f", hivePagedLOD::STileNodeLoadCost(150, 100, 21));
		LoadCostMap.insert("g", hivePagedLOD::STileNodeLoadCost(100, 100, 22));
		LoadCostMap.insert("h", hivePagedLOD::STileNodeLoadCost(150, 200, 25));
		LoadCostMap.insert("i", hivePagedLOD::STileNodeLoadCost(200, 200, 28));
		LoadCostMap.insert("j", hivePagedLOD::STileNodeLoadCost(200, 200, 25));
		LoadCostMap.insert("k", hivePagedLOD::STileNodeLoadCost(250, 200, 32));
		LoadCostMap.insert("l", hivePagedLOD::STileNodeLoadCost(150, 200, 22));
		LoadCostMap.insert("m", hivePagedLOD::STileNodeLoadCost(200, 200, 24));
		LoadCostMap.insert("n", hivePagedLOD::STileNodeLoadCost(400, 100, 43));
		LoadCostMap.insert("o", hivePagedLOD::STileNodeLoadCost(500, 100, 49));

		////Tile3
		LoadCostMap.insert("A", hivePagedLOD::STileNodeLoadCost(100, 100, 10));
		LoadCostMap.insert("B", hivePagedLOD::STileNodeLoadCost(200, 200, 15));
		LoadCostMap.insert("C", hivePagedLOD::STileNodeLoadCost(300, 150, 20));
		LoadCostMap.insert("D", hivePagedLOD::STileNodeLoadCost(100, 100, 10));
		LoadCostMap.insert("E", hivePagedLOD::STileNodeLoadCost(150, 100, 13));
		LoadCostMap.insert("F", hivePagedLOD::STileNodeLoadCost(100, 100, 11));
		LoadCostMap.insert("G", hivePagedLOD::STileNodeLoadCost(100, 200, 11));
		LoadCostMap.insert("H", hivePagedLOD::STileNodeLoadCost(150, 200, 14));
		LoadCostMap.insert("I", hivePagedLOD::STileNodeLoadCost(200, 200, 21));
		LoadCostMap.insert("J", hivePagedLOD::STileNodeLoadCost(250, 200, 30));
		LoadCostMap.insert("K", hivePagedLOD::STileNodeLoadCost(300, 150, 34));
		LoadCostMap.insert("L", hivePagedLOD::STileNodeLoadCost(350, 150, 36));

		return LoadCostMap;
	}

	const std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>> getPreferredTileNodeSetByID(unsigned int vID)
	{
		////1	1
		std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>> PreferredTileNodeSet1(1);
		PreferredTileNodeSet1[0].emplace_back(hivePagedLOD::CTileNode("1.bin"));

		////2	3
		std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>> PreferredTileNodeSet2(1);
		PreferredTileNodeSet2[0].emplace_back(hivePagedLOD::CTileNode("3.bin"));
		
		////3	7,8,5,6
		std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>> PreferredTileNodeSet3(1);
		PreferredTileNodeSet3[0].emplace_back(hivePagedLOD::CTileNode("7.bin"));
		PreferredTileNodeSet3[0].emplace_back(hivePagedLOD::CTileNode("8.bin"));
		PreferredTileNodeSet3[0].emplace_back(hivePagedLOD::CTileNode("5.bin"));
		PreferredTileNodeSet3[0].emplace_back(hivePagedLOD::CTileNode("6.bin"));

		////4	7,8,10,11,14,15,18
		std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>> PreferredTileNodeSet4(1);
		PreferredTileNodeSet4[0].emplace_back(hivePagedLOD::CTileNode("7.bin"));
		PreferredTileNodeSet4[0].emplace_back(hivePagedLOD::CTileNode("8.bin"));
		PreferredTileNodeSet4[0].emplace_back(hivePagedLOD::CTileNode("10.bin"));
		PreferredTileNodeSet4[0].emplace_back(hivePagedLOD::CTileNode("11.bin"));
		PreferredTileNodeSet4[0].emplace_back(hivePagedLOD::CTileNode("14.bin"));
		PreferredTileNodeSet4[0].emplace_back(hivePagedLOD::CTileNode("15.bin"));
		PreferredTileNodeSet4[0].emplace_back(hivePagedLOD::CTileNode("18.bin"));

		////5	7,8,5,6,h,i,j,k,l,m,e,f,g,D,E,F
		std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>> PreferredTileNodeSet5(3);
		PreferredTileNodeSet5[0].emplace_back(hivePagedLOD::CTileNode("7.bin"));
		PreferredTileNodeSet5[0].emplace_back(hivePagedLOD::CTileNode("8.bin"));
		PreferredTileNodeSet5[0].emplace_back(hivePagedLOD::CTileNode("5.bin"));
		PreferredTileNodeSet5[0].emplace_back(hivePagedLOD::CTileNode("6.bin"));
		PreferredTileNodeSet5[1].emplace_back(hivePagedLOD::CTileNode("h.bin"));
		PreferredTileNodeSet5[1].emplace_back(hivePagedLOD::CTileNode("i.bin"));
		PreferredTileNodeSet5[1].emplace_back(hivePagedLOD::CTileNode("j.bin"));
		PreferredTileNodeSet5[1].emplace_back(hivePagedLOD::CTileNode("k.bin"));
		PreferredTileNodeSet5[1].emplace_back(hivePagedLOD::CTileNode("l.bin"));
		PreferredTileNodeSet5[1].emplace_back(hivePagedLOD::CTileNode("m.bin"));
		PreferredTileNodeSet5[1].emplace_back(hivePagedLOD::CTileNode("e.bin"));
		PreferredTileNodeSet5[1].emplace_back(hivePagedLOD::CTileNode("f.bin"));
		PreferredTileNodeSet5[1].emplace_back(hivePagedLOD::CTileNode("g.bin"));
		PreferredTileNodeSet5[2].emplace_back(hivePagedLOD::CTileNode("D.bin"));
		PreferredTileNodeSet5[2].emplace_back(hivePagedLOD::CTileNode("E.bin"));
		PreferredTileNodeSet5[2].emplace_back(hivePagedLOD::CTileNode("F.bin"));

		////6	7,8,10,6,h,n,j,k,l,m,e,f,g,G,H,I,J,E,F
		std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>> PreferredTileNodeSet6(3);
		PreferredTileNodeSet6[0].emplace_back(hivePagedLOD::CTileNode("7.bin"));
		PreferredTileNodeSet6[0].emplace_back(hivePagedLOD::CTileNode("8.bin"));
		PreferredTileNodeSet6[0].emplace_back(hivePagedLOD::CTileNode("10.bin"));
		PreferredTileNodeSet6[0].emplace_back(hivePagedLOD::CTileNode("6.bin"));
		PreferredTileNodeSet6[1].emplace_back(hivePagedLOD::CTileNode("h.bin"));
		PreferredTileNodeSet6[1].emplace_back(hivePagedLOD::CTileNode("n.bin"));
		PreferredTileNodeSet6[1].emplace_back(hivePagedLOD::CTileNode("j.bin"));
		PreferredTileNodeSet6[1].emplace_back(hivePagedLOD::CTileNode("k.bin"));
		PreferredTileNodeSet6[1].emplace_back(hivePagedLOD::CTileNode("l.bin"));
		PreferredTileNodeSet6[1].emplace_back(hivePagedLOD::CTileNode("m.bin"));
		PreferredTileNodeSet6[1].emplace_back(hivePagedLOD::CTileNode("e.bin"));
		PreferredTileNodeSet6[1].emplace_back(hivePagedLOD::CTileNode("f.bin"));
		PreferredTileNodeSet6[1].emplace_back(hivePagedLOD::CTileNode("g.bin"));
		PreferredTileNodeSet6[2].emplace_back(hivePagedLOD::CTileNode("G.bin"));
		PreferredTileNodeSet6[2].emplace_back(hivePagedLOD::CTileNode("H.bin"));
		PreferredTileNodeSet6[2].emplace_back(hivePagedLOD::CTileNode("I.bin"));
		PreferredTileNodeSet6[2].emplace_back(hivePagedLOD::CTileNode("J.bin"));
		PreferredTileNodeSet6[2].emplace_back(hivePagedLOD::CTileNode("E.bin"));
		PreferredTileNodeSet6[2].emplace_back(hivePagedLOD::CTileNode("F.bin"));

		switch (vID)
		{
		case 1:
			return PreferredTileNodeSet1;
		case 2:
			return PreferredTileNodeSet2;
		case 3:
			return PreferredTileNodeSet3;
		case 4:
			return PreferredTileNodeSet4;
		case 5:
			return PreferredTileNodeSet5;
		case 6:
			return PreferredTileNodeSet6;
		default:
			_ASSERT(false);
		}

		return std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>>();
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