#include "pch.h"
#include "MockData.h"

class RenderingTileNodeGeneratorTest :public testing::Test 
{
protected:
	virtual void SetUp()
	{
		MockData::dumpCostMapPointer(m_TestCostMapPointer);
		MockData::dumpTestTileNodeSet(m_TestTileNodeSet);

		MockData::dumpDuplicateTestTileNodeSetAndTestDrawUIDSet(m_DuplicateTestTileNodeSet, m_DuplicateDrawUIDSet);
		MockData::ExtractNeedLoadTileNodeSet(m_DuplicateTestTileNodeSet, m_NeedLoadTileNodeSet);

		m_TestInputPipelineFromPreferred = std::make_shared<hivePagedLOD::CPipelinePreferred2RenderingTileNodeGenerator>();
		m_TestOutputPipelineToTileNodeLoader = std::make_shared < hivePagedLOD::CPipelineRenderingTileNodeGenerator2TileNodeLoader>();
		m_TestInputPipelineFromPreferred->initInputBuffer();
		m_TestInputPipelineFromPreferred->initOutputBuffer();
		m_TestOutputPipelineToTileNodeLoader->initInputBuffer();
		m_TestOutputPipelineToTileNodeLoader->initOutputBuffer();
	}

	void TearDown()
	{
		m_TestCostMapPointer.clear();
		m_TestTileNodeSet.clear();
		m_DuplicateTestTileNodeSet.clear();
		m_DuplicateDrawUIDSet.clear();
		m_NeedLoadTileNodeSet.clear();
	}

	std::vector<tbb::concurrent_unordered_map<unsigned int, hivePagedLOD::STileNodeLoadCost>> m_TestCostMapPointer;
	std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> m_TestTileNodeSet;

	std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> m_DuplicateTestTileNodeSet;
	std::set<unsigned int> m_DuplicateDrawUIDSet;

	std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> m_NeedLoadTileNodeSet;

	std::shared_ptr<hivePagedLOD::CPipelinePreferred2RenderingTileNodeGenerator> m_TestInputPipelineFromPreferred;
	std::shared_ptr<hivePagedLOD::CPipelineRenderingTileNodeGenerator2TileNodeLoader> m_TestOutputPipelineToTileNodeLoader;
};

//测试点：根据Mock的CostMap和Mock的存在于CostMap中的TileNode，计算当前的真实Cost和TriangleCount
TEST_F(RenderingTileNodeGeneratorTest, __calculateCostAndTriangleVertification) 
{
	uintmax_t ExpectCost = 10;
	uintmax_t ExpectTriangleCount = 25;
	uintmax_t ActualCost = 0;
	uintmax_t ActualTriangleCount = 0;

	//FIXME
	hivePagedLOD::CRenderingTileNodeGenerator TestRenderingTileNodeGenerator(m_TestInputPipelineFromPreferred, m_TestOutputPipelineToTileNodeLoader, &m_TestCostMapPointer);
	TestRenderingTileNodeGenerator.calculateCostAndTriangle(m_TestTileNodeSet, ActualCost, ActualTriangleCount);

	EXPECT_EQ(ExpectCost, ActualCost);
	EXPECT_EQ(ExpectTriangleCount, ActualTriangleCount);
}

//测试点：输入一组包含祖先节点的Tile节点集合和一组对应的UID集合，输出不包含祖先的TileNode集合和UID集合
TEST_F(RenderingTileNodeGeneratorTest, removeDrawUIDSetDuplicationVertification)
{
	std::set<unsigned int> ExpectUIDSet = { 0x00000001,0x01000001 };

	hivePagedLOD::CRenderingTileNodeGenerator TestRenderingTileNodeGenerator(m_TestInputPipelineFromPreferred, m_TestOutputPipelineToTileNodeLoader, &m_TestCostMapPointer);
	TestRenderingTileNodeGenerator.removeDrawUIDSetDuplication(m_DuplicateTestTileNodeSet, m_DuplicateDrawUIDSet);

	EXPECT_EQ(m_DuplicateTestTileNodeSet.size(), m_DuplicateDrawUIDSet.size());
	EXPECT_EQ(ExpectUIDSet.size(), m_DuplicateDrawUIDSet.size());
	unsigned int Size = ExpectUIDSet.size();
	for (unsigned int i = 0; i < Size; ++i)
	{
		EXPECT_TRUE(ExpectUIDSet.find(m_DuplicateTestTileNodeSet[i]->getUID()) != ExpectUIDSet.end());
		EXPECT_TRUE(m_DuplicateDrawUIDSet.find(m_DuplicateTestTileNodeSet[i]->getUID()) != m_DuplicateDrawUIDSet.end());
	}
}

//测试点：输入待加载的TileNode集合，返回得到LoadTask集合和需加载的数据量
//输入：当LimitLoadSize小于第一个待加载TileNode的真实数据加载量，共Mock三个待加载TileNode
//输出：至少会加载一个TileNode，返回该TileNode的LoadTask和数据加载量
TEST_F(RenderingTileNodeGeneratorTest, addTileNodeToLoadTaskSetVertification0)
{
	hivePagedLOD::CRenderingTileNodeGenerator TestRenderingTileNodeGenerator(m_TestInputPipelineFromPreferred, m_TestOutputPipelineToTileNodeLoader, &m_TestCostMapPointer);

	int TempLimitLoadSize = 10;
	TestRenderingTileNodeGenerator.setLoadLimitSizePerFrame(TempLimitLoadSize);

	std::vector<std::shared_ptr<hivePagedLOD::SLoadTask>> ExpectLoadTaskSet;
	auto LoadTask0 = std::make_shared<hivePagedLOD::SLoadTask>();
	LoadTask0->TileNode = m_NeedLoadTileNodeSet[0];
	LoadTask0->NeedLoadTexture = true;
	ExpectLoadTaskSet.emplace_back(LoadTask0);
	uintmax_t ExpectLoadSize = 15;

	std::vector<std::shared_ptr<hivePagedLOD::SLoadTask>> ActualLoadTaskSet;
	uintmax_t ActualLoadSize = 0;

	TestRenderingTileNodeGenerator.addTileNodeToLoadTaskSet(m_NeedLoadTileNodeSet, ActualLoadTaskSet, ActualLoadSize);

	EXPECT_EQ(ExpectLoadSize, ActualLoadSize);
	EXPECT_EQ(ExpectLoadTaskSet.size(), ActualLoadTaskSet.size());
	unsigned int Size = ExpectLoadTaskSet.size();
	for (unsigned int i = 0; i < Size; ++i)
	{
		EXPECT_EQ(ExpectLoadTaskSet[i]->TileNode->getUID(), ActualLoadTaskSet[i]->TileNode->getUID());
		EXPECT_EQ(ExpectLoadTaskSet[i]->NeedLoadTexture, ActualLoadTaskSet[i]->NeedLoadTexture);
	}
}

//测试点：输入待加载的TileNode集合，返回得到LoadTask集合和需加载的数据量
//输入：LimitLoadSize小于三个TileNode的加载量，大于两个TileNode 的加载量，共Mock三个待加载TileNode
//输出：3个TileNode均会加载，同时由于第2、3个TileNode属于兄弟节点，故纹理文件只加载一次
TEST_F(RenderingTileNodeGeneratorTest, addTileNodeToLoadTaskSetVertification1)
{
	hivePagedLOD::CRenderingTileNodeGenerator TestRenderingTileNodeGenerator(m_TestInputPipelineFromPreferred, m_TestOutputPipelineToTileNodeLoader, &m_TestCostMapPointer);

	int TempLimitLoadSize = 30;
	TestRenderingTileNodeGenerator.setLoadLimitSizePerFrame(TempLimitLoadSize);

	std::vector<std::shared_ptr<hivePagedLOD::SLoadTask>> ExpectLoadTaskSet;
	auto LoadTask0 = std::make_shared<hivePagedLOD::SLoadTask>();
	LoadTask0->TileNode = m_NeedLoadTileNodeSet[0];
	LoadTask0->NeedLoadTexture = true;
	ExpectLoadTaskSet.emplace_back(LoadTask0);
	auto LoadTask1 = std::make_shared<hivePagedLOD::SLoadTask>();
	LoadTask1->TileNode = m_NeedLoadTileNodeSet[1];
	LoadTask1->NeedLoadTexture = true;
	ExpectLoadTaskSet.emplace_back(LoadTask1);
	auto LoadTask2 = std::make_shared<hivePagedLOD::SLoadTask>();
	LoadTask2->TileNode = m_NeedLoadTileNodeSet[2];
	LoadTask2->NeedLoadTexture = true;
	ExpectLoadTaskSet.emplace_back(LoadTask2);
	uintmax_t ExpectLoadSize = 35;

	std::vector<std::shared_ptr<hivePagedLOD::SLoadTask>> ActualLoadTaskSet;
	uintmax_t ActualLoadSize = 0;

	TestRenderingTileNodeGenerator.addTileNodeToLoadTaskSet(m_NeedLoadTileNodeSet, ActualLoadTaskSet, ActualLoadSize);

	EXPECT_EQ(ExpectLoadSize, ActualLoadSize);
	EXPECT_EQ(ExpectLoadTaskSet.size(), ActualLoadTaskSet.size());
	unsigned int Size = ExpectLoadTaskSet.size();
	for (unsigned int i = 0; i < Size; ++i)
	{
		EXPECT_EQ(ExpectLoadTaskSet[i]->TileNode->getUID(), ActualLoadTaskSet[i]->TileNode->getUID());
		EXPECT_EQ(ExpectLoadTaskSet[i]->NeedLoadTexture, ActualLoadTaskSet[i]->NeedLoadTexture);
	}
}

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}