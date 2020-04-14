#include "pch.h"
#include "MockData.h"
TEST(TestUpdateTileNodeLoadCostTable, updateTileNodeLoadCostTable) 
{
	auto LoadCostMap = MockData::getTileNodeLoadCostMap();
	auto UpdateDataSet = MockData::getUpdateDataSet();
	auto ResultSet = MockData::getExceptResultSet();
	auto RenderingTileNodeGenerator = MockData::createRenderingTileNodeGenerator();

	RenderingTileNodeGenerator.setLoadCostMap(LoadCostMap);
	RenderingTileNodeGenerator.updateTileNodeLoadCostTable(UpdateDataSet);
	auto NewMap = RenderingTileNodeGenerator.getLoadCostMap();

	for (std::map<std::string, hivePagedLOD::STileNodeLoadCost>::iterator it = NewMap.begin(), int i = 0; it != NewMap.end(); ++it)
	{
		EXPECT_EQ(std::get<0>(ResultSet[i]), it->second.GeoInMemory);
		EXPECT_EQ(std::get<1>(ResultSet[i]), it->second.TexInMemory);
		EXPECT_EQ(std::get<2>(ResultSet[i]), it->second.LoadCost);
	}
}