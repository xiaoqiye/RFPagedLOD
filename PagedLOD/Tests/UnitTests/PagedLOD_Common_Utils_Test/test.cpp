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

//���Ե㣺������Ч��ViewInfo���жϼ������׵��ƽ���Ƿ���ȷ
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

//���Ե㣺������׵��ƽ����Ϣ����Ҫ���Ե�TileNode��Ϣ���ж����TileNode��Frustum��ϵ�Ƿ���ȷ
//TileNode������λ�÷ֱ�Ϊ��׵���ڡ���׵�嶥�㡢��׵��ߡ���׵���桢��׵���⣨�ཻ�����С����룩
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

//���Ե㣺������Ч������Ч�ļ����ļ���������Ч�����չ�����д������أ�����Ч��������ֱ�ӷ���
//��Ч���룺�ָ���+��ȷ��׺  ��Ч���룺�޷ָ���||�޺�׺||��׺����
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

//���Ե㣺������Ч������Ч�������ļ���������Ч�����չ�����д���󷵻أ�����Ч��������ֱ�ӷ���
//��Ч���룺�ָ���+��ȷ��׺  ��Ч���룺�޷ָ���||�޺�׺||��׺����
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