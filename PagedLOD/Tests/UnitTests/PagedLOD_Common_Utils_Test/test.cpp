#include "pch.h"
#include "MockData.h"

class PagedLODCommonUtilsTest :public testing::Test
{
protected:
	virtual void SetUp()
	{
		m_ValidViewInfo = MockData::getValidViewInfo();
		m_FrustumParamDiffThreshold = 0.01f;
		m_InvalidViewInfo = MockData::getInvalidViewInfo();

		m_FrustumPlanes = MockData::getFrustumPlanes();
		m_TileNodeSet = MockData::getTileNodeSet();

		m_ValidGeoFileNameSet = MockData::getValidGeoFileNameSet();
		m_InvalidGeoFileNameSet = MockData::getInvalidGeoFileNameSet();

		m_ValidTexFileNameSet = MockData::getValidTexFileNameSet();
		m_InvalidTexFileNameSet = MockData::getInvalidTexFileNameSet();
	}

	hivePagedLOD::SViewInfo m_InvalidViewInfo;
	hivePagedLOD::SViewInfo m_ValidViewInfo;

	std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> m_TileNodeSet;
	std::vector<glm::vec4> m_FrustumPlanes;

	std::vector<std::string> m_ValidGeoFileNameSet;
	std::vector<std::string> m_InvalidGeoFileNameSet;

	std::vector<std::string> m_ValidTexFileNameSet;
	std::vector<std::string> m_InvalidTexFileNameSet;

	float m_FrustumParamDiffThreshold;
};

//测试点：根据ViewInfo计算视椎体平面
//输入：有效的ViewInfo
//输出：视椎体平面
TEST_F(PagedLODCommonUtilsTest, FrustumPlaneCalculationValidVertification)
{
	std::vector<glm::vec4> ExpectFrustumPlanes;
	std::vector<glm::vec4> ActualFrustumPlanes;

	hivePagedLOD::CUtils::getInstance()->calculateFrustumPlane(ActualFrustumPlanes, m_ValidViewInfo);
	hivePagedLOD::CUtils::getInstance()->calculateFrustumPlaneStd(ExpectFrustumPlanes, m_ValidViewInfo);

	EXPECT_EQ(ExpectFrustumPlanes.size(), ActualFrustumPlanes.size());
	unsigned int Size = ExpectFrustumPlanes.size();
	for (auto i = 0; i < Size; ++i)
	{
		for (auto k = 0; k < ExpectFrustumPlanes[i].length(); ++k)
			EXPECT_NEAR(ActualFrustumPlanes[i][k], ExpectFrustumPlanes[i][k], m_FrustumParamDiffThreshold);
	}
}

//测试点：无效输入的错误消息
//输入：无效的ViewInfo和empty视椎体平面信息；有效的ViewInfo和非empty视椎体平面信息
//输出："Error: Negative Input"并退出
TEST_F(PagedLODCommonUtilsTest, FrustumPlaneCalculationInvalidVertification)
{
	std::vector<glm::vec4> TempTestFrustumPlanes;
	EXPECT_DEATH(hivePagedLOD::CUtils::getInstance()->calculateFrustumPlane(TempTestFrustumPlanes, m_InvalidViewInfo), "Error: Negative Input");
	
	TempTestFrustumPlanes.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
	EXPECT_DEATH(hivePagedLOD::CUtils::getInstance()->calculateFrustumPlane(TempTestFrustumPlanes, m_ValidViewInfo), "Error: Negative Input");
}

//测试点：判断视椎体和绑定球的位置关系
//输入：视椎体平面信息和需要测试的TileNode信息，TileNode：中心位置分别为视椎体内、视椎体顶点、视椎体边、视椎体面、视椎体外（相交、相切、相离）
//输出：true or false（TileNode与Frustum是否相交）
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

//测试点：从几何文件名中提取组装纹理文件名
//输入：有效的几何文件名
//输出：纹理文件名
TEST_F(PagedLODCommonUtilsTest, getTextureFileNameFromGeometryFileNameWithValidInput)
{
	const std::vector<std::string> ExpectResult = { "T1_111.bint","T1_.bint","T1_111.bint" };

	unsigned int Size = m_ValidGeoFileNameSet.size();
	for (auto i = 0; i < Size; ++i)
	{
		const std::string ActualResult = hivePagedLOD::CUtils::getInstance()->getTextureFileNameFromGeometryFileName(m_ValidGeoFileNameSet[i]);
		EXPECT_STREQ(ExpectResult[i].c_str(), ActualResult.c_str());
	}
}

//测试点：从几何文件名中提取组装纹理文件名
//输入：无效的几何文件名
//输出：无效的几何文件名
TEST_F(PagedLODCommonUtilsTest, getTextureFileNameFromGeometryFileNameWithInvalidInput)
{
	const std::vector<std::string> ExpectResult = { "T1222.bin","T1222","T1222.bint" };

	unsigned int Size = m_InvalidGeoFileNameSet.size();
	for (auto i = 0; i < Size; ++i)
	{
		const std::string ActualResult = hivePagedLOD::CUtils::getInstance()->getTextureFileNameFromGeometryFileName(m_InvalidGeoFileNameSet[i]);
		EXPECT_STREQ(ExpectResult[i].c_str(), ActualResult.c_str());
	}
}

//测试点：纹理文件名中提出几何文件名前缀
//输入：有效的纹理文件名
//输出：几何文件名前缀
TEST_F(PagedLODCommonUtilsTest, getGeoNamePrefixByTexNameWithValidInput) 
{
	const std::vector<std::string> ExpectResult = { "T1_","T1._","T1.1_" };

	unsigned int Size = m_ValidTexFileNameSet.size();
	for (auto i = 0; i < Size; ++i)
	{
		const std::string ActualResult = hivePagedLOD::CUtils::getInstance()->getGeoNamePrefixByTexName(m_ValidTexFileNameSet[i]);
		EXPECT_STREQ(ExpectResult[i].c_str(), ActualResult.c_str());
	}
}

//测试点：纹理文件名中提出几何文件名前缀
//输入：无效的纹理文件名
//输出：无效的纹理文件名
TEST_F(PagedLODCommonUtilsTest, getGeoNamePrefixByTexNameWithInvalidInput)
{
	const std::vector<std::string> ExpectResult = { "T1222bint" ,"T1222","T1222.bin" };

	unsigned int Size = m_InvalidTexFileNameSet.size();
	for (auto i = 0; i < Size; ++i)
	{
		const std::string ActualResult = hivePagedLOD::CUtils::getInstance()->getGeoNamePrefixByTexName(m_InvalidTexFileNameSet[i]);
		EXPECT_STREQ(ExpectResult[i].c_str(), ActualResult.c_str());
	}
}

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}