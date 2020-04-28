#pragma once

#include "Scene.h"
#include "Common.h"
#include "MemoryMeshLRUList.h"
#include "PipelineTileNodeLoader2MemoryBufferManager.h"
#include "PipelineMemoryBufferManager2GPUThread.h"

#include <tbb/concurrent_unordered_map.h>
#include <set>

namespace hivePagedLOD
{
	class CMemoryMeshLRUList;
	class CMemoryBufferManager
	{
	public:
		CMemoryBufferManager(const std::shared_ptr<CPipelineTileNodeLoader2MemoryBufferManager>& vInputPipelineFromLoader,
			const std::shared_ptr<CPipelineMemoryBufferManager2GPUThread>& vOutputPipelineToGPUThread);

		~CMemoryBufferManager() = default;


		void start();
		void join() { _ASSERTE(m_Thread.joinable()); m_Thread.join(); }
		void setFinishSignal(bool vFinish) { m_Close = vFinish; }
		void doFirstFrame();
		
		////MemoryBufferManager Functions
		void __workByLoadedMeshBuffer();
		void __manageMemory(const std::vector<unsigned int>& vDrawUIDSet);

		////LRU List Functions
		bool hasLRUList() const { return m_pMemoryMeshLRUList.get(); }

		void useLRUList(bool vUes);
		void setMemoryLimit(std::uintmax_t vLimit);
		void setUsedMemoryCount(std::uintmax_t vCount) { m_UsedMemoryCount = vCount; }
		std::uintmax_t getUsedMemoryCount() const { return m_UsedMemoryCount; }
		unsigned int getMemoryBufferMapSize() const { return static_cast<unsigned int>(m_MemoryMeshBufferMap.size()); }

		////Utility Functions
		bool isTextureExistInMemory(unsigned int vUID) const;
		std::shared_ptr<STexture> getTextureByUIDIfExist(unsigned int vUID) const;
		void clearMemoryBuffers();

		////Load Cost Map Functions
		void resetLoadCostMap();
		void initMapPointer();
		std::vector<tbb::concurrent_unordered_map<unsigned int, STileNodeLoadCost>>* getLoadCostMapPointer() { return m_pLoadCostMapPointer; }

	private:

		void __processMeshBufferSet(const std::vector<unsigned int>& vTileNodeUIDSet, const std::vector<std::shared_ptr<SMemoryMeshBuffer>>& vLoadedMeshBufferSet);
		void __initSplayList();
		void __updateLoadCostMapByUIDSet(const std::set<unsigned int>& vUIDSet, bool vSetFlag);
		
		std::shared_ptr<CPipelineTileNodeLoader2MemoryBufferManager> m_pInputPipelineFromLoader;
		std::shared_ptr<CPipelineMemoryBufferManager2GPUThread> m_pOutputPipelineToGPUThread;

		std::thread m_Thread;
		bool m_Close = false;

		std::map<unsigned int, std::shared_ptr<SMemoryMeshBuffer>> m_MemoryMeshBufferMap;//<UID, MeshBuffer*>

		std::shared_ptr<CMemoryMeshLRUList> m_pMemoryMeshLRUList = nullptr;
		std::uintmax_t m_UsedMemoryCount = 0;

		std::vector<tbb::concurrent_unordered_map<unsigned int, STileNodeLoadCost>>* m_pLoadCostMapPointer;
		std::vector<tbb::concurrent_unordered_map<unsigned int, std::vector<unsigned int>>>* m_pTileNodeAncestorMapPointer;
		std::vector<tbb::concurrent_unordered_map<unsigned int, std::vector<unsigned int>>>* m_pTileNodeBrotherMapPointer;
	};
}
