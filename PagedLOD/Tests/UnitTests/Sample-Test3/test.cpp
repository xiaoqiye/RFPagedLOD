#include "pch.h"
#include "MockData.h"
#include "Scene.h"

const std::string LODTREE_FOLDER_PATH = "H:\\PagedLOD_Resource\\Chengdu\\SerializedData10\\";
const std::string BIN_FOLDER_PATH = "H:\\PagedLOD_Resource\\Chengdu\\BinData\\";
const std::string BASEN_FOLDER_PATH = "";

using namespace hivePagedLOD;

TEST(TestPreferredKernal, TestName) 
{
	auto pPipelineV2P = std::make_shared<CPipelineFrameGenerator2PreferredTileNodeGenerator>();
	pPipelineV2P->initInputBuffer();
	pPipelineV2P->initOutputBuffer();
	auto pPipelineP2R = std::make_shared<CPipelinePreferred2RenderingTileNodeGenerator>();
	pPipelineP2R->initInputBuffer();
	pPipelineP2R->initOutputBuffer();
	CPreferredTileNodeGenerator PreferredTileNodeGenerator(pPipelineV2P, pPipelineP2R);

	CScene::getInstance()->init(LODTREE_FOLDER_PATH, BIN_FOLDER_PATH, BASEN_FOLDER_PATH, true, 1, 1);

	std::vector<SViewInfo> ViewInfoSet = MockData::makeViewInfoSet();
	std::vector<std::shared_ptr<CTileNode>> ExpectTileSet = MockData::makeExpectTile(CScene::getInstance()->getTileSet()[0]);

	pPipelineV2P->tryPush(ViewInfoSet[0]);
	std::vector<std::vector<std::shared_ptr<CTileNode>>> OutputBufferResult0;
	PreferredTileNodeGenerator.generatePreferredTileNodeSet(CScene::getInstance()->getTileSet(), OutputBufferResult0);
	pPipelineP2R->tryPop(OutputBufferResult0);

	pPipelineV2P->tryPush(ViewInfoSet[1]);
	std::vector<std::vector<std::shared_ptr<CTileNode>>> OutputBufferResult1;
	PreferredTileNodeGenerator.generatePreferredTileNodeSet(CScene::getInstance()->getTileSet(), OutputBufferResult1);
	pPipelineP2R->tryPop(OutputBufferResult1);

	//after move camera, tileSet[0][1] has been replaced by it`s three children tile node
	EXPECT_TRUE(OutputBufferResult0[0].size() + 2 == OutputBufferResult1[0].size());
	EXPECT_EQ(3, OutputBufferResult0[0][0]->getNumChildren());

	EXPECT_TRUE(MockData::isSameTileSet(ExpectTileSet, OutputBufferResult0[0][0]->getAllChildren()));
}