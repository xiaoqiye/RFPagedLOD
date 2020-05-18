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
			if (m_LastViewInfo == m_ViewInfo)
			{
				std::vector<std::vector<std::shared_ptr<CTileNode>>> EmptyPreferredTileNodeSet;
				std::vector<unsigned int> EmptyMaxDeepSet;
				m_pOutputPipeline->tryPush(SPreferredResult(EmptyPreferredTileNodeSet, EmptyMaxDeepSet));
				continue;
			}
			m_LastViewInfo = m_ViewInfo;
			CTimer::getInstance()->tick(__FUNCTION__);

			if (m_FrustumPlanes.empty()) 
				CUtils::getInstance()->calculateFrustumPlane(m_FrustumPlanes, m_ViewInfo);
			CUtils::getInstance()->calculateMatrices(m_ViewMatrix, m_ProjectionMatrix, m_ViewInfo);
			if (m_LastPreferredTileNumSet.size() != 0)
				CScene::getInstance()->resetTileNodeStatus(m_LastPreferredTileNumSet);

			auto& TileRootNodeSet = CScene::getInstance()->fetchTileSet();
			const auto Size = TileRootNodeSet.size();
			std::vector<std::vector<std::shared_ptr<CTileNode>>> PreferredTileNodeSet(Size);
			std::vector<unsigned int> MaxDeepSet(Size, 0);
			for (unsigned int i = 0; i < Size; ++i)
				if (__isTileVisible(TileRootNodeSet[i]))
					__generatePreferredTileNodeSet(TileRootNodeSet[i], PreferredTileNodeSet[i], MaxDeepSet[i]);

			m_LastPreferredTileNumSet.clear();
			for (auto& PreferredTileNode : PreferredTileNodeSet)
			{
				if (!PreferredTileNode.empty())
				{
					unsigned int TileNum = UID_TILE_NUM_MASK & PreferredTileNode[0]->getUID();
					TileNum >>= OFFSET_BIT;
					m_LastPreferredTileNumSet.emplace_back(TileNum);
				}
			}

			m_pOutputPipeline->tryPush(SPreferredResult(PreferredTileNodeSet, MaxDeepSet));

			CTimer::getInstance()->tock(__FUNCTION__);
			if (CTimer::getInstance()->needOutput() && CTimer::getInstance()->isRegistered(__FUNCTION__))
				std::cout << __FUNCTION__ << CTimer::getInstance()->getCostTimeByName(__FUNCTION__) << std::endl;
		}
	}
}

//****************************************************************************
//FUNCTION:深度优先遍历树，进行剔除和LOD计算，到叶子节点一定preferred
void CPreferredTileNodeGenerator::__generatePreferredTileNodeSet(std::shared_ptr<CTileNode>& vTileNode, std::vector<std::shared_ptr<CTileNode>>& voPreferredTileNode, unsigned int& voPreferredSetMaxDeep) const
{
	if (!vTileNode) return;

	auto NodeDeep = vTileNode->getNodeDeep();
	if (vTileNode->getNumChildren() == 0)
	{
		voPreferredSetMaxDeep = NodeDeep > voPreferredSetMaxDeep ? NodeDeep : voPreferredSetMaxDeep;
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
			voPreferredSetMaxDeep = NodeDeep > voPreferredSetMaxDeep ? NodeDeep : voPreferredSetMaxDeep;
			vTileNode->setMatchLOD(true);
			voPreferredTileNode.push_back(vTileNode);
		}
		else
		{
			vTileNode->setMatchLOD(false);
			for (unsigned int i = 0; i < vTileNode->getNumChildren(); ++i)
				__generatePreferredTileNodeSet(vTileNode->fetchChildPointerAt(i), voPreferredTileNode, voPreferredSetMaxDeep);
		}
	}
	else
	{
		vTileNode->setIsVisible(false);
	}
}

//****************************************************************************
//FUNCTION:
bool CPreferredTileNodeGenerator::__isTileVisible(const std::shared_ptr<CTileNode>& vTile) const
{
	if (!vTile) return false;
	_ASSERTE(vTile->getBoundingSphere().isValid());

	const auto Center = m_ViewMatrix * MODEL_MATRIX * glm::vec4(vTile->getBoundingSphere().getCenter(), 1.0f);
	const auto Radius = vTile->getBoundingSphere().getRadius();
	return CUtils::getInstance()->isBoundingSphereInFrustum(m_FrustumPlanes, Center, Radius);
}

//****************************************************************************
//FUNCTION:
bool CPreferredTileNodeGenerator::__isPreferredTileNode(const std::shared_ptr<CTileNode>& vTileNode) const
{
	if (!vTileNode) return false;

	//FIXME:计算规则如何得出
	const auto DistanceFactor = std::powf(2, static_cast<float>(vTileNode->getLODLevel()));
	const auto Distance = glm::distance(m_ViewInfo.CameraInfo.Position, glm::vec3(MODEL_MATRIX * glm::vec4(vTileNode->getBoundingSphere().getCenter(), 1.0f)));
	return DistanceFactor < Distance;
}