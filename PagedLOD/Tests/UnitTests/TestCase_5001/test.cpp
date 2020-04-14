#include "pch.h"
#include "MockData.h"

TEST(TestLoadFromFile, loadFromFile)
{
	auto LostTileNodeSet = MockData::getLostTileNodeSet();
	auto TileNodeLoader = MockData::createTileNodeLoader();
	auto MemoryBufferManager = MockData::createMemoryBufferManager();

	std::vector<std::shared_ptr<SMemoryMeshBuffer>> LoadedMemoryMeshBufferSet;
	TileNodeLoader.loadFromFile(LostTileNodeSet, LoadedMemoryMeshBufferSet);

	auto ResultSet = MockData::getExceptResultSet();
	EXPECT_EQ(ResultSet[0]->getGeoFileName(), LoadedMemoryMeshBufferSet[0]->getGeoFileName());
	EXPECT_EQ(ResultSet[0]->getTexFileName(), LoadedMemoryMeshBufferSet[0]->getTexFileName());
	EXPECT_EQ(ResultSet[1]->getGeoFileName(), LoadedMemoryMeshBufferSet[1]->getGeoFileName());
	EXPECT_EQ(ResultSet[1]->getTexFileName(), LoadedMemoryMeshBufferSet[1]->getTexFileName());
	EXPECT_EQ(ResultSet[2]->getGeoFileName(), LoadedMemoryMeshBufferSet[2]->getGeoFileName());
	EXPECT_EQ(ResultSet[2]->getTexFileName(), LoadedMemoryMeshBufferSet[2]->getTexFileName());
	EXPECT_EQ(ResultSet[3]->getGeoFileName(), LoadedMemoryMeshBufferSet[3]->getGeoFileName());
	EXPECT_EQ(ResultSet[3]->getTexFileName(), LoadedMemoryMeshBufferSet[3]->getTexFileName());
}