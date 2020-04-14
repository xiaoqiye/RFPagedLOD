#include "pch.h"
#include "MockData.h"

TEST(TestCaseName, TestName) 
{
	auto pPipelineR2M = std::make_shared<hivePagedLOD::CPipelineRenderingTileNodeGenerator2MemoryBufferManager>();
	pPipelineR2M->initInputBuffer();
	pPipelineR2M->initOutputBuffer();
	auto pPipelineM2GM = std::make_shared<hivePagedLOD::CPipelineMemoryBufferManager2GPUBufferManager>();
	pPipelineM2GM->initInputBuffer();
	pPipelineM2GM->initOutputBuffer();
	auto pPipelineM2L = std::make_shared<hivePagedLOD::CPipelineMemoryBufferManager2TileNodeLoader>();
	pPipelineM2L->initInputBuffer();
	pPipelineM2L->initOutputBuffer();
	auto pPipelineM2R = std::make_shared<hivePagedLOD::CPipelineMemoryBufferManager2RenderingTileNodeGenerator>();
	pPipelineM2R->initInputBuffer();
	pPipelineM2R->initOutputBuffer();


	hivePagedLOD::CMemoryBufferManager MemoryBufferManager(pPipelineR2M, pPipelineM2GM, pPipelineM2L, pPipelineM2R);

	auto RenderingTileNodeSet = MockData::getRenderingTileNodeSet();
	pPipelineR2M->try_push(RenderingTileNodeSet);

	std::vector<std::shared_ptr<CTileNode>> OuputTileNodeSet;
	pPipelineR2M->try_pop(OuputTileNodeSet);
	size_t RenderingTileNodeSetTotalSize = 0;
	for (auto& i : RenderingTileNodeSet)
		RenderingTileNodeSetTotalSize += i.size();

	EXPECT_EQ(OuputTileNodeSet.size(), RenderingTileNodeSetTotalSize);


	auto MemoryBufferMap = MockData::getMemoryBufferMap();
	MemoryBufferManager.setMemoryBufferMap(MemoryBufferMap);
	std::vector<std::string> LostTileNodeSet;
	MemoryBufferManager.buildLostTileSet(OuputTileNodeSet, LostTileNodeSet);

	EXPECT_EQ(LostTileNodeSet[0], "Tile_1_7.bin");
	EXPECT_EQ(LostTileNodeSet[1], "Tile_1_8.bin");
	EXPECT_EQ(LostTileNodeSet[2], "Tile_2_2.bin");
	EXPECT_EQ(LostTileNodeSet[3], "Tile_2_3.bin");

	pPipelineM2L->try_push(LostTileNodeSet);

	//LRUList.push();
	//Tile_1_6.bin expired
	//std::vector<std::string> ExpiredTileNodeSet;
	//ExpiredTileNodeSet.emplace_back("Tile_1_6.bin");

	std::vector<std::pair<std::string, std::shared_ptr<SMemoryMeshBuffer>>>BufferToDelete;
	MemoryBufferManager.buildExpiredTileSet(BufferToDelete);

	EXPECT_EQ(BufferToDelete[0].first, "Tile_1_6.bin");

	std::vector<std::pair<std::string, std::shared_ptr<SMemoryMeshBuffer>>>BufferToShow;
	MemoryBufferManager.buildMeshBuffer(LostTileNodeSet, BufferToShow);

	EXPECT_EQ(BufferToShow.size(), RenderingTileNodeSetTotalSize);

	std::shared_ptr<SMemoryTask> MemoryTask;
	MemoryBufferManager.buildMemoryTask(BufferToDelete, BufferToShow, MemoryTask);

	pPipelineM2GM->try_push(MemoryTask);

	std::vector<std::pair<std::string, bool>> UpdateTileNodeLoadCostSet;
	MemoryBufferManager.buildLoadCostSet(LostTileNodeSet, ExpiredTileNodeSet, UpdateTileNodeLoadCostSet);

	pPipelineM2R->try_push(UpdateTileNodeLoadCostSet);
}