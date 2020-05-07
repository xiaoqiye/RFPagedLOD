#pragma once
#include "RenderingTileNodeGenerator.h"
#include"PipelinePreferred2RenderingTileNodeGenerator.h"
#include "PipelineRenderingTileNodeGenerator2TileNodeLoader.h"

namespace MockData
{
	const unsigned int TestTileNum = 2;
	void dumpCostMapPointer(std::vector<tbb::concurrent_unordered_map<unsigned int, hivePagedLOD::STileNodeLoadCost>>& voTempCostMapPointer)
	{
		voTempCostMapPointer.resize(TestTileNum);

		unsigned int TileNodeUID0 = 0x00000001;
		hivePagedLOD::STileNodeLoadCost TileNodeLoadCost0(5, 10, true, true, 15);
		unsigned int TileNodeUID1 = 0x00000002;
		hivePagedLOD::STileNodeLoadCost TileNodeLoadCost1(10, 5, false, false, 30);
		unsigned int TileNodeUID2 = 0x00000003;
		hivePagedLOD::STileNodeLoadCost TileNodeLoadCost2(5, 10, true, false, 30);
		unsigned int TileNodeUID3 = 0x01000001;
		hivePagedLOD::STileNodeLoadCost TileNodeLoadCost3(10, 5, true, false, 30);
		unsigned int TileNodeUID4 = 0x01000002;
		hivePagedLOD::STileNodeLoadCost TileNodeLoadCost4(5, 5, false, false, 10);
		unsigned int TileNodeUID5 = 0x01000003;
		hivePagedLOD::STileNodeLoadCost TileNodeLoadCost5(10, 5, false, false, 30);

		unsigned int TempTileNum = TileNodeUID0 & hivePagedLOD::UID_TILE_NUM_MASK;
		TempTileNum >>= hivePagedLOD::OFFSET_BIT;
		voTempCostMapPointer.at(TempTileNum)[TileNodeUID0] = TileNodeLoadCost0;

		TempTileNum= TileNodeUID1 & hivePagedLOD::UID_TILE_NUM_MASK;
		TempTileNum >>= hivePagedLOD::OFFSET_BIT;
		voTempCostMapPointer.at(TempTileNum)[TileNodeUID1] = TileNodeLoadCost1;

		TempTileNum = TileNodeUID2 & hivePagedLOD::UID_TILE_NUM_MASK;
		TempTileNum >>= hivePagedLOD::OFFSET_BIT;
		voTempCostMapPointer.at(TempTileNum)[TileNodeUID2] = TileNodeLoadCost2;

		TempTileNum = TileNodeUID3 & hivePagedLOD::UID_TILE_NUM_MASK;
		TempTileNum >>= hivePagedLOD::OFFSET_BIT;
		voTempCostMapPointer.at(TempTileNum)[TileNodeUID3] = TileNodeLoadCost3;

		TempTileNum = TileNodeUID4 & hivePagedLOD::UID_TILE_NUM_MASK;
		TempTileNum >>= hivePagedLOD::OFFSET_BIT;
		voTempCostMapPointer.at(TempTileNum)[TileNodeUID4] = TileNodeLoadCost4;

		TempTileNum = TileNodeUID5 & hivePagedLOD::UID_TILE_NUM_MASK;
		TempTileNum >>= hivePagedLOD::OFFSET_BIT;
		voTempCostMapPointer.at(TempTileNum)[TileNodeUID5] = TileNodeLoadCost5;
	}

	void dumpTestTileNodeSet(std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& voTestTileNodeSet)
	{
		auto TempTileNode0 = std::make_shared<hivePagedLOD::CTileNode>();
		TempTileNode0->setUID(0x00000001);

		auto TempTileNode1 = std::make_shared<hivePagedLOD::CTileNode>();
		TempTileNode1->setUID(0x01000002);

		voTestTileNodeSet.emplace_back(TempTileNode0);
		voTestTileNodeSet.emplace_back(TempTileNode1);
	}

	void dumpDuplicateTestTileNodeSetAndTestDrawUIDSet(std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& voDuplicateTestTileNodeSet, std::set<unsigned int>& voDuplicateDrawUIDSet)
	{
		auto TempTileNode0 = std::make_shared<hivePagedLOD::CTileNode>();
		TempTileNode0->setUID(0x00000001);
		TempTileNode0->setAncestorUIDSet({0x00000000});
		TempTileNode0->setTextureFileName("T1");

		auto TempTileNode1 = std::make_shared<hivePagedLOD::CTileNode>();
		TempTileNode1->setUID(0x00000002);
		TempTileNode1->setAncestorUIDSet({ 0x00000001 });
		TempTileNode1->setTextureFileName("T2");

		auto TempTileNode2 = std::make_shared<hivePagedLOD::CTileNode>();
		TempTileNode2->setUID(0x00000003);
		TempTileNode2->setAncestorUIDSet({ 0x00000002 });
		TempTileNode2->setTextureFileName("T3");

		auto TempTileNode3 = std::make_shared<hivePagedLOD::CTileNode>();
		TempTileNode3->setUID(0x01000001);
		TempTileNode3->setAncestorUIDSet({ 0x00000000 });
		TempTileNode3->setTextureFileName("T4");

		auto TempTileNode4 = std::make_shared<hivePagedLOD::CTileNode>();
		TempTileNode4->setUID(0x01000002);
		TempTileNode4->setAncestorUIDSet({ 0x01000001 });
		TempTileNode4->setTextureFileName("T5");

		auto TempTileNode5 = std::make_shared<hivePagedLOD::CTileNode>();
		TempTileNode5->setUID(0x01000003);
		TempTileNode5->setAncestorUIDSet({ 0x01000001 });
		TempTileNode5->setTextureFileName("T5");

		voDuplicateTestTileNodeSet.emplace_back(TempTileNode0);
		voDuplicateTestTileNodeSet.emplace_back(TempTileNode1);
		voDuplicateTestTileNodeSet.emplace_back(TempTileNode2);
		voDuplicateTestTileNodeSet.emplace_back(TempTileNode3);
		voDuplicateTestTileNodeSet.emplace_back(TempTileNode4);
		voDuplicateTestTileNodeSet.emplace_back(TempTileNode5);

		voDuplicateDrawUIDSet.insert(TempTileNode0->getUID());
		voDuplicateDrawUIDSet.insert(TempTileNode1->getUID());
		voDuplicateDrawUIDSet.insert(TempTileNode2->getUID());
		voDuplicateDrawUIDSet.insert(TempTileNode3->getUID());
		voDuplicateDrawUIDSet.insert(TempTileNode4->getUID());
		voDuplicateDrawUIDSet.insert(TempTileNode5->getUID());
	}

	void ExtractNeedLoadTileNodeSet(const std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& vDuplicateTestTileNodeSet, std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& voNeedLoadTileNodeSet)
	{
		voNeedLoadTileNodeSet.emplace_back(vDuplicateTestTileNodeSet[1]);
		voNeedLoadTileNodeSet.emplace_back(vDuplicateTestTileNodeSet[4]);
		voNeedLoadTileNodeSet.emplace_back(vDuplicateTestTileNodeSet[5]);
	}
}