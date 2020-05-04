#pragma once
#include "Utils.h"

namespace MockData
{
	hivePagedLOD::SViewInfo getValidViewInfo()
	{
		hivePagedLOD::SViewInfo TempValidViewInfo;

		TempValidViewInfo.ViewPortInfo.Width = 1080;
		TempValidViewInfo.ViewPortInfo.Height = 720;
		TempValidViewInfo.CameraInfo.Position = glm::vec3(0.0f, 0.0f, 3.0f);
		TempValidViewInfo.CameraInfo.Front = glm::vec3(0.0f, 0.0f, 1.0f);
		TempValidViewInfo.CameraInfo.Up = glm::vec3(0.0f, 1.0f, 0.0f);
		TempValidViewInfo.CameraInfo.FOV = 45.0f;
		TempValidViewInfo.CameraInfo.NearPlane = 1.0f;
		TempValidViewInfo.CameraInfo.FarPlane = 1000.0f;

		return TempValidViewInfo;
	}

	const std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> getTileNodeSet()
	{
		std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> TempTileNodeSet;

		auto TileNode0 = std::make_shared<hivePagedLOD::CTileNode>();
		TileNode0->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(310.19, 206.793, -500.5), 10.0f));
		auto TileNode1 = std::make_shared<hivePagedLOD::CTileNode>();
		TileNode1->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(0.62, 0.414f, -1.0), 10.0f));
		auto TileNode2 = std::make_shared<hivePagedLOD::CTileNode>();
		TileNode2->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(0.0f, 0.414, -1.0), 10.0f));
		auto TileNode3 = std::make_shared<hivePagedLOD::CTileNode>();
		TileNode3->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(0.0f, 0.0f, -1.0), 10.0f));
		auto TileNode4 = std::make_shared<hivePagedLOD::CTileNode>();
		TileNode4->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(315.19, 206.793, -500.5), 10.0f));
		auto TileNode5 = std::make_shared<hivePagedLOD::CTileNode>();
		TileNode5->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(320.19, 206.793, -500.5), 10.0f));
		auto TileNode6 = std::make_shared<hivePagedLOD::CTileNode>();
		TileNode6->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(325.19, 206.793, -500.5), 10.0f));

		TempTileNodeSet.emplace_back(TileNode0);
		TempTileNodeSet.emplace_back(TileNode1);
		TempTileNodeSet.emplace_back(TileNode2);
		TempTileNodeSet.emplace_back(TileNode3);
		TempTileNodeSet.emplace_back(TileNode4);
		TempTileNodeSet.emplace_back(TileNode5);
		TempTileNodeSet.emplace_back(TileNode6);

		return TempTileNodeSet;
	}

	const std::vector<glm::vec4> getFrustumPlanes()
	{
		std::vector<glm::vec4> TempFrustumPlanes;

		glm::vec4 Vec0(0.0f, 0.0f, -1.0f, -1.0f);
		glm::vec4 Vec1(0.0f, 0.0f, 1.0f, 1000.0f);
		glm::vec4 Vec2(0.0f, -0.924, -0.383, 0.0f);
		glm::vec4 Vec3(0.0f, 0.924, -0.383, 0.0f);
		glm::vec4 Vec4(0.849, 0.0f, -0.528, 0.0f);
		glm::vec4 Vec5(-0.849, 0.0f, -0.528, 0.0f);

		TempFrustumPlanes.emplace_back(Vec0);
		TempFrustumPlanes.emplace_back(Vec1);
		TempFrustumPlanes.emplace_back(Vec2);
		TempFrustumPlanes.emplace_back(Vec3);
		TempFrustumPlanes.emplace_back(Vec4);
		TempFrustumPlanes.emplace_back(Vec5);

		return TempFrustumPlanes;
	}

	std::vector<std::string> getGeoFileNameSet()
	{
		std::vector<std::string> TempGeoFileNameSet;

		std::string GeoFileName0 = "T1_111_222.bin";
		std::string GeoFileName1 = "T1__111.bin";
		std::string GeoFileName2 = "T1_111_222.BIN";
		std::string GeoFileName3 = "T1222.bin";
		std::string GeoFileName4 = "T1222";
		std::string GeoFileName5 = "T1222.bint";

		TempGeoFileNameSet.emplace_back(GeoFileName0);
		TempGeoFileNameSet.emplace_back(GeoFileName1);
		TempGeoFileNameSet.emplace_back(GeoFileName2);
		TempGeoFileNameSet.emplace_back(GeoFileName3);
		TempGeoFileNameSet.emplace_back(GeoFileName4);
		TempGeoFileNameSet.emplace_back(GeoFileName5);

		return TempGeoFileNameSet;

	}

	std::vector<std::string> getTexFileNameSet()
	{
		std::vector<std::string> TempTexFileNameSet;

		std::string GeoFileName0 = "T1.bint";
		std::string GeoFileName1 = "T1..bint";
		std::string GeoFileName2 = "T1.1.bint";
		std::string GeoFileName3 = "T1222bint";
		std::string GeoFileName4 = "T1222";
		std::string GeoFileName5 = "T1222.bin";

		TempTexFileNameSet.emplace_back(GeoFileName0);
		TempTexFileNameSet.emplace_back(GeoFileName1);
		TempTexFileNameSet.emplace_back(GeoFileName2);
		TempTexFileNameSet.emplace_back(GeoFileName3);
		TempTexFileNameSet.emplace_back(GeoFileName4);
		TempTexFileNameSet.emplace_back(GeoFileName5);

		return TempTexFileNameSet;

	}
}