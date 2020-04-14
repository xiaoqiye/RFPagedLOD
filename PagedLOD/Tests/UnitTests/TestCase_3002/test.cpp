#include "pch.h"
#include "MockData.h"

TEST(TestMeshLURList, TestPushTileNode) 
{
	auto LRUList = MockData::createMeshLRUList();
	//1 insert
	LRUList.push(SSplayElement("Tile_1_3.bin", 400, 400));
	LRUList.push(SSplayElement("Tile_1_2_1.bin", 300, 200));
	LRUList.push(SSplayElement("Tile_1_2_0.bin", 200, 200));
	LRUList.push(SSplayElement("Tile_1_1.bin", 100, 100));
	//1_1->1_2_0->1_2_1->1_3;	memsize = 1700
	EXPECT_EQ(LRUList.getCurrentMemorySize(), 1700);
	EXPECT_EQ(LRUList.getFrontNode().MeshName, "Tile_1_1.bin");

	//2 update
	LRUList.push(SSplayElement("Tile_1_2_0.bin", 200, 200));
	LRUList.push(SSplayElement("Tile_1_3.bin", 400, 400));
	//1_3->1_2_0->1_1->1_2_1;	memsize = 1700
	EXPECT_EQ(LRUList.getCurrentMemorySize(), 1700);
	EXPECT_EQ(LRUList.getFrontNode().MeshName, "Tile_1_3.bin");

	//3 delete
	LRUList.push(SSplayElement("Tile_2_2_0.bin", 400, 200));
	//2_2_0->1_3->1_2_0->1_1;	memsize = 2000
	EXPECT_EQ(LRUList.getCurrentMemorySize(), 2000);
	EXPECT_EQ(LRUList.getFrontNode().MeshName, "Tile_2_2_0.bin");
	EXPECT_EQ(LRUList.getBackNode().MeshName, "Tile_1_1.bin");


	LRUList.push(SSplayElement("Tile_2_2_1.bin", 500, 200));
	//2_2_1->2_2_0->1_3;		memsize = 1900
	EXPECT_EQ(LRUList.getCurrentMemorySize(), 1900);
	EXPECT_EQ(LRUList.getFrontNode().MeshName, "Tile_2_2_1.bin");
	EXPECT_EQ(LRUList.getBackNode().MeshName, "Tile_1_3.bin");

	LRUList.push(SSplayElement("Tile_1_3.bin", 400, 400));
	//1_3->2_2_0->2_2_1;		memsize = 1900
	EXPECT_EQ(LRUList.getCurrentMemorySize(), 1900);
	EXPECT_EQ(LRUList.getFrontNode().MeshName, "Tile_1_3.bin");
	EXPECT_EQ(LRUList.getBackNode().MeshName, "Tile_2_2_1.bin");

	LRUList.push(SSplayElement("Tile_3_3.bin", 800, 100));
	//3_3->1_3;		memsize = 1700
	EXPECT_EQ(LRUList.getCurrentMemorySize(), 1700);
	EXPECT_EQ(LRUList.getFrontNode().MeshName, "Tile_3_3.bin");
	EXPECT_EQ(LRUList.getBackNode().MeshName, "Tile_1_3.bin");
}