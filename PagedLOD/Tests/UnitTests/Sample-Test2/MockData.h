#pragma once
#include "TileNode.h"
#include "Common.h"
#include "PreferredTileNodeGenerator.h"

#include <GLM/glm.hpp>

#include <vector>

namespace MockData
{
	const std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> getTileNodeSet()
	{
		//FIXME:测试用例的数据封装可读性不高
		//指数级增长，假设使用以10为第一级的LOD，距离为1024
		std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> TileNodeSet;
		auto TileNode0 = std::make_shared<hivePagedLOD::CTileNode>();
		//TileNode0->setRange(41.9909f);
		TileNode0->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(0.0f), 41.9909f));
		TileNode0->setTileNodeName("Node0");
		TileNode0->setLODLevel(10);

		auto TileNode1 = std::make_shared<hivePagedLOD::CTileNode>();
		//TileNode1->setRange(83.9818f);
		TileNode1->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(0.0f), 41.9909f));
		TileNode1->setTileNodeName("Node1");
		TileNode1->setLODLevel(9);

		auto TileNode2 = std::make_shared<hivePagedLOD::CTileNode>();
		//TileNode2->setRange(167.964f);
		TileNode2->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(0.0f), 41.9909f));
		TileNode2->setTileNodeName("Node2");
		TileNode2->setLODLevel(8);

		auto TileNode3 = std::make_shared<hivePagedLOD::CTileNode>();
		//TileNode3->setRange(334.751f);
		TileNode3->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(0.0f), 41.8438f));
		TileNode3->setTileNodeName("Node3");
		TileNode3->setLODLevel(7);

		auto TileNode4 = std::make_shared<hivePagedLOD::CTileNode>();
		//TileNode4->setRange(417.568f);
		TileNode4->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(0.0f), 26.098f));
		TileNode4->setTileNodeName("Node4");
		TileNode4->setLODLevel(6);

		auto TileNode5 = std::make_shared<hivePagedLOD::CTileNode>();
		//TileNode5->setRange(448.281f);
		TileNode5->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(0.0f), 14.0088f));
		TileNode5->setTileNodeName("Node5");
		TileNode5->setLODLevel(5);

		auto TileNode6 = std::make_shared<hivePagedLOD::CTileNode>();
		//TileNode6->setRange(0.25f);
		TileNode6->setBoundingSphere(hivePagedLOD::CBoundingSphere(glm::vec3(0.0f), 1.0f));
		TileNode6->setTileNodeName("Node6");
		TileNode6->setLODLevel(-2);

		TileNode1->setParent(TileNode0);
		TileNode2->setParent(TileNode1); 
		TileNode3->setParent(TileNode2);
		TileNode4->setParent(TileNode3);
		TileNode5->setParent(TileNode4);

		TileNode0->addChild(TileNode1);
		TileNode1->addChild(TileNode2);
		TileNode2->addChild(TileNode3);
		TileNode3->addChild(TileNode4);
		TileNode4->addChild(TileNode5);

		TileNodeSet.emplace_back(TileNode0);
		TileNodeSet.emplace_back(TileNode6);
		return TileNodeSet;
	}

	const hivePagedLOD::SViewInfo getViewInfo()
	{
		hivePagedLOD::SViewInfo ViewInfo0;
		ViewInfo0.ViewPortInfo.Width = 1080;
		ViewInfo0.ViewPortInfo.Height = 720;
		ViewInfo0.CameraInfo.Position = glm::vec3(0.0f, 0.0f, 0.0f);
		ViewInfo0.CameraInfo.Front = glm::vec3(0.0f, 0.0f, 1.0f);
		ViewInfo0.CameraInfo.Up = glm::vec3(0.0f, 1.0f, 0.0f);
		ViewInfo0.CameraInfo.FOV = 45.0f;
		ViewInfo0.CameraInfo.NearPlane = 10.0f;
		ViewInfo0.CameraInfo.FarPlane = 10000.0f;

		return ViewInfo0;
	}

	std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> getExceptResultSet()
	{
		std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> Result;
		auto Node0 = std::make_shared<hivePagedLOD::CTileNode>();
		auto Node1 = std::make_shared<hivePagedLOD::CTileNode>();
		auto Node2 = std::make_shared<hivePagedLOD::CTileNode>();
		auto Node3 = std::make_shared<hivePagedLOD::CTileNode>();
		auto Node4 = std::make_shared<hivePagedLOD::CTileNode>();
		auto Node5 = std::make_shared<hivePagedLOD::CTileNode>();
		Node0->setTileNodeName("Node0");
		Node1->setTileNodeName("Node1");
		Node2->setTileNodeName("Node2");
		Node3->setTileNodeName("Node3");
		Node4->setTileNodeName("Node4");
		Node5->setTileNodeName("Node5");

		//FIXME:期望结果的封装看不太懂
		Result.emplace_back(Node5);
		//50
		Result.emplace_back(Node4);
		//100
		Result.emplace_back(Node3);
		Result.emplace_back(Node3);
		Result.emplace_back(Node3);
		//250
		Result.emplace_back(Node2);
		Result.emplace_back(Node2);
		Result.emplace_back(Node2);
		Result.emplace_back(Node2);
		Result.emplace_back(Node2);
		//500
		Result.emplace_back(Node1);
		Result.emplace_back(Node1);
		Result.emplace_back(Node1);
		Result.emplace_back(Node1);
		Result.emplace_back(Node1);
		Result.emplace_back(Node1);
		Result.emplace_back(Node1);
		Result.emplace_back(Node1);
		Result.emplace_back(Node1);
		Result.emplace_back(Node1);
		//1000
		Result.emplace_back(Node0);
		Result.emplace_back(Node0);
		Result.emplace_back(Node0);
		Result.emplace_back(Node0);
		Result.emplace_back(Node0);
		//1250
		return Result;
	}

	static bool isSameTileNode(const std::shared_ptr<hivePagedLOD::CTileNode>& vTileNode1, const std::shared_ptr<hivePagedLOD::CTileNode>& vTileNode2)
	{
		//if (
		//	vTileNode1->getCreatedByFileName() != vTileNode2->getCreatedByFileName() ||
		//	vTileNode1->getGeometryFileName() != vTileNode2->getGeometryFileName() ||
		//	vTileNode1->getTextureFileName() != vTileNode2->getTextureFileName() ||
		//	vTileNode1->getGeometryFileSize() != vTileNode2->getGeometryFileSize() ||
		//	vTileNode1->getTextureFileSize() != vTileNode2->getTextureFileSize() ||
		//	std::fabs(vTileNode1->getBoundingSphere().getCenter().x - vTileNode2->getBoundingSphere().getCenter().x) > std::numeric_limits<float>::epsilon() ||
		//	std::fabs(vTileNode1->getBoundingSphere().getCenter().y - vTileNode2->getBoundingSphere().getCenter().y) > std::numeric_limits<float>::epsilon() ||
		//	std::fabs(vTileNode1->getBoundingSphere().getCenter().z - vTileNode2->getBoundingSphere().getCenter().z) > std::numeric_limits<float>::epsilon() ||
		//	std::fabs(vTileNode1->getBoundingSphere().getRadius() - vTileNode2->getBoundingSphere().getRadius()) > std::numeric_limits<float>::epsilon() ||
		//	std::fabs(vTileNode1->getRange() - vTileNode2->getRange()) > std::numeric_limits<float>::epsilon()
		//	)
		//	return false;
		//return true;

		return vTileNode1->getTileNodeName() == vTileNode2->getTileNodeName();
	}
}