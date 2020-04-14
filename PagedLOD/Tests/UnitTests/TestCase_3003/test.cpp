#include "pch.h"
#include "MockData.h"

TEST(TestbuildLoadCostSet, buildLoadCostSet)
{
	auto MemoryBufferManager = MockData::createMemoryBufferManager();
	auto ExpectResult = MockData::getExpectResult();
	auto LostTileNodeSet = MockData::getLostNodeSet();
	auto ExpiredTileNodeSet = MockData::getExpectResult();

	std::vector<std::pair<std::string, bool>> UpdateTileNodeLoadCostSet;
	MemoryBufferManager.buildLoadCostSet(LostTileNodeSet, ExpiredTileNodeSet, UpdateTileNodeLoadCostSet);

	for (auto i = 0; i < UpdateTileNodeLoadCostSet.size(); ++i)
	{
		EXPECT_EQ(UpdateTileNodeLoadCostSet[i], ExpectResult[i]);
	}
}