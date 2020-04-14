#pragma once
#include "pch.h"
#include "PagedLOD/Common.h"
#include "PagedLOD/TileNode.h"
#include "PagedLOD/PreferredTileNodeGenerator.h"

#include <vector>

namespace MockData
{
	std::vector<hivePagedLOD::SViewInfo> makeViewInfoSet()
	{
		//for tile_1 and use lod level 10
		std::vector<hivePagedLOD::SViewInfo> Res;
		hivePagedLOD::SViewInfo ViewInfo0;
		ViewInfo0.ViewPortInfo.Width = 1080;
		ViewInfo0.ViewPortInfo.Height = 720;

		ViewInfo0.CameraInfo.Position = hivePagedLOD::MODEL_MATRIX * glm::vec4(174.0f, 50.0f, 464.0f, 1.0f);

		ViewInfo0.CameraInfo.Front = glm::vec3(0.0f, 0.0f, -1.0f);
		ViewInfo0.CameraInfo.Up = glm::vec3(0.0f, 1.0f, 0.0f);
		ViewInfo0.CameraInfo.FOV = 45.0f;
		ViewInfo0.CameraInfo.NearPlane = 0.1f;
		ViewInfo0.CameraInfo.FarPlane = 10000.0f;
		Res.emplace_back(ViewInfo0);

		hivePagedLOD::SViewInfo ViewInfo1;
		ViewInfo1.ViewPortInfo.Width = 1080;
		ViewInfo1.ViewPortInfo.Height = 720;

		ViewInfo1.CameraInfo.Position = hivePagedLOD::MODEL_MATRIX * glm::vec4(174.0f, 150.0f, 464.0f, 1.0f);

		ViewInfo1.CameraInfo.Front = glm::vec3(0.0f, 0.0f, -1.0f);
		ViewInfo1.CameraInfo.Up = glm::vec3(0.0f, 1.0f, 0.0f);
		ViewInfo1.CameraInfo.FOV = 45.0f;
		ViewInfo1.CameraInfo.NearPlane = 0.1f;
		ViewInfo1.CameraInfo.FarPlane = 10000.0f;
		Res.emplace_back(ViewInfo1);

		return Res;
	}

	static bool isSameTileSet(const std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& vLTileSet, const std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& vRTileSet)
	{
		if (vLTileSet.size() != vRTileSet.size()) return false;
		for (unsigned int i = 0; i < vLTileSet.size(); ++i)
		{
			if (
				vLTileSet[i]->getCreatedByFileName() != vRTileSet[i]->getCreatedByFileName() ||
				vLTileSet[i]->getGeometryFileName() != vRTileSet[i]->getGeometryFileName() ||
				vLTileSet[i]->getTextureFileName() != vRTileSet[i]->getTextureFileName() ||
				vLTileSet[i]->getGeometryFileSize() != vRTileSet[i]->getGeometryFileSize() ||
				vLTileSet[i]->getTextureFileSize() != vRTileSet[i]->getTextureFileSize() ||
				std::fabs(vLTileSet[i]->getBoundingSphere().getCenter().x - vRTileSet[i]->getBoundingSphere().getCenter().x) > std::numeric_limits<float>::epsilon() ||
				std::fabs(vLTileSet[i]->getBoundingSphere().getCenter().y - vRTileSet[i]->getBoundingSphere().getCenter().y) > std::numeric_limits<float>::epsilon() ||
				std::fabs(vLTileSet[i]->getBoundingSphere().getCenter().z - vRTileSet[i]->getBoundingSphere().getCenter().z) > std::numeric_limits<float>::epsilon() ||
				std::fabs(vLTileSet[i]->getBoundingSphere().getRadius() - vRTileSet[i]->getBoundingSphere().getRadius()) > std::numeric_limits<float>::epsilon() ||
				std::fabs(vLTileSet[i]->getRange() - vRTileSet[i]->getRange()) > std::numeric_limits<float>::epsilon()
				)
				return false;
		}
		return true;
	}

	static std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> makeExpectTile(const std::shared_ptr<hivePagedLOD::CTileNode>& vRoot)
	{
		std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> res;
		auto L18 = vRoot->getChildAt(0).getChildAt(0).getChildAt(0);
		res.emplace_back(L18.getChildPointerAt(0));
		res.emplace_back(L18.getChildPointerAt(1));
		res.emplace_back(L18.getChildPointerAt(2));

		return res;
	}
}