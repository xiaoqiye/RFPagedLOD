#include "pch.h"
#include "MockData.h"

TEST(TestBuildLostTileSet, buildLostTileSet)
{
	auto MemoryBufferManager = MockData::createMemoryBufferManager();
	auto RenderingTileNodeSet = MockData::getTileNodeSet();
	auto MemoryBufferMap = MockData::getMemoryBufferMap();
	MemoryBufferManager.setMemoryBufferMap(MemoryBufferMap);

	std::vector<std::string> LostTileNodeMap;
	MemoryBufferManager.buildLostTileSet(RenderingTileNodeSet, LostTileNodeMap);

	for (int i = 0; i < LostTileNodeMap.size(); ++i)
	{
		EXPECT_EQ(LostTileNodeMap[i], "Tile_2_2.bin");
		EXPECT_EQ(LostTileNodeMap[i], "Tile_2_3.bin");
		EXPECT_EQ(LostTileNodeMap[i], "Tile_1_7.bin");
		EXPECT_EQ(LostTileNodeMap[i], "Tile_1_8.bin");
	}
}