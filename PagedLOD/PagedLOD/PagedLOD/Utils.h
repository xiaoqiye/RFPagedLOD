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
		static void calculateFrustumPlane(std::vector<glm::vec4>&voFrustumPlanes, const SViewInfo& vViewInfo);
		static void calculateMatrices(glm::mat4& voViewMatrix, glm::mat4& voProjectionMatrix, const SViewInfo& vViewInfo);
		static bool isBoundingSphereInFrustum(const std::vector<glm::vec4>& vFrustum, const glm::vec3& vBoundingSphereCenter, float vBoundingSphereRadius);
		static std::string getTextureFileNameFromGeometryFileName(const std::string& vMeshFileName);
		void printCameraInfo(const hivePagedLOD::SCameraInfo& vCameraInfo);
		//notice:TEMP FUNCTION
		void setCameraPos(const glm::vec3& vVec3) { CAMERA_POSITION_FOR_IMPORTANCE_COMPUTE = vVec3; }
		const glm::vec3& getCameraPos() const { return CAMERA_POSITION_FOR_IMPORTANCE_COMPUTE; }
		static std::string getGeoNamePrefixByTexName(const std::string& vTextureFileName);
	private:
		CUtils() = default;
		//temp
		glm::vec3 CAMERA_POSITION_FOR_IMPORTANCE_COMPUTE = glm::vec3(0.0f);
	friend class hiveOO::CSingleton<CUtils>;
	};
}
