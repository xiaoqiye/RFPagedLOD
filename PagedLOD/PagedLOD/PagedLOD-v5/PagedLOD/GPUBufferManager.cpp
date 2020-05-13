#include "GPUBufferManager.h"
#include "Utils.h"
#include <boost/algorithm/string/predicate.hpp>
#include <iostream>

using namespace hivePagedLOD;

//****************************************************************************
//FUNCTION:
void CGPUBufferManager::saveGeometryBuffer(unsigned int vGeoBufferUID, const std::pair<SGPUGeometryBufferHandle, unsigned int>& vGeoBufferHandle)
{
	const auto pair = m_GPUGeometryBufferMap.emplace(vGeoBufferUID, vGeoBufferHandle);
	if (!pair.second)
		std::cout << "save gpu geometry buffer failed: "<< vGeoBufferUID << std::endl;
}

//****************************************************************************
//FUNCTION:
void CGPUBufferManager::saveTextureBuffer(unsigned int vTexBufferUID, const TTextureID& vTexBufferHandle)
{
	const auto pair = m_GPUTextureBufferMap.emplace(vTexBufferUID, vTexBufferHandle);
	if (!pair.second)
		std::cout << "save gpu texture buffer failed: " << vTexBufferUID << std::endl;
}

//****************************************************************************
//FUNCTION:
SGPUGeometryBufferHandle CGPUBufferManager::deleteGeometryBuffer(unsigned int vGeoBufferUID)
{
	auto it = m_GPUGeometryBufferMap.find(vGeoBufferUID);
	if (it != m_GPUGeometryBufferMap.end())
	{
		auto BufferHandle = it->second.first;
		m_GPUGeometryBufferMap.erase(it);
		return BufferHandle;
	}
	return SGPUGeometryBufferHandle();
}

//****************************************************************************
//FUNCTION:
TTextureID CGPUBufferManager::deleteTextureBuffer(unsigned int vTexBufferUID)
{
	auto it = m_GPUTextureBufferMap.find(vTexBufferUID);
	if (it != m_GPUTextureBufferMap.end())
	{
		auto BufferHandle = it->second;
		m_GPUTextureBufferMap.erase(it);
		return BufferHandle;
	}
	else
	{
		unsigned int TileNum = UID_TILE_NUM_MASK & vTexBufferUID;
		TileNum >>= OFFSET_BIT;
		auto& BrotherUIDSet = m_pTileNodeBrotherMapPointer->at(TileNum)[vTexBufferUID];
		for (auto& BrotherUID : BrotherUIDSet)
		{
			auto BrotherIt = m_GPUTextureBufferMap.find(BrotherUID);
			if (BrotherIt != m_GPUTextureBufferMap.end())
			{
				auto BufferHandle = BrotherIt->second;
				m_GPUTextureBufferMap.erase(BrotherIt);
				return BufferHandle;
			}
		}
	}
	return 0;
}

//****************************************************************************
//FUNCTION:
std::vector<std::shared_ptr<SGPUMeshBuffer>> CGPUBufferManager::generateRenderMeshBufferSet(const std::vector<unsigned int>& vMeshBufferUIDSet)
{
	std::vector<std::shared_ptr<SGPUMeshBuffer>> RenderMeshBufferSet;
	for (auto& UID : vMeshBufferUIDSet)
	{
		auto GeoIt = m_GPUGeometryBufferMap.find(UID);
		auto TexIt = m_GPUTextureBufferMap.find(UID);
		if (TexIt == m_GPUTextureBufferMap.end())
		{
			unsigned int TileNum = UID_TILE_NUM_MASK & UID;
			TileNum >>= OFFSET_BIT;
			auto& BrotherUIDSet = m_pTileNodeBrotherMapPointer->at(TileNum)[UID];
			for (auto& BrotherUID : BrotherUIDSet)
			{
				if (m_GPUTextureBufferMap.find(BrotherUID) != m_GPUTextureBufferMap.end())
				{
					TexIt = m_GPUTextureBufferMap.find(BrotherUID);
					break;
				}
			}
		}

		_ASSERTE(TexIt != m_GPUTextureBufferMap.end() && GeoIt != m_GPUGeometryBufferMap.end());
		if (TexIt != m_GPUTextureBufferMap.end() && GeoIt != m_GPUGeometryBufferMap.end())
		{
			auto TexBuffer = std::make_shared<SGPUTextureBuffer>(TexIt->second);
			auto GeoBuffer = std::make_shared<SGPUGeometryBuffer>(GeoIt->second.first, GeoIt->second.second);

			RenderMeshBufferSet.emplace_back(std::make_shared<SGPUMeshBuffer>(TexBuffer, GeoBuffer));
		}
	}
	return RenderMeshBufferSet;
}

//****************************************************************************
//FUNCTION:
void CGPUBufferManager::clearGPUBufferMap()
{
	m_GPUGeometryBufferMap.clear();
	m_GPUTextureBufferMap.clear();
}

//****************************************************************************
//FUNCTION:
std::vector<SGPUGeometryBufferHandle> CGPUBufferManager::getGeoBufferHandleSet()
{
	std::vector<SGPUGeometryBufferHandle> ResultSet;
	for (auto& GeoIt : m_GPUGeometryBufferMap)
		ResultSet.emplace_back(GeoIt.second.first);
	return ResultSet;
}

//****************************************************************************
//FUNCTION:
std::vector<TTextureID> CGPUBufferManager::getTexBufferIDSet()
{
	std::vector<TTextureID> ResultSet;
	for (auto& TexIt : m_GPUTextureBufferMap)
		ResultSet.emplace_back(TexIt.second);
	return ResultSet;
}
