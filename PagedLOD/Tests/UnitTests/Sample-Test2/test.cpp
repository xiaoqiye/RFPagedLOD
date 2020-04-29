#include "pch.h"
#include "MockData.h"

TEST(TestLOD, generatePreferredTileNode) 
{
	auto TileNodeSet = MockData::getTileNodeSet();
	auto ViewInfo = MockData::getViewInfo();
	auto ExpectResultSet = MockData::getExceptResultSet();

	auto pPipelineF2P = std::make_shared<hivePagedLOD::CPipelineFrameGenerator2PreferredTileNodeGenerator>();
	pPipelineF2P->initInputBuffer();
	pPipelineF2P->initOutputBuffer();
	auto pPipelineP2R = std::make_shared<hivePagedLOD::CPipelinePreferred2RenderingTileNodeGenerator>();
	pPipelineP2R->initInputBuffer();
	pPipelineP2R->initOutputBuffer();
	hivePagedLOD::CPreferredTileNodeGenerator PreferredTileNodeGenerator(pPipelineF2P, pPipelineP2R);

	PreferredTileNodeGenerator.start();
	
	auto Size = ExpectResultSet.size();
	for (auto i = Size; i > 0; --i)
	{
		ViewInfo.CameraInfo.Position = glm::vec3(0.0f, 0.0f, -50.0f * i);
		pPipelineF2P->tryPush(ViewInfo);
	}
	
	std::this_thread::sleep_for(std::chrono::seconds(5));
	
	//std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>> PreferredTileNodeSet;
	hivePagedLOD::SPreferredResult TempPreferedResult;
	for (auto i = Size; i > 0; --i)
	{
		if (PreferredTileNodeGenerator.fetchOutputPipeline()->tryPop(TempPreferedResult))
		{
			SCOPED_TRACE("row:" + std::to_string(i));
			//Tile1 will prefer one TileNode
			EXPECT_TRUE(MockData::isSameTileNode(ExpectResultSet[i - 1], TempPreferedResult.PreferredTileNodeSet[0][0]));
			//Tile2 is leaf node, it preferred anyway
			EXPECT_TRUE(MockData::isSameTileNode(TileNodeSet[TileNodeSet.size() - 1], TempPreferedResult.PreferredTileNodeSet[1][0]));
		}
	}
}