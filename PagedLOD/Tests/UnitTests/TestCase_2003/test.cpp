#include "pch.h"
#include "MockData.h"

const std::string LODTREE_FOLDER_PATH = "H:\\PagedLOD_Resource\\Chengdu\\SerializedData\\";
const std::string BIN_FOLDER_PATH = "H:\\PagedLOD_Resource\\Chengdu\\BinData\\";

TEST(TestRenderingTileNodeGeneratorKernal, TestWork)
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

	auto PreferredTileNodeSet = MockData::getPreferredTileNodeSet();

	pPipelineP2R->tryPush(PreferredTileNodeSet);
	std::vector<std::vector<std::shared_ptr<CTileNode>>> OutputBufferResult0;
	pPipelineP2R->tryPop(OutputBufferResult0);

	std::vector<std::vector<std::shared_ptr<CTileNode>>> RenderingTileNodeSet;

	auto LoadCostMap = MockData::getTileNodeLoadCostMap();
	LoadCostMap["1.bin"].GeoInMemory = true;
	LoadCostMap["1.bin"].TexInMemory = true;
	LoadCostMap["1.bin"].LoadCost = 0;
	LoadCostMap["3.bin"].GeoInMemory = true;
	LoadCostMap["3.bin"].TexInMemory = true;
	LoadCostMap["3.bin"].LoadCost = 0;
	LoadCostMap["5.bin"].GeoInMemory = true;
	LoadCostMap["5.bin"].TexInMemory = true;
	LoadCostMap["5.bin"].LoadCost = 0;	
	LoadCostMap["6.bin"].GeoInMemory = true;
	LoadCostMap["6.bin"].TexInMemory = true;
	LoadCostMap["6.bin"].LoadCost = 0;

	Kernal.setLoadCostMap(LoadCostMap);
	Kernal.setMaxLoadPerFrame(1000);

	Kernal.generateRenderingTileNodeByStrategy(hivePagedLOD::EStrategy::xxx, OutputBufferResult0, RenderingTileNodeSet);
	EXPECT_EQ(RenderingTileNodeSet.size(), 1);
	EXPECT_EQ(RenderingTileNodeSet[0].size(), 3);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "4.bin");

	pPipelineR2M->tryPush(RenderingTileNodeSet);

	auto UpdateDataSet = MockData::getUpdateDataSet();
	Kernal.updateTileNodeLoadCostTable(UpdateDataSet);
	auto NewMap = Kernal.getLoadCostMap();

	EXPECT_FALSE(["1.bin"].GeoInMemory);
	EXPECT_FALSE(["1.bin"].TexInMemory);
	EXPECT_EQ(["1.bin"].LoadCost, 200);
	EXPECT_TRUE(["4.bin"].GeoInMemory);
	EXPECT_TRUE(["4.bin"].TexInMemory);
	EXPECT_EQ(["4.bin"].LoadCost, 0);
}