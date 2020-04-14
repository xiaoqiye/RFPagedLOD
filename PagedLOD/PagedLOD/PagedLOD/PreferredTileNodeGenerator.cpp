#include "PreferredTileNodeGenerator.h"
#include "Utils.h"

#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/detail/func_geometric.hpp"
#include "Timer.h"
#include <iostream>
#include <set>

using namespace hivePagedLOD;

CPreferredTileNodeGenerator::CPreferredTileNodeGenerator(const std::shared_ptr<CPipelineFrameGenerator2PreferredTileNodeGenerator>& vInputPipeline, const std::shared_ptr<CPipelinePreferred2RenderingTileNodeGenerator>& vOutputPipeline) : m_pInputPipeline(vInputPipeline), m_pOutputPipeline(vOutputPipeline)
{
	_ASSERTE(m_pInputPipeline && m_pOutputPipeline);
}

//****************************************************************************
//FUNCTION:
void CPreferredTileNodeGenerator::start()
{
	m_Thread = std::thread([&] {__workByViewInfo(); });
}

//****************************************************************************
//FUNCTION:
void CPreferredTileNodeGenerator::__workByViewInfo()
{
	while (!m_Close)
	{
		if (m_pInputPipeline->tryPop(1, m_ViewInfo))
		{
			if (m_LastViewInfo.CameraInfo.Front == m_ViewInfo.CameraInfo.Front
				&& m_LastViewInfo.CameraInfo.Up == m_ViewInfo.CameraInfo.Up
				&& m_LastViewInfo.CameraInfo.Position == m_ViewInfo.CameraInfo.Position)
			{
				std::vector<std::vector<std::shared_ptr<CTileNode>>> EmptyPreferredTileNodeSet;
				std::vector<unsigned int> EmptyMaxDeepSet;
				m_pOutputPipeline->tryPush(SPreferredResult(EmptyPreferredTileNodeSet, EmptyMaxDeepSet));
				continue;
			}
			
			m_LastViewInfo = m_ViewInfo;
			
			CTimer::getInstance()->tick(__FUNCTION__);

			if (m_FrustumPlanes.empty()) CUtils::getInstance()->calculateFrustumPlane(m_FrustumPlanes, m_ViewInfo);
			CUtils::getInstance()->calculateMatrices(m_ViewMatrix, m_ProjectionMatrix, m_ViewInfo);

			CScene::getInstance()->resetTileNodeStatus(m_LastPreferredTileNumSet);

			auto& TileRootNodeSet = CScene::getInstance()->fetchTileSet();
			const auto Size = TileRootNodeSet.size();
			std::vector<std::vector<std::shared_ptr<CTileNode>>> PreferredTileNodeSet;
			PreferredTileNodeSet.resize(Size);
			std::vector<unsigned int> MinDeepSet(Size, INT_MAX);
			std::vector<unsigned int> MaxDeepSet(Size, 0);
			for (unsigned int i = 0; i < Size; ++i)
			{
				if (__isTileVisible(TileRootNodeSet[i]))
					__generatePreferredTileNodeSet(TileRootNodeSet[i], PreferredTileNodeSet[i], MinDeepSet[i], MaxDeepSet[i]);
			}

			m_LastPreferredTileNumSet.clear();
			for (auto& i : PreferredTileNodeSet)
			{
				if (!i.empty())
				{
					unsigned int TileNum = UID_TILE_NUM_MASK & i[0]->getUID();
					TileNum >>= OFFSET_BIT;
					m_LastPreferredTileNumSet.emplace_back(TileNum);
				}
			}
			//for (auto& i : PreferredTileNodeSet)
				//std::sort(i.begin(), i.end(), [](const std::shared_ptr<CTileNode>& lhs, const std::shared_ptr<CTileNode>& rhs) {return lhs->getNodeDeep() > rhs->getNodeDeep(); });

			//for (auto& i : PreferredTileNodeSet)
			//	for (auto& k : i)
			//		std::cout << k->getGeometryFileName() << std::endl;
			
			m_pOutputPipeline->tryPush(SPreferredResult(PreferredTileNodeSet, MaxDeepSet));

			CTimer::getInstance()->tock(__FUNCTION__);
			if (CTimer::getInstance()->needOutput() && CTimer::getInstance()->isRegistered(__FUNCTION__))
				std::cout << __FUNCTION__ << CTimer::getInstance()->getCostTimeByName(__FUNCTION__) << std::endl;
		}
	}
}

//****************************************************************************
//FUNCTION:深度优先遍历树，进行剔除和LOD计算，到叶子节点一定preferred
void CPreferredTileNodeGenerator::__generatePreferredTileNodeSet(std::shared_ptr<CTileNode>& vTileNode, std::vector<std::shared_ptr<CTileNode>>& voPreferredTileNode, unsigned int& voPreferredSetMinDeep, unsigned int& voPreferredSetMaxDeep) const
{
	if (!vTileNode) return;

	if (vTileNode->getNumChildren() == 0)
	{
		if (vTileNode->getNodeDeep() > voPreferredSetMaxDeep)
			voPreferredSetMaxDeep = vTileNode->getNodeDeep();
		
		if (vTileNode->getNodeDeep() < voPreferredSetMinDeep)
			voPreferredSetMinDeep = vTileNode->getNodeDeep();

		vTileNode->setMatchLOD(true);
		vTileNode->setIsVisible(true);
		voPreferredTileNode.push_back(vTileNode);
		return;
	}

	if (__isTileVisible(vTileNode))
	{
		vTileNode->setIsVisible(true);
		if (__isPreferredTileNode(vTileNode))
		{
			if (vTileNode->getNodeDeep() > voPreferredSetMaxDeep)
				voPreferredSetMaxDeep = vTileNode->getNodeDeep();
			
			if (vTileNode->getNodeDeep() < voPreferredSetMinDeep)
				voPreferredSetMinDeep = vTileNode->getNodeDeep();

			vTileNode->setMatchLOD(true);
			voPreferredTileNode.push_back(vTileNode);
		}
		else
		{
			vTileNode->setMatchLOD(false);
			for (unsigned int i = 0; i < vTileNode->getNumChildren(); ++i)
				__generatePreferredTileNodeSet(vTileNode->fetchChildPointerAt(i), voPreferredTileNode, voPreferredSetMinDeep, voPreferredSetMaxDeep);
		}
	}
	else
	{
		vTileNode->setIsVisible(false);
	}
}

////****************************************************************************
////FUNCTION:深度优先遍历树，进行剔除和LOD计算，到叶子节点一定preferred
//void CPreferredTileNodeGenerator::__generatePreferredTileNodeSet(const std::shared_ptr<CTileNode>& vTileNode, std::vector<std::shared_ptr<CTileNode>>& voPreferredTileNode) const
//{
//	if (!vTileNode) return;
//
//	if (vTileNode->getNumChildren() == 0)
//	{
//		voPreferredTileNode.push_back(vTileNode);
//		return;
//	}
//
//	if (__isTileVisible(vTileNode))
//	{
//		if (__isPreferredTileNode(vTileNode))
//		{
//			voPreferredTileNode.push_back(vTileNode);
//		}
//		else
//		{
//			for (unsigned int i = 0; i < vTileNode->getNumChildren(); ++i)
//				__generatePreferredTileNodeSet(vTileNode->getChildPointerAt(i), voPreferredTileNode);
//		}
//	}
//}

//****************************************************************************
//FUNCTION:
bool CPreferredTileNodeGenerator::__isTileVisible(const std::shared_ptr<CTileNode>& vTile) const
{
	if (!vTile) return false;
	_ASSERTE(vTile->getBoundingSphere().isValid());
	auto Center = m_ViewMatrix * MODEL_MATRIX * glm::vec4(vTile->getBoundingSphere().getCenter(), 1.0f);
	auto Radius = vTile->getBoundingSphere().getRadius();
	return CUtils::getInstance()->isBoundingSphereInFrustum(m_FrustumPlanes, Center, Radius);
}

//****************************************************************************
//FUNCTION:
bool CPreferredTileNodeGenerator::__isPreferredTileNode(const std::shared_ptr<CTileNode>& vTileNode) const
{
	////todo:discuss
	////trick
	//float Distance = glm::distance(m_ViewInfo.CameraInfo.Position, glm::vec3(MODEL_MATRIX * glm::vec4(vTileNode->getBoundingSphere().getCenter(), 1.0f)));
	//float T = vTileNode->getRange() / vTileNode->getBoundingSphere().getRadius();
	//float LODMagicNumber = 2048.0f / (Distance + 1.00f);
	//return glm::max(0.0f, LODMagicNumber) < T;

	//correct function
	auto DistanceFactor = std::powf(2, static_cast<float>(vTileNode->getLODLevel()));
	auto Distance = glm::distance(m_ViewInfo.CameraInfo.Position, glm::vec3(MODEL_MATRIX * glm::vec4(vTileNode->getBoundingSphere().getCenter(), 1.0f)));
	return DistanceFactor < Distance;
}