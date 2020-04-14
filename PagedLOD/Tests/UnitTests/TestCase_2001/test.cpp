#include "pch.h"
#include "MockData.h"

////一个Tile
TEST(TestStrategy, OneTile)
{
	auto LoadCostMap = MockData::getTileNodeLoadCostMap();
	auto RenderingTileNodeGenerator = MockData::createRenderingTileNodeGenerator();

	RenderingTileNodeGenerator.setLoadCostMap(LoadCostMap);
	RenderingTileNodeGenerator.setMaxLoadPerFrame(1500);

	std::vector<std::vector<std::shared_ptr<CTileNode>>> RenderingTileNodeSet;

	//1
	auto PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(1);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 1);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "1.bin");

	//mock after update cost table by hard code
	LoadCostMap["1.bin"].GeoInMemory = true;
	LoadCostMap["1.bin"].TexInMemory = true;
	LoadCostMap["1.bin"].LoadCost = 0;

	//2
	RenderingTileNodeSet.clear();
	PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(2);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 1);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "3.bin");

	LoadCostMap["3.bin"].GeoInMemory = true;
	LoadCostMap["3.bin"].TexInMemory = true;
	LoadCostMap["3.bin"].LoadCost = 0;

	//3
	RenderingTileNodeSet.clear();
	PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(3);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 3);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "4.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][1]->getGeometryFileName(), "5.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][2]->getGeometryFileName(), "6.bin");

	LoadCostMap["4.bin"].GeoInMemory = true;
	LoadCostMap["4.bin"].TexInMemory = true;
	LoadCostMap["4.bin"].LoadCost = 0;
	LoadCostMap["5.bin"].GeoInMemory = true;
	LoadCostMap["5.bin"].TexInMemory = true;
	LoadCostMap["5.bin"].LoadCost = 0;
	LoadCostMap["6.bin"].GeoInMemory = true;
	LoadCostMap["6.bin"].TexInMemory = true;
	LoadCostMap["6.bin"].LoadCost = 0;

	//3
	RenderingTileNodeSet.clear();
	PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(3);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 4);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "7.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][1]->getGeometryFileName(), "8.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][2]->getGeometryFileName(), "5.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][3]->getGeometryFileName(), "6.bin");
}

////一个Tile，加载不了Preferred
TEST(TestStrategy, OneFrameCannotLoadPreferred)
{
	auto LoadCostMap = MockData::getTileNodeLoadCostMap();
	auto RenderingTileNodeGenerator = MockData::createRenderingTileNodeGenerator();

	RenderingTileNodeGenerator.setLoadCostMap(LoadCostMap);

	//
	RenderingTileNodeGenerator.setMaxLoadPerFrame(300);

	std::vector<std::vector<std::shared_ptr<CTileNode>>> RenderingTileNodeSet;

	//2
	RenderingTileNodeSet.clear();
	auto PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(2);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 1);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "3.bin");

	LoadCostMap["3.bin"].GeoInMemory = true;
	LoadCostMap["3.bin"].TexInMemory = true;
	LoadCostMap["3.bin"].LoadCost = 0;

	//3
	RenderingTileNodeSet.clear();
	PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(3);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 1);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "3.bin");

	LoadCostMap["7.bin"].GeoInMemory = true;
	LoadCostMap["7.bin"].TexInMemory = true;
	LoadCostMap["7.bin"].LoadCost = 0;
	LoadCostMap["8.bin"].GeoInMemory = false;
	LoadCostMap["8.bin"].TexInMemory = true;
	LoadCostMap["8.bin"].LoadCost = 450;

	//3
	RenderingTileNodeSet.clear();
	PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(3);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 1);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "3.bin");

	LoadCostMap["7.bin"].GeoInMemory = true;
	LoadCostMap["7.bin"].TexInMemory = true;
	LoadCostMap["7.bin"].LoadCost = 0;
	LoadCostMap["8.bin"].GeoInMemory = true;
	LoadCostMap["8.bin"].TexInMemory = true;
	LoadCostMap["8.bin"].LoadCost = 0;

	//3
	RenderingTileNodeSet.clear();
	PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(3);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 4);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "7.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][1]->getGeometryFileName(), "8.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][2]->getGeometryFileName(), "5.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][3]->getGeometryFileName(), "6.bin");
}

////一个Tile，一个TileNode太大加载不了
TEST(TestStrategy, OneTileNodeTooBig)
{
	auto LoadCostMap = MockData::getTileNodeLoadCostMap();
	auto RenderingTileNodeGenerator = MockData::createRenderingTileNodeGenerator();

	RenderingTileNodeGenerator.setLoadCostMap(LoadCostMap);

	//
	RenderingTileNodeGenerator.setMaxLoadPerFrame(500);

	std::vector<std::vector<std::shared_ptr<CTileNode>>> RenderingTileNodeSet;

	//1
	auto PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(1);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 1);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "1.bin");

	//mock after update cost table by hard code
	LoadCostMap["1.bin"].GeoInMemory = true;
	LoadCostMap["1.bin"].TexInMemory = true;
	LoadCostMap["1.bin"].LoadCost = 0;

	//2
	RenderingTileNodeSet.clear();
	auto PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(2);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 1);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "1.bin");

	LoadCostMap["3.bin"].GeoInMemory = true;
	LoadCostMap["3.bin"].TexInMemory = true;
	LoadCostMap["3.bin"].LoadCost = 0;

	//2
	RenderingTileNodeSet.clear();
	auto PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(2);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 1);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "3.bin");
}

////一个Tile，复杂情况
TEST(TestStrategy, OneTile1)
{
	auto LoadCostMap = MockData::getTileNodeLoadCostMap();
	auto RenderingTileNodeGenerator = MockData::createRenderingTileNodeGenerator();

	RenderingTileNodeGenerator.setLoadCostMap(LoadCostMap);

	//
	RenderingTileNodeGenerator.setMaxLoadPerFrame(1500);

	std::vector<std::vector<std::shared_ptr<CTileNode>>> RenderingTileNodeSet;

	LoadCostMap["3.bin"].GeoInMemory = true;
	LoadCostMap["3.bin"].TexInMemory = true;
	LoadCostMap["3.bin"].LoadCost = 0;
	LoadCostMap["7.bin"].GeoInMemory = true;
	LoadCostMap["7.bin"].TexInMemory = true;
	LoadCostMap["7.bin"].LoadCost = 0;
	LoadCostMap["8.bin"].GeoInMemory = true;
	LoadCostMap["8.bin"].TexInMemory = true;
	LoadCostMap["8.bin"].LoadCost = 0;
	LoadCostMap["5.bin"].GeoInMemory = true;
	LoadCostMap["5.bin"].TexInMemory = true;
	LoadCostMap["5.bin"].LoadCost = 0;
	LoadCostMap["6.bin"].GeoInMemory = true;
	LoadCostMap["6.bin"].TexInMemory = true;
	LoadCostMap["6.bin"].LoadCost = 0;

	//4
	RenderingTileNodeSet.clear();
	auto PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(4);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 6); //7, 8, 5, 11, 12, 13
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "7.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][1]->getGeometryFileName(), "8.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][2]->getGeometryFileName(), "5.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][3]->getGeometryFileName(), "11.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][4]->getGeometryFileName(), "12.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][5]->getGeometryFileName(), "13.bin");

	LoadCostMap["11.bin"].GeoInMemory = true;
	LoadCostMap["11.bin"].TexInMemory = true;
	LoadCostMap["11.bin"].LoadCost = 0;
	LoadCostMap["12.bin"].GeoInMemory = true;
	LoadCostMap["12.bin"].TexInMemory = true;
	LoadCostMap["12.bin"].LoadCost = 0;
	LoadCostMap["13.bin"].GeoInMemory = true;
	LoadCostMap["13.bin"].TexInMemory = true;
	LoadCostMap["13.bin"].LoadCost = 0;

	//4
	RenderingTileNodeSet.clear();
	auto PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(4);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 7); //7,8,5,11,14,15,13
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "7.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][1]->getGeometryFileName(), "8.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][2]->getGeometryFileName(), "5.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][3]->getGeometryFileName(), "11.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][4]->getGeometryFileName(), "14.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][5]->getGeometryFileName(), "15.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][6]->getGeometryFileName(), "13.bin");

	LoadCostMap["14.bin"].GeoInMemory = true;
	LoadCostMap["14.bin"].TexInMemory = true;
	LoadCostMap["14.bin"].LoadCost = 0;
	LoadCostMap["15.bin"].GeoInMemory = true;
	LoadCostMap["15.bin"].TexInMemory = true;
	LoadCostMap["15.bin"].LoadCost = 0;

	//4
	RenderingTileNodeSet.clear();
	auto PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(4);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 7); //7, 8, 10, 11, 14, 15, 18
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "7.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][1]->getGeometryFileName(), "8.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][2]->getGeometryFileName(), "5.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][3]->getGeometryFileName(), "11.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][4]->getGeometryFileName(), "14.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][5]->getGeometryFileName(), "15.bin");
	EXPECT_EQ(RenderingTileNodeSet[0][6]->getGeometryFileName(), "18.bin");
}

////多个Tile
TEST(TestStrategy, MultiTile)
{
	auto LoadCostMap = MockData::getTileNodeLoadCostMap();
	auto RenderingTileNodeGenerator = MockData::createRenderingTileNodeGenerator();

	RenderingTileNodeGenerator.setLoadCostMap(LoadCostMap);
	RenderingTileNodeGenerator.setMaxLoadPerFrame(1500);

	std::vector<std::vector<std::shared_ptr<CTileNode>>> RenderingTileNodeSet;

	//5
	LoadCostMap["4.bin"].GeoInMemory = true;
	LoadCostMap["4.bin"].TexInMemory = true;
	LoadCostMap["4.bin"].LoadCost = 0;
	LoadCostMap["5.bin"].GeoInMemory = true;
	LoadCostMap["5.bin"].TexInMemory = true;
	LoadCostMap["5.bin"].LoadCost = 0;
	LoadCostMap["6.bin"].GeoInMemory = true;
	LoadCostMap["6.bin"].TexInMemory = true;
	LoadCostMap["6.bin"].LoadCost = 0;
	LoadCostMap["d.bin"].GeoInMemory = true;
	LoadCostMap["d.bin"].TexInMemory = true;
	LoadCostMap["d.bin"].LoadCost = 0;
	LoadCostMap["e.bin"].GeoInMemory = true;
	LoadCostMap["e.bin"].TexInMemory = true;
	LoadCostMap["e.bin"].LoadCost = 0;
	LoadCostMap["f.bin"].GeoInMemory = true;
	LoadCostMap["f.bin"].TexInMemory = true;
	LoadCostMap["f.bin"].LoadCost = 0;
	LoadCostMap["g.bin"].GeoInMemory = true;
	LoadCostMap["g.bin"].TexInMemory = true;
	LoadCostMap["g.bin"].LoadCost = 0;
	LoadCostMap["D.bin"].GeoInMemory = true;
	LoadCostMap["D.bin"].TexInMemory = true;
	LoadCostMap["D.bin"].LoadCost = 0;
	LoadCostMap["E.bin"].GeoInMemory = true;
	LoadCostMap["E.bin"].TexInMemory = true;
	LoadCostMap["E.bin"].LoadCost = 0;
	LoadCostMap["F.bin"].GeoInMemory = true;
	LoadCostMap["F.bin"].TexInMemory = true;
	LoadCostMap["F.bin"].LoadCost = 0;

	RenderingTileNodeSet.clear();
	auto PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(5);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	//4,5,6,h,i,j,k,l,m,e,f,g,D,E,F
	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 3);
	ASSERT_ANY_THROW(RenderingTileNodeSet[1].size() == 9);
	ASSERT_ANY_THROW(RenderingTileNodeSet[2].size() == 3);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "4.bin");
	EXPECT_EQ(RenderingTileNodeSet[1][0]->getGeometryFileName(), "h.bin");
	EXPECT_EQ(RenderingTileNodeSet[2][1]->getGeometryFileName(), "E.bin");
}

////多个Tile
TEST(TestStrategy, MultiTile1)
{
	auto LoadCostMap = MockData::getTileNodeLoadCostMap();
	auto RenderingTileNodeGenerator = MockData::createRenderingTileNodeGenerator();

	RenderingTileNodeGenerator.setLoadCostMap(LoadCostMap);
	RenderingTileNodeGenerator.setMaxLoadPerFrame(1500);

	std::vector<std::vector<std::shared_ptr<CTileNode>>> RenderingTileNodeSet;

	//6
	LoadCostMap["4.bin"].GeoInMemory = true;
	LoadCostMap["4.bin"].TexInMemory = true;
	LoadCostMap["4.bin"].LoadCost = 0;
	LoadCostMap["5.bin"].GeoInMemory = true;
	LoadCostMap["5.bin"].TexInMemory = true;
	LoadCostMap["5.bin"].LoadCost = 0;
	LoadCostMap["6.bin"].GeoInMemory = true;
	LoadCostMap["6.bin"].TexInMemory = true;
	LoadCostMap["6.bin"].LoadCost = 0;
	LoadCostMap["d.bin"].GeoInMemory = true;
	LoadCostMap["d.bin"].TexInMemory = true;
	LoadCostMap["d.bin"].LoadCost = 0;
	LoadCostMap["e.bin"].GeoInMemory = true;
	LoadCostMap["e.bin"].TexInMemory = true;
	LoadCostMap["e.bin"].LoadCost = 0;
	LoadCostMap["f.bin"].GeoInMemory = true;
	LoadCostMap["f.bin"].TexInMemory = true;
	LoadCostMap["f.bin"].LoadCost = 0;
	LoadCostMap["g.bin"].GeoInMemory = true;
	LoadCostMap["g.bin"].TexInMemory = true;
	LoadCostMap["g.bin"].LoadCost = 0;
	LoadCostMap["D.bin"].GeoInMemory = true;
	LoadCostMap["D.bin"].TexInMemory = true;
	LoadCostMap["D.bin"].LoadCost = 0;
	LoadCostMap["E.bin"].GeoInMemory = true;
	LoadCostMap["E.bin"].TexInMemory = true;
	LoadCostMap["E.bin"].LoadCost = 0;
	LoadCostMap["F.bin"].GeoInMemory = true;
	LoadCostMap["F.bin"].TexInMemory = true;
	LoadCostMap["F.bin"].LoadCost = 0;

	RenderingTileNodeSet.clear();
	auto PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(6);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	//4,5,6,h,i,j,k,l,m,e,f,g,D,E,F
	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 3);
	ASSERT_ANY_THROW(RenderingTileNodeSet[1].size() == 9);
	ASSERT_ANY_THROW(RenderingTileNodeSet[2].size() == 3);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "4.bin");
	EXPECT_EQ(RenderingTileNodeSet[1][1]->getGeometryFileName(), "i.bin");
	EXPECT_EQ(RenderingTileNodeSet[2][2]->getGeometryFileName(), "F.bin");

	LoadCostMap["h.bin"].GeoInMemory = true;
	LoadCostMap["h.bin"].TexInMemory = true;
	LoadCostMap["h.bin"].LoadCost = 0;
	LoadCostMap["i.bin"].GeoInMemory = true;
	LoadCostMap["i.bin"].TexInMemory = true;
	LoadCostMap["i.bin"].LoadCost = 0;
	LoadCostMap["j.bin"].GeoInMemory = true;
	LoadCostMap["j.bin"].TexInMemory = true;
	LoadCostMap["j.bin"].LoadCost = 0;
	LoadCostMap["k.bin"].GeoInMemory = true;
	LoadCostMap["k.bin"].TexInMemory = true;
	LoadCostMap["k.bin"].LoadCost = 0;
	LoadCostMap["l.bin"].GeoInMemory = true;
	LoadCostMap["l.bin"].TexInMemory = true;
	LoadCostMap["l.bin"].LoadCost = 0;
	LoadCostMap["m.bin"].GeoInMemory = true;
	LoadCostMap["m.bin"].TexInMemory = true;
	LoadCostMap["m.bin"].LoadCost = 0;

	//6
	RenderingTileNodeSet.clear();
	auto PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(6);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	//4,5,6,h,n,j,k,l,m,e,f,g,G,H,I,J,E,F 
	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 3);
	ASSERT_ANY_THROW(RenderingTileNodeSet[1].size() == 9);
	ASSERT_ANY_THROW(RenderingTileNodeSet[2].size() == 6);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "4.bin");
	EXPECT_EQ(RenderingTileNodeSet[1][1]->getGeometryFileName(), "n.bin");
	EXPECT_EQ(RenderingTileNodeSet[2][2]->getGeometryFileName(), "I.bin");

	LoadCostMap["G.bin"].GeoInMemory = true;
	LoadCostMap["G.bin"].TexInMemory = true;
	LoadCostMap["G.bin"].LoadCost = 0;
	LoadCostMap["H.bin"].GeoInMemory = true;
	LoadCostMap["H.bin"].TexInMemory = true;
	LoadCostMap["H.bin"].LoadCost = 0;
	LoadCostMap["I.bin"].GeoInMemory = true;
	LoadCostMap["I.bin"].TexInMemory = true;
	LoadCostMap["I.bin"].LoadCost = 0;
	LoadCostMap["J.bin"].GeoInMemory = true;
	LoadCostMap["J.bin"].TexInMemory = true;
	LoadCostMap["J.bin"].LoadCost = 0;
	LoadCostMap["n.bin"].GeoInMemory = true;
	LoadCostMap["n.bin"].TexInMemory = true;
	LoadCostMap["n.bin"].LoadCost = 0;

	//6
	RenderingTileNodeSet.clear();
	auto PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(6);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	//7,8,5,6,h,n,j,k,l,m,e,f,g,G,H,I,J,E,F 
	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 4);
	ASSERT_ANY_THROW(RenderingTileNodeSet[1].size() == 9);
	ASSERT_ANY_THROW(RenderingTileNodeSet[2].size() == 6);
	EXPECT_EQ(RenderingTileNodeSet[0][0]->getGeometryFileName(), "7.bin");
	EXPECT_EQ(RenderingTileNodeSet[1][1]->getGeometryFileName(), "n.bin");
	EXPECT_EQ(RenderingTileNodeSet[2][2]->getGeometryFileName(), "I.bin");

	LoadCostMap["7.bin"].GeoInMemory = true;
	LoadCostMap["7.bin"].TexInMemory = true;
	LoadCostMap["7.bin"].LoadCost = 0;
	LoadCostMap["8.bin"].GeoInMemory = true;
	LoadCostMap["8.bin"].TexInMemory = true;
	LoadCostMap["8.bin"].LoadCost = 0;

	//6
	RenderingTileNodeSet.clear();
	auto PreferredTileNodeSet = MockData::getPreferredTileNodeSetByID(6);
	RenderingTileNodeGenerator.generateRenderingTileNodeByStrategy(hivePagedLOD::EStragtegy::xxx, PreferredTileNodeSet, RenderingTileNodeSet);

	//7,8,10,6,h,n,j,k,l,m,e,f,g,G,H,I,J,E,F
	ASSERT_ANY_THROW(RenderingTileNodeSet[0].size() == 4);
	ASSERT_ANY_THROW(RenderingTileNodeSet[1].size() == 9);
	ASSERT_ANY_THROW(RenderingTileNodeSet[2].size() == 6);
	EXPECT_EQ(RenderingTileNodeSet[0][2]->getGeometryFileName(), "10.bin");
	EXPECT_EQ(RenderingTileNodeSet[1][1]->getGeometryFileName(), "n.bin");
	EXPECT_EQ(RenderingTileNodeSet[2][2]->getGeometryFileName(), "I.bin");
}