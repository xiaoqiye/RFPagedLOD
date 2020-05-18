#include "MemoryBufferManager.h"
#include "Utils.h"
#include "Common.h"
#include "TileNode.h"
#include "GPUTask.h"
#include <iostream>
#include "Timer.h"

using namespace hivePagedLOD;

CMemoryBufferManager::CMemoryBufferManager(const std::shared_ptr<CPipelineTileNodeLoader2MemoryBufferManager>& vInputPipelineFromLoader, const std::shared_ptr<CPipelineMemoryBufferManager2GPUThread>& vOutputPipelineToGPUThread)
	: m_pInputPipelineFromLoader(vInputPipelineFromLoader),
	m_pOutputPipelineToGPUThread(vOutputPipelineToGPUThread)
{
	_ASSERTE(m_pInputPipelineFromLoader && m_pOutputPipelineToGPUThread);
}

//****************************************************************************
//FUNCTION:
void CMemoryBufferManager::start()
{
	m_Thread = std::thread([&] {__workByLoadedMeshBuffer(); });
}

//****************************************************************************
//FUNCTION:record all root and send gpu task
void CMemoryBufferManager::doFirstFrame()
{
	std::shared_ptr<SLoaderResult> LoaderResult;
	m_pInputPipelineFromLoader->tryPop(1, LoaderResult);
	_ASSERTE(LoaderResult->TileNodeUIDSet.size() == LoaderResult->LoadedMeshBufferSet.size());

	unsigned int LoadedMeshBufferSetSize = static_cast<unsigned int>(LoaderResult->LoadedMeshBufferSet.size());
	for (unsigned int i = 0; i < LoadedMeshBufferSetSize; ++i)
	{
		auto& MeshBuffer = LoaderResult->LoadedMeshBufferSet[i];
		auto& UID = LoaderResult->TileNodeUIDSet[i];
		//add:show LODLevel
		m_pOutputPipelineToGPUThread->tryPush(std::make_shared<SGPUTaskGenGeoBuffer>(UID, MeshBuffer->pGeometry, MeshBuffer->LODLevel));
		m_pOutputPipelineToGPUThread->tryPush(std::make_shared<SGPUTaskGenTexBuffer>(UID, MeshBuffer->pTexture, MeshBuffer->LODLevel));

		const auto GeoSize = MeshBuffer->pGeometry->IndexCount * sizeof(unsigned int) + MeshBuffer->pGeometry->VertexCount * sizeof(OSG_VERTEX_SIZE);
		const auto TexSize = MeshBuffer->pTexture->TexInfo.Size;

		unsigned int TileNum = UID_TILE_NUM_MASK & UID;
		TileNum >>= OFFSET_BIT;
		auto& TempTileNodeLoadCost = m_pLoadCostMapPointer->at(TileNum)[UID];
		TempTileNodeLoadCost.GeoInMemory = true;
		TempTileNodeLoadCost.TexInMemory = true;
		TempTileNodeLoadCost.LoadCost = 0;

		m_MemoryMeshBufferMap[UID] = MeshBuffer;
		m_UsedMemoryCount += GeoSize + TexSize;
		//	not push into LRU list, it cannot be expired
	}
}

//****************************************************************************
//FUNCTION:
void CMemoryBufferManager::__workByLoadedMeshBuffer()
{
	while (!m_Close)
	{
		std::shared_ptr<SLoaderResult> LoaderResult;
		if (m_pInputPipelineFromLoader->tryPop(1, LoaderResult))
		{
			CTimer::getInstance()->tick(__FUNCTION__);

			__processMeshBufferSet(LoaderResult->TileNodeUIDSet, LoaderResult->LoadedMeshBufferSet);
			__manageMemory(LoaderResult->DrawUIDSet);
			
			const std::shared_ptr<SGPUTaskRender> RenderNameSet = std::make_shared<SGPUTaskRender>(std::make_shared<std::vector<unsigned int>>(LoaderResult->DrawUIDSet));
			m_pOutputPipelineToGPUThread->tryPush(RenderNameSet);
			
			CTimer::getInstance()->tock(__FUNCTION__);
			if (CTimer::getInstance()->needOutput() && CTimer::getInstance()->isRegistered(__FUNCTION__))
				std::cout << __FUNCTION__ << CTimer::getInstance()->getCostTimeByName(__FUNCTION__) << std::endl;
		}
	}
}

//****************************************************************************
//FUNCTION:
void CMemoryBufferManager::__processMeshBufferSet(const std::vector<unsigned int>& vTileNodeUIDSet, const std::vector<std::shared_ptr<SMemoryMeshBuffer>>& vLoadedMeshBufferSet)
{
	if (vLoadedMeshBufferSet.empty())
		return;

	std::set<unsigned int> WaitUpdateTextureUIDSet; //update load cost map

	_ASSERTE(vLoadedMeshBufferSet.size() == vTileNodeUIDSet.size());
	unsigned int LoadedMeshBufferSetSize = static_cast<unsigned int>(vLoadedMeshBufferSet.size());
	for (unsigned int i = 0; i < LoadedMeshBufferSetSize; ++i)
	{
		auto& MeshBuffer = vLoadedMeshBufferSet[i];
		auto& UID = vTileNodeUIDSet[i];

		if (m_MemoryMeshBufferMap.find(UID) != m_MemoryMeshBufferMap.end())
			continue;

		bool IsTexInMemory = true;
		if (MeshBuffer->pTexture)
		{
			IsTexInMemory = false;
			//add:show LODLevel
			std::shared_ptr<SGPUTaskGenTexBuffer> GenTexBufferTask = std::make_shared<SGPUTaskGenTexBuffer>(UID, MeshBuffer->pTexture, MeshBuffer->LODLevel);
			m_pOutputPipelineToGPUThread->tryPush(GenTexBufferTask);
			WaitUpdateTextureUIDSet.insert(UID);
		}
		else
		{
			auto TempTexture = getTextureByUIDIfExist(UID);
			if (!TempTexture)
				return;
			MeshBuffer->pTexture = TempTexture;
		}
		//add:show LODLevel
		std::shared_ptr<SGPUTaskGenGeoBuffer> GenGeoBufferTask = std::make_shared<SGPUTaskGenGeoBuffer>(UID, MeshBuffer->pGeometry, MeshBuffer->LODLevel);
		m_pOutputPipelineToGPUThread->tryPush(GenGeoBufferTask);

		const auto GeoSize = MeshBuffer->pGeometry->IndexCount * sizeof(unsigned int) + MeshBuffer->pGeometry->VertexCount * sizeof(OSG_VERTEX_SIZE);
		const auto TexSize = MeshBuffer->pTexture->TexInfo.Size;

		unsigned int TileNum = UID_TILE_NUM_MASK & UID;
		TileNum >>= OFFSET_BIT;
		auto& TempTileNodeLoadCost = m_pLoadCostMapPointer->at(TileNum)[UID];
		TempTileNodeLoadCost.GeoInMemory = true;
		TempTileNodeLoadCost.TexInMemory = true;
		TempTileNodeLoadCost.LoadCost = 0;

		_ASSERTE(m_MemoryMeshBufferMap.find(UID) == m_MemoryMeshBufferMap.end() && MeshBuffer->pTexture);
		m_MemoryMeshBufferMap[UID] = MeshBuffer;
		m_UsedMemoryCount += GeoSize + TexSize * static_cast<std::uintmax_t>(!IsTexInMemory);
		m_pMemoryMeshLRUList->addNewMemoryElement(UID, GeoSize, TexSize);
	}
	__updateLoadCostMapByUIDSet(WaitUpdateTextureUIDSet, true);
}

//****************************************************************************
//FUNCTION:更新代价表
void CMemoryBufferManager::__updateLoadCostMapByUIDSet(const std::set<unsigned int>& vUIDSet, bool vSetFlag)
{
	if (vUIDSet.empty())
		return;

	for (auto& UID : vUIDSet)
	{
		unsigned int TileNum = UID_TILE_NUM_MASK & UID;
		TileNum >>= OFFSET_BIT;
		auto& SelfLoadCost = m_pLoadCostMapPointer->at(TileNum)[UID];
		SelfLoadCost.TexInMemory = vSetFlag;
		SelfLoadCost.updateCost();

		auto& BrotherUIDSet = m_pTileNodeBrotherMapPointer->at(TileNum)[UID];
		for (auto& BrotherUID: BrotherUIDSet)
		{
			if (BrotherUID != 0)
			{
				auto& LoadCost = m_pLoadCostMapPointer->at(TileNum)[BrotherUID];
				LoadCost.TexInMemory = vSetFlag;
				LoadCost.updateCost();
			}
		}
	}
}

//****************************************************************************
//FUNCTION:
void CMemoryBufferManager::__manageMemory(const std::vector<unsigned int>& vDrawUIDSet)
{
	if (m_UsedMemoryCount < m_pMemoryMeshLRUList->getLimit())
		return;
	
	std::set<unsigned int> UpdateNameSet;
	for (auto& UID : vDrawUIDSet)
		UpdateNameSet.insert(UID);

	m_pMemoryMeshLRUList->updateList(UpdateNameSet);
	m_pMemoryMeshLRUList->deleteExpiredTileNode();
}

//****************************************************************************
//FUNCTION:
bool CMemoryBufferManager::isTextureExistInMemory(unsigned int vUID) const
{
	unsigned int TileNum = UID_TILE_NUM_MASK & vUID;
	TileNum >>= OFFSET_BIT;
	auto& BrotherUIDSet = m_pTileNodeBrotherMapPointer->at(TileNum)[vUID];

	for (auto& TempBrotherUID : BrotherUIDSet)
		if (m_MemoryMeshBufferMap.find(TempBrotherUID) != m_MemoryMeshBufferMap.end())
			return true;
	
	return false;
}

//****************************************************************************
//FUNCTION:
std::shared_ptr<STexture> CMemoryBufferManager::getTextureByUIDIfExist(unsigned int vUID) const
{
	unsigned int TileNum = UID_TILE_NUM_MASK & vUID;
	TileNum >>= OFFSET_BIT;
	auto& BrotherUIDSet = m_pTileNodeBrotherMapPointer->at(TileNum)[vUID];

	for (auto& TempBrotherUID : BrotherUIDSet)
		if (m_MemoryMeshBufferMap.find(TempBrotherUID) != m_MemoryMeshBufferMap.end())
			return m_MemoryMeshBufferMap.find(TempBrotherUID)->second->pTexture;
	
	return nullptr;
}

//****************************************************************************
//FUNCTION:
void CMemoryBufferManager::clearMemoryBuffers()
{
	m_MemoryMeshBufferMap.clear();
	m_pMemoryMeshLRUList->clearLRUList();
	m_UsedMemoryCount = 0;
	resetLoadCostMap();
}

//****************************************************************************
//FUNCTION:
void CMemoryBufferManager::resetLoadCostMap()
{
	CScene::getInstance()->resetLoadCostMap();
	m_pLoadCostMapPointer = CScene::getInstance()->getLoadCostMapPointer();
}

//****************************************************************************
//FUNCTION:
void CMemoryBufferManager::initMapPointer()
{
	m_pLoadCostMapPointer = CScene::getInstance()->getLoadCostMapPointer();
	m_pTileNodeAncestorMapPointer = CScene::getInstance()->getAncestorMapPointer();
	m_pTileNodeBrotherMapPointer = CScene::getInstance()->getBrotherMapPointer();
}

//****************************************************************************
//FUNCTION:
void CMemoryBufferManager::useLRUList(bool vUes)
{
	if (vUes && !m_pMemoryMeshLRUList)
	{
		__initSplayList();
	}
	else if (!vUes && m_pMemoryMeshLRUList)
	{
		m_pMemoryMeshLRUList = nullptr;
	}
}

//****************************************************************************
//FUNCTION:
void CMemoryBufferManager::setMemoryLimit(std::uintmax_t vLimit)
{
	_ASSERTE(m_pMemoryMeshLRUList);
	m_pMemoryMeshLRUList->setLimit(vLimit);
}

//****************************************************************************
//FUNCTION:
void CMemoryBufferManager::__initSplayList()
{
	_ASSERTE(!m_pMemoryMeshLRUList);
	m_pMemoryMeshLRUList = std::make_shared<CMemoryMeshLRUList>(this);
	m_pMemoryMeshLRUList->setLimit(DEFAULT_MEMORY_LIMIT);
	m_pMemoryMeshLRUList->setOverflowCallback([&](unsigned int UID)
		{
			const std::shared_ptr<SGPUTaskDelGeoBuffer> DelGeoBufferTask = std::make_shared<SGPUTaskDelGeoBuffer>(UID);
			m_pOutputPipelineToGPUThread->tryPush(DelGeoBufferTask);
		
			unsigned int TileNum = UID_TILE_NUM_MASK & UID;
			TileNum >>= OFFSET_BIT;
			auto& LoadCost = m_pLoadCostMapPointer->at(TileNum)[UID];
			LoadCost.GeoInMemory = false;
			m_MemoryMeshBufferMap.erase(UID);

			const bool TextureInMemory = isTextureExistInMemory(UID);
			if (!TextureInMemory)
			{
				const std::shared_ptr<SGPUTaskDelTexBuffer> DelTexBufferTask = std::make_shared<SGPUTaskDelTexBuffer>(UID);
				m_pOutputPipelineToGPUThread->tryPush(DelTexBufferTask);

				std::set<unsigned int> UIDSet;
				UIDSet.insert(UID);
				__updateLoadCostMapByUIDSet(UIDSet, false);
				return;
			}
			LoadCost.TexInMemory = true;
			LoadCost.updateCost();
		});

	for (auto& MemoryMeshBuffer : m_MemoryMeshBufferMap)
	{
		auto& UID = MemoryMeshBuffer.first;
		auto& MeshBuffer = MemoryMeshBuffer.second;
		_ASSERTE(MeshBuffer);
		const auto GeoSize = MeshBuffer->pGeometry->IndexCount * sizeof(unsigned int) + MeshBuffer->pGeometry->VertexCount * sizeof(OSG_VERTEX_SIZE);
		const auto TexSize = MeshBuffer->pTexture->TexInfo.Size;
		m_pMemoryMeshLRUList->addNewMemoryElement(UID, GeoSize, TexSize);
	}
}