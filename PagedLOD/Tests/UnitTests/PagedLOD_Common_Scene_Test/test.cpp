#include "pch.h"
#include "MockData.h"

class PagedLODCommonSceneTest :public testing::Test
{
protected:
	
	virtual void SetUp()
	{
		//m_TestNodeRecord = MockData::getTestNodeRecord();
	}

	const std::string m_TestNodeRecordFileName = "test.lodtree";
	const std::vector<hivePagedLOD::NodeRecord> m_TestNodeRecord;
};

//测试点：从文件中加载TileNode信息
//输入：测试文件输入流
//输出：TileNode的NodeRecord
TEST_F(PagedLODCommonSceneTest, loadNodeRecordFromFileVertification)
{
	unsigned int ExpectUID = 16777529;
	hivePagedLOD::CBoundingSphere ExpectBoundingSphere(163.562, 214.638, 465.493, 6.33679);
	const std::string ExpectGeoFileName = "Tile_1\\Tile_1_L21_000120t7_5.bin";
	const std::string ExpectTexFileName = "Tile_1\\Tile_1_L21_000120t7.bint"; 
	std::uintmax_t ExpectGeoFileSize = 207232;
	std::uintmax_t ExpectTexFileSize = 1048596;
	std::uintmax_t ExpectTriangleCount = 18855;
	unsigned int ExpectNodeDeep = 6;
	int ExpectLODLevel = 5;
	std::vector<unsigned int> ExpectAncestorUIDSet = { 16777216 ,16777217 ,16777218 ,16777219 ,16777369 ,16777497 ,0,0,0,0,0,0,0,0,0,0 };
	std::vector<unsigned int> ExpectBrotherUIDSet = { 16777498 ,16777501 ,16777503 ,16777506 ,16777512 ,0 ,16777540 ,16777554 };
	int ExpectFather = 281;

	auto ExpectTileNode = hivePagedLOD::CTileNode();
	ExpectTileNode.setUID(ExpectUID);
	ExpectTileNode.setBoundingSphere(ExpectBoundingSphere);
	ExpectTileNode.setGeometryFileName(ExpectGeoFileName);
	ExpectTileNode.setTextureFileName(ExpectTexFileName);
	ExpectTileNode.setGeometryFileSize(ExpectGeoFileSize);
	ExpectTileNode.setTextureFileSize(ExpectTexFileSize);
	ExpectTileNode.setTriangleCount(ExpectTriangleCount);
	ExpectTileNode.setNodeDeep(ExpectNodeDeep);
	ExpectTileNode.setLODLevel(ExpectLODLevel);
	ExpectTileNode.setAncestorUIDSet(ExpectAncestorUIDSet);
	ExpectTileNode.setBrotherUIDSet(ExpectBrotherUIDSet);

	std::ifstream TestNodeRecordInput(m_TestNodeRecordFileName, std::ios::in);
	if (!TestNodeRecordInput) throw std::exception("Can't open this file! Please check!");

	hivePagedLOD::NodeRecord ActualResult;
	ActualResult = hivePagedLOD::CScene::getInstance()->loadNodeRecordFromFile(TestNodeRecordInput);
	TestNodeRecordInput.close();

	//FIXME:
	EXPECT_TRUE(ExpectTileNode == ActualResult.first);
	EXPECT_EQ(ExpectFather, ActualResult.second);
}

//测试点：
TEST_F(PagedLODCommonSceneTest, parseRecordVertification) 
{
	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
}

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}