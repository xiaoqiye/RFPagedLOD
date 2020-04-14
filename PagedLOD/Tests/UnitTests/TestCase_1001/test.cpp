#include "pch.h"

#include "MockData.h"
#include "PagedLOD/Utils.h"
#include <GLM/glm.hpp>

using namespace hivePagedLOD;

TEST(CulllingTest, TestIsBoundingSphereInFrustum)
{
	auto TileNodeSet = MockData::getTileNodeSet();
	auto ViewInfoSet = MockData::getViewInfoSet();
	std::vector<glm::vec4> FrustumPlanes;
	std::vector<std::vector<bool>> Expected
	{ 
	{false, true, false, true, false}, 
	{true, false, false, true, false}, 
	{false, true, true, true, false} 
	};

	for (unsigned int i = 0; i < ViewInfoSet.size(); i++)
	{
		glm::mat4 ViewMat, ProjectionMat;

		CUtils::getInstance()->calculateMatrices(ViewMat, ProjectionMat, ViewInfoSet[i]);
		FrustumPlanes.clear();
		CUtils::getInstance()->calculateFrustumPlane(FrustumPlanes, ViewInfoSet[i]);

		for (unsigned int j = 0; j < TileNodeSet.size(); ++j)
		{
			SCOPED_TRACE("row:" + std::to_string(i) + "\tcol:" + std::to_string(j));
			glm::vec4 Center = ViewMat * glm::vec4(TileNodeSet[j]->getBoundingSphere().getCenter(), 1.0f);
			float Radius = TileNodeSet[j]->getBoundingSphere().getRadius();
			EXPECT_EQ(Expected[i][j], CUtils::getInstance()->isBoundingSphereInFrustum(FrustumPlanes, Center, Radius));
		}
	}
}