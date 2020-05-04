#include "pch.h"
#include "MockData.h"

class PagedLODCommonUtilsTest :public testing::Test
{
protected:
	virtual void SetUp()
	{
		m_ValidViewInfo = MockData::getValidViewInfo();
		m_FrustumParamDiffThreshold = 0.01f;

		m_FrustumPlanes = MockData::getFrustumPlanes();
		m_TileNodeSet = MockData::getTileNodeSet();

		m_GeoFileNameSet = MockData::getGeoFileNameSet();
		m_TexFileNameSet = MockData::getTexFileNameSet();
	}

	hivePagedLOD::SViewInfo m_InvalidViewInfo;
	hivePagedLOD::SViewInfo m_ValidViewInfo;

	std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> m_TileNodeSet;
	std::vector<glm::vec4> m_FrustumPlanes;

	std::vector<std::string> m_GeoFileNameSet;
	std::vector<std::string> m_TexFileNameSet;

	float m_FrustumParamDiffThreshold;
};

//测试点：输入有效的ViewInfo，判断计算的视椎体平面是否正确
TEST_F(PagedLODCommonUtilsTest, FrustumPlaneCalculationVertification)
{
	std::vector<glm::vec4> ExpectFrustumPlanes;
	std::vector<glm::vec4> ActualFrustumPlanes;

	hivePagedLOD::CUtils::getInstance()->calculateFrustumPlane(ActualFrustumPlanes, m_ValidViewInfo);
	hivePagedLOD::CUtils::getInstance()->calculateFrustumPlaneNew(ExpectFrustumPlanes, m_ValidViewInfo);

	unsigned int Size = ExpectFrustumPlanes.size();
	for (auto i = 0; i < Size; ++i)
	{
		for (auto k = 0; k < ExpectFrustumPlanes[i].length(); ++k)
			EXPECT_NEAR(ActualFrustumPlanes[i][k], ExpectFrustumPlanes[i][k], m_FrustumParamDiffThreshold);
	}
}

//测试点：输入视椎体平面信息和需要测试的TileNode信息，判断输出TileNode与Frustum关系是否正确
//TileNode：中心位置分别为视椎体内、视椎体顶点、视椎体边、视椎体面、视椎体外（相交、相切、相离）
TEST_F(PagedLODCommonUtilsTest, IsBoundingSphereInFrustumCalculationVertification)
{
	std::vector<bool> ExpectResult = { true,true,true,true,true,true,false };

	unsigned int Size = m_TileNodeSet.size();
	for (auto i = 0; i < Size; ++i)
	{
		glm::vec3 TempCenter = m_TileNodeSet[i]->getBoundingSphere().getCenter();
		float TempRadius = m_TileNodeSet[i]->getBoundingSphere().getRadius();
		EXPECT_EQ(ExpectResult[i], hivePagedLOD::CUtils::getInstance()->isBoundingSphereInFrustum(m_FrustumPlanes, TempCenter, TempRadius));
	}
}

//测试点：输入有效或者无效的几何文件名，若有效，则按照规则进行处理并返回；若无效，将输入直接返回
//有效输入：分隔符+正确后缀  无效输入：无分隔符||无后缀||后缀错误
TEST_F(PagedLODCommonUtilsTest, getTextureFileNameFromGeometryFileNameVertification)
{
	const std::vector<std::string> ExpectResult = { "T1_111.bint","T1_.bint","T1_111.bint","T1222.bin","T1222","T1222.bint" };

	unsigned int Size = m_GeoFileNameSet.size();
	for (auto i = 0; i < Size; ++i)
	{
		const std::string ActualResult = hivePagedLOD::CUtils::getInstance()->getTextureFileNameFromGeometryFileName(m_GeoFileNameSet[i]);
		EXPECT_STREQ(ExpectResult[i].c_str(), ActualResult.c_str());
	}
}

//测试点：输入有效或者无效的纹理文件名，若有效，按照规则进行处理后返回；若无效，则将输入直接返回
//有效输入：分隔符+正确后缀  无效输入：无分隔符||无后缀||后缀错误
TEST_F(PagedLODCommonUtilsTest, getGeoNamePrefixByTexNameVertification) 
{
	const std::vector<std::string> ExpectResult = { "T1_","T1._","T1.1_","T1222bint" ,"T1222","T1222.bin" };

	unsigned int Size = m_TexFileNameSet.size();
	for (auto i = 0; i < Size; ++i)
	{
		const std::string ActualResult = hivePagedLOD::CUtils::getInstance()->getGeoNamePrefixByTexName(m_TexFileNameSet[i]);
		EXPECT_STREQ(ExpectResult[i].c_str(), ActualResult.c_str());
	}
}