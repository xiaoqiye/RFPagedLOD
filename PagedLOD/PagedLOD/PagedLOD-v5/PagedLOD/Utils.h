#pragma once
#include "Common.h"
#include "TileNode.h"
#include <common/Singleton.h>
#include <GLM/glm.hpp>
#include <vector>
#include <string>

namespace hivePagedLOD
{
	class CUtils : public hiveOO::CSingleton<CUtils>
	{
	public:
		~CUtils() = default;
		void calculateFrustumPlane(std::vector<glm::vec4>& voFrustumPlanes, const SViewInfo& vViewInfo);
		void calculateMatrices(glm::mat4& voViewMatrix, glm::mat4& voProjectionMatrix, const SViewInfo& vViewInfo);
		bool isBoundingSphereInFrustum(const std::vector<glm::vec4>& vFrustum, const glm::vec3& vBoundingSphereCenter, float vBoundingSphereRadius);
		std::string getTextureFileNameFromGeometryFileName(const std::string& vMeshFileName);
		std::string getGeoNamePrefixByTexName(const std::string& vTextureFileName);
		void printCameraInfo(const hivePagedLOD::SCameraInfo& vCameraInfo);
		//for _UNIT_TEST
		void calculateFrustumPlaneStd(std::vector<glm::vec4>& voFrustumPlanes, const SViewInfo& vViewInfo);

	private:
		CUtils() = default;
		friend class hiveOO::CSingleton<CUtils>;
	};
}
