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

//���Ե㣺����ViewInfo������׵��ƽ��
//���룺��Ч��ViewInfo
//�������׵��ƽ��
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

//���Ե㣺��Ч����Ĵ�����Ϣ
//���룺��Ч��ViewInfo��empty��׵��ƽ����Ϣ����Ч��ViewInfo�ͷ�empty��׵��ƽ����Ϣ
//�����"Error: Negative Input"���˳�
TEST_F(PagedLODCommonUtilsTest, FrustumPlaneCalculationInvalidVertification)
{
	std::vector<glm::vec4> TempTestFrustumPlanes;
	EXPECT_DEATH(hivePagedLOD::CUtils::getInstance()->calculateFrustumPlane(TempTestFrustumPlanes, m_InvalidViewInfo), "Error: Negative Input");
	
	TempTestFrustumPlanes.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
	EXPECT_DEATH(hivePagedLOD::CUtils::getInstance()->calculateFrustumPlane(TempTestFrustumPlanes, m_ValidViewInfo), "Error: Negative Input");
}

//���Ե㣺�ж���׵��Ͱ����λ�ù�ϵ
//���룺��׵��ƽ����Ϣ����Ҫ���Ե�TileNode��Ϣ��TileNode������λ�÷ֱ�Ϊ��׵���ڡ���׵�嶥�㡢��׵��ߡ���׵���桢��׵���⣨�ཻ�����С����룩
//�����true or false��TileNode��Frustum�Ƿ��ཻ��
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

//���Ե㣺�Ӽ����ļ�������ȡ��װ�����ļ���
//���룺��Ч�ļ����ļ���
//����������ļ���
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

//���Ե㣺�Ӽ����ļ�������ȡ��װ�����ļ���
//���룺��Ч�ļ����ļ���
//�������Ч�ļ����ļ���
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

//���Ե㣺�����ļ�������������ļ���ǰ׺
//���룺��Ч�������ļ���
//����������ļ���ǰ׺
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

//���Ե㣺�����ļ�������������ļ���ǰ׺
//���룺��Ч�������ļ���
//�������Ч�������ļ���
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