#pragma once
#include "Scene.h"

namespace MockData
{
	std::vector<hivePagedLOD::NodeRecord> getTestNodeRecord()
	{
		const std::string TestNodeRecordFileName = "TestNodeRecord.lodtree";
		const int NodeRecordSize = 3;

		std::ifstream TestNodeRecordInput(TestNodeRecordFileName, std::ios::in);
		if (!TestNodeRecordInput) throw std::exception("Can't open this file! Please check!");

		std::vector<hivePagedLOD::NodeRecord> TempTestNodeRecord;
		
		for (unsigned int i = 0; i < NodeRecordSize; ++i)
			TempTestNodeRecord.emplace_back(hivePagedLOD::CScene::getInstance()->loadNodeRecordFromFile(TestNodeRecordInput));

		return TempTestNodeRecord;
	}
}