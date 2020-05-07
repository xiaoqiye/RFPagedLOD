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

//���Ե㣺����Mock��CostMap��Mock�Ĵ�����CostMap�е�TileNode�����㵱ǰ����ʵCost��TriangleCount
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

//���Ե㣺����һ��������Ƚڵ��Tile�ڵ㼯�Ϻ�һ���Ӧ��UID���ϣ�������������ȵ�TileNode���Ϻ�UID����
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

//���Ե㣺��������ص�TileNode���ϣ����صõ�LoadTask���Ϻ�����ص�������
//���룺��LimitLoadSizeС�ڵ�һ��������TileNode����ʵ���ݼ���������Mock����������TileNode
//��������ٻ����һ��TileNode�����ظ�TileNode��LoadTask�����ݼ�����
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

//���Ե㣺��������ص�TileNode���ϣ����صõ�LoadTask���Ϻ�����ص�������
//���룺LimitLoadSizeС������TileNode�ļ���������������TileNode �ļ���������Mock����������TileNode
//�����3��TileNode������أ�ͬʱ���ڵ�2��3��TileNode�����ֵܽڵ㣬�������ļ�ֻ����һ��
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