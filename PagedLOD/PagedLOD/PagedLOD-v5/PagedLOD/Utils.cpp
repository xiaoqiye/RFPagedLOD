#include "Utils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <iostream>

using namespace hivePagedLOD;

//****************************************************************************
//FUNCTION:
void CUtils::calculateFrustumPlane(std::vector<glm::vec4>& voFrustumPlanes, const SViewInfo& vViewInfo)
{
	_ASSERT(voFrustumPlanes.empty() && vViewInfo.isValid());
	if (!voFrustumPlanes.empty() || !vViewInfo.isValid())
	{
		std::cerr << "Error: Negative Input\n";
		exit(-1);
	}

	std::vector<glm::vec3> Position;
	auto TanHeight = glm::tan(glm::radians(vViewInfo.CameraInfo.FOV / 2.0f));
	auto TanWidth = static_cast<float>(vViewInfo.ViewPortInfo.Width) / vViewInfo.ViewPortInfo.Height * TanHeight;
	auto HalfHeight = vViewInfo.CameraInfo.NearPlane * TanHeight;
	auto HalfWidth  = vViewInfo.CameraInfo.NearPlane * TanWidth;
	Position.emplace_back( HalfWidth,  HalfHeight, -vViewInfo.CameraInfo.NearPlane);
	Position.emplace_back(-HalfWidth,  HalfHeight, -vViewInfo.CameraInfo.NearPlane);
	Position.emplace_back( HalfWidth, -HalfHeight, -vViewInfo.CameraInfo.NearPlane);
	Position.emplace_back(-HalfWidth, -HalfHeight, -vViewInfo.CameraInfo.NearPlane);

	HalfHeight = vViewInfo.CameraInfo.FarPlane * TanHeight;
	HalfWidth  = vViewInfo.CameraInfo.FarPlane * TanWidth;
	Position.emplace_back( HalfWidth,  HalfHeight, -vViewInfo.CameraInfo.FarPlane);
	Position.emplace_back(-HalfWidth,  HalfHeight, -vViewInfo.CameraInfo.FarPlane);
	Position.emplace_back( HalfWidth, -HalfHeight, -vViewInfo.CameraInfo.FarPlane);
	Position.emplace_back(-HalfWidth, -HalfHeight, -vViewInfo.CameraInfo.FarPlane);

	//near
	voFrustumPlanes.emplace_back(0.0f, 0.0f, -1.0f, -vViewInfo.CameraInfo.NearPlane);
	//far
	voFrustumPlanes.emplace_back(0.0f, 0.0f, 1.0f, vViewInfo.CameraInfo.FarPlane);
	//FIXME:视椎体平面计算感觉有问题，且此函数没有测试用例
	//top
	auto TopNormal = glm::normalize(glm::cross(Position[1] - Position[0], Position[4] - Position[0]));
	voFrustumPlanes.emplace_back(TopNormal, 0.0f);
	//bottom
	auto BottomNormal = glm::normalize(glm::cross(Position[6] - Position[2], Position[3] - Position[2]));
	voFrustumPlanes.emplace_back(BottomNormal, 0.0f);
	//left
	auto LeftNormal = glm::normalize(glm::cross(Position[3] - Position[1], Position[5] - Position[1]));
	voFrustumPlanes.emplace_back(LeftNormal, 0.0f);
	//right
	auto RightNormal = glm::normalize(glm::cross(Position[4] - Position[0], Position[2] - Position[0]));
	voFrustumPlanes.emplace_back(RightNormal, 0.0f);
}

//****************************************************************************
//FUNCTION:
void CUtils::calculateMatrices(glm::mat4& voViewMatrix, glm::mat4& voProjectionMatrix, const SViewInfo& vViewInfo)
{
	_ASSERT(vViewInfo.ViewPortInfo.Width != 0 && vViewInfo.ViewPortInfo.Height != 0);

	voViewMatrix = glm::lookAt(vViewInfo.CameraInfo.Position, vViewInfo.CameraInfo.Front + vViewInfo.CameraInfo.Position, vViewInfo.CameraInfo.Up);
	voProjectionMatrix = glm::perspective(glm::radians(vViewInfo.CameraInfo.FOV), static_cast<float>(vViewInfo.ViewPortInfo.Width) / vViewInfo.ViewPortInfo.Height, vViewInfo.CameraInfo.NearPlane, vViewInfo.CameraInfo.FarPlane);
}

//****************************************************************************
//FUNCTION:
bool CUtils::isBoundingSphereInFrustum(const std::vector<glm::vec4>& vFrustum, const glm::vec3& vBoundingSphereCenter, float vBoundingSphereRadius)
{
	_ASSERT(vFrustum.size() == 6);

	for (auto& FrustumPlane : vFrustum)
	{
		if (FrustumPlane.x * vBoundingSphereCenter.x + FrustumPlane.y * vBoundingSphereCenter.y + FrustumPlane.z * vBoundingSphereCenter.z + FrustumPlane.w < -vBoundingSphereRadius)
			return false;
	}
	return true;
}

//****************************************************************************
//FUNCTION:
std::string CUtils::getTextureFileNameFromGeometryFileName(const std::string& vMeshFileName)
{
	_ASSERTE(boost::iends_with(vMeshFileName, BINARY_GEOMETRY_SUFFIX));
	_ASSERTE(vMeshFileName.find('_') < vMeshFileName.length());

	if (!boost::iends_with(vMeshFileName, BINARY_GEOMETRY_SUFFIX) || vMeshFileName.find('_') >= vMeshFileName.length())
	{
		std::invalid_argument("The Input param is invalid,please check!");
		return vMeshFileName;
	}
	return vMeshFileName.substr(0, vMeshFileName.find_last_of('_')) + BINARY_TEXTURE_SUFFIX;
}

//****************************************************************************
//FUNCTION:
std::string CUtils::getGeoNamePrefixByTexName(const std::string& vTextureFileName)
{
	_ASSERTE(boost::ends_with(vTextureFileName, BINARY_TEXTURE_SUFFIX));
	_ASSERTE(vTextureFileName.find('.') < vTextureFileName.length());

	if (!boost::ends_with(vTextureFileName, BINARY_TEXTURE_SUFFIX) || vTextureFileName.find('.') >= vTextureFileName.length())
	{
		std::invalid_argument("The Input param is invalid,please check!");
		return vTextureFileName;
	}

	return std::string(vTextureFileName.begin(), vTextureFileName.begin() + vTextureFileName.find_last_of('.')) + '_';
}

//****************************************************************************
//FUNCTION:
void CUtils::printCameraInfo(const hivePagedLOD::SCameraInfo& vCameraInfo)
{
	std::cout << "position:  x:" << vCameraInfo.Position.x << "  y:" << vCameraInfo.Position.y << "  z:" << vCameraInfo.Position.z << std::endl;
	std::cout << "front:  x:" << vCameraInfo.Front.x << "  y:" << vCameraInfo.Front.y << "  z:" << vCameraInfo.Front.z << std::endl;
	std::cout << "up:  x:" << vCameraInfo.Up.x << "  y:" << vCameraInfo.Up.y << "  z:" << vCameraInfo.Up.z << std::endl;
}

//****************************************************************************
//FUNCTION:
void CUtils::calculateFrustumPlaneStd(std::vector<glm::vec4>& voFrustumPlanes, const SViewInfo& vViewInfo)
{
	_ASSERT(vViewInfo.isValid());

	std::vector<glm::vec3> Position;
	auto TanHeight = glm::tan(glm::radians(vViewInfo.CameraInfo.FOV / 2.0f));
	auto TanWidth = static_cast<float>(vViewInfo.ViewPortInfo.Width) / vViewInfo.ViewPortInfo.Height * TanHeight;
	auto HalfHeight = vViewInfo.CameraInfo.NearPlane * TanHeight;
	auto HalfWidth = vViewInfo.CameraInfo.NearPlane * TanWidth;
	Position.emplace_back(HalfWidth, HalfHeight, -vViewInfo.CameraInfo.NearPlane);
	Position.emplace_back(-HalfWidth, HalfHeight, -vViewInfo.CameraInfo.NearPlane);
	Position.emplace_back(HalfWidth, -HalfHeight, -vViewInfo.CameraInfo.NearPlane);
	Position.emplace_back(-HalfWidth, -HalfHeight, -vViewInfo.CameraInfo.NearPlane);

	HalfHeight = vViewInfo.CameraInfo.FarPlane * TanHeight;
	HalfWidth = vViewInfo.CameraInfo.FarPlane * TanWidth;
	Position.emplace_back(HalfWidth, HalfHeight, -vViewInfo.CameraInfo.FarPlane);
	Position.emplace_back(-HalfWidth, HalfHeight, -vViewInfo.CameraInfo.FarPlane);
	Position.emplace_back(HalfWidth, -HalfHeight, -vViewInfo.CameraInfo.FarPlane);
	Position.emplace_back(-HalfWidth, -HalfHeight, -vViewInfo.CameraInfo.FarPlane);

	_ASSERTE(voFrustumPlanes.empty());
	//near
	voFrustumPlanes.emplace_back(0.0f, 0.0f, -1.0f, -vViewInfo.CameraInfo.NearPlane);
	//far
	voFrustumPlanes.emplace_back(0.0f, 0.0f, 1.0f, vViewInfo.CameraInfo.FarPlane);
	//top
	auto TopNormal = glm::normalize(glm::cross(Position[1] - Position[0], Position[4] - Position[0]));
	float ParamD = -(TopNormal.x*Position[0].x + TopNormal.y*Position[0].y + TopNormal.z*Position[0].z);
	voFrustumPlanes.emplace_back(TopNormal, ParamD);
	//bottom
	auto BottomNormal = glm::normalize(glm::cross(Position[6] - Position[2], Position[3] - Position[2]));
	ParamD = -(BottomNormal.x*Position[2].x + BottomNormal.y*Position[2].y + BottomNormal.z*Position[2].z);
	voFrustumPlanes.emplace_back(BottomNormal, ParamD);
	//left
	auto LeftNormal = glm::normalize(glm::cross(Position[3] - Position[1], Position[5] - Position[1]));
	ParamD = -(LeftNormal.x*Position[1].x + LeftNormal.y*Position[1].y + LeftNormal.z*Position[1].z);
	voFrustumPlanes.emplace_back(LeftNormal, ParamD);
	//right
	auto RightNormal = glm::normalize(glm::cross(Position[4] - Position[0], Position[2] - Position[0]));
	ParamD = -(RightNormal.x*Position[0].x + RightNormal.y*Position[0].y + RightNormal.z*Position[0].z);
	voFrustumPlanes.emplace_back(RightNormal, ParamD);
}
