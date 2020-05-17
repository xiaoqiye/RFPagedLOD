#pragma once
#include "Common.h"
#include "GPUBufferCommon.h"
#include "Scene.h"
#include <map>
#include <tbb/concurrent_unordered_map.h>

namespace hivePagedLOD
{
	class CGPUBufferManager
	{
	public:
		CGPUBufferManager() = default;
		~CGPUBufferManager() = default;

		//add:show LODLevel
		void saveGeometryBuffer(unsigned int vGeoBufferUID, const std::pair<SGPUGeometryBufferHandle, unsigned int>& vGeoBufferHandle, int vLODLevel);
		void saveTextureBuffer(unsigned int vTexBufferUID, const TTextureID& vTexBufferHandle, int vLODLevel);
		SGPUGeometryBufferHandle deleteGeometryBuffer(unsigned int vGeoBufferUID);
		TTextureID deleteTextureBuffer(unsigned int vTexBufferUID);

		std::vector<std::shared_ptr<SGPUMeshBuffer>> generateRenderMeshBufferSet(const std::vector<unsigned int>& vMeshBufferUIDSet);
		void initMapPointer() { m_pTileNodeBrotherMapPointer = CScene::getInstance()->getBrotherMapPointer();}
		
		////Utility
		void clearGPUBufferMap();
		std::vector<SGPUGeometryBufferHandle> getGeoBufferHandleSet();
		std::vector<TTextureID> getTexBufferIDSet();
		std::map<unsigned int, std::pair<SGPUGeometryBufferHandle, unsigned int>>& getGeoBufferMap() { return m_GPUGeometryBufferMap; }
		std::map<unsigned int, TTextureID>& getTexBufferMap() { return m_GPUTextureBufferMap; }

	private:
		std::map<unsigned int, std::pair<SGPUGeometryBufferHandle, unsigned int>> m_GPUGeometryBufferMap;
		std::map<unsigned int, TTextureID> m_GPUTextureBufferMap;
		std::map<unsigned int, int> m_UID2LODLevel;
		std::vector<tbb::concurrent_unordered_map<unsigned int, std::vector<unsigned int>>>* m_pTileNodeBrotherMapPointer;
	};
}
