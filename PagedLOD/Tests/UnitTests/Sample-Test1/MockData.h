#pragma once
#include "TileNode.h"
#include "Common.h"
#include <GLM/glm.hpp>

#include <vector>

namespace MockData
{
	const std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> getTileNodeSet()
	{
		std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> TileNodeSet;
		auto TileNode0 = std::make_shared<hivePagedLOD::CTileNode>();
		TileNode0->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(0.0f), 10.0f));
		auto TileNode1 = std::make_shared<hivePagedLOD::CTileNode>();
		TileNode1->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(0.0f, 0.0f, 100.0f), 10.0f));
		auto TileNode2 = std::make_shared<hivePagedLOD::CTileNode>();
		TileNode2->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(0.0f, 0.0f, 50.0f), 2.0f));
		auto TileNode3 = std::make_shared<hivePagedLOD::CTileNode>();
		TileNode3->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(0.0f, 0.0f, 0.0f), 200.0f));
		auto TileNode4 = std::make_shared<hivePagedLOD::CTileNode>();
		TileNode4->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(30.0f, 0.0f, 30.0f), 10.0f));
		TileNodeSet.emplace_back(TileNode0);
		TileNodeSet.emplace_back(TileNode1);
		TileNodeSet.emplace_back(TileNode2);
		TileNodeSet.emplace_back(TileNode3);
		TileNodeSet.emplace_back(TileNode4);

		return TileNodeSet;
	}

	const std::vector<hivePagedLOD::SViewInfo> getViewInfoSet()
	{
		std::vector<hivePagedLOD::SViewInfo> ViewInfoSet;

		hivePagedLOD::SViewInfo ViewInfo0;
		ViewInfo0.ViewPortInfo.Width = 1080;
		ViewInfo0.ViewPortInfo.Height = 720;
		ViewInfo0.CameraInfo.Position = glm::vec3(0.0f, 0.0f, 50.0f);
		ViewInfo0.CameraInfo.Front = glm::vec3(0.0f, 0.0f, 1.0f);
		ViewInfo0.CameraInfo.Up = glm::vec3(0.0f, 1.0f, 0.0f);
		ViewInfo0.CameraInfo.FOV = 45.0f;
		ViewInfo0.CameraInfo.NearPlane = 10.0f;
		ViewInfo0.CameraInfo.FarPlane = 10000.0f;

		hivePagedLOD::SViewInfo ViewInfo1;
		ViewInfo1.ViewPortInfo.Width = 1080;
		ViewInfo1.ViewPortInfo.Height = 720;
		ViewInfo1.CameraInfo.Position = glm::vec3(0.0f, 0.0f, 50.0f);
		ViewInfo1.CameraInfo.Front = glm::vec3(0.0f, 0.0f, -1.0f);
		ViewInfo1.CameraInfo.Up = glm::vec3(0.0f, 1.0f, 0.0f);
		ViewInfo1.CameraInfo.FOV = 45.0f;
		ViewInfo1.CameraInfo.NearPlane = 10.0f;
		ViewInfo1.CameraInfo.FarPlane = 10000.0f;

		hivePagedLOD::SViewInfo ViewInfo2;
		ViewInfo2.ViewPortInfo.Width = 1080;
		ViewInfo2.ViewPortInfo.Height = 720;
		ViewInfo2.CameraInfo.Position = glm::vec3(0.0f, 0.0f, 15.0f);
		ViewInfo2.CameraInfo.Front = glm::vec3(0.0f, 0.0f, 1.0f);
		ViewInfo2.CameraInfo.Up = glm::vec3(0.0f, 1.0f, 0.0f);
		ViewInfo2.CameraInfo.FOV = 45.0f;
		ViewInfo2.CameraInfo.NearPlane = 10.0f;
		ViewInfo2.CameraInfo.FarPlane = 10000.0f;

		ViewInfoSet.emplace_back(ViewInfo0);
		ViewInfoSet.emplace_back(ViewInfo1);
		ViewInfoSet.emplace_back(ViewInfo2);

		return ViewInfoSet;
	}
}