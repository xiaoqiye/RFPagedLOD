#pragma once
#include "Common.h"
#include "PipelineTileNodeLoader2MemoryBufferManager.h"
#include <thread>
#include <vector>
#include <mutex>
#include <set>
#include "PipelineRenderingTileNodeGenerator2TileNodeLoader.h"

namespace hivePagedLOD
{
	class CTileNodeLoader
	{
	public:
		CTileNodeLoader() = delete;
		CTileNodeLoader(const std::shared_ptr<CPipelineRenderingTileNodeGenerator2TileNodeLoader>& vPipelineInputFromMemoryRendering, const std::shared_ptr<CPipelineTileNodeLoader2MemoryBufferManager>& vPipelineOutputToMemoryBufferManager);
		~CTileNodeLoader() = default;

		void start();
		void join() { _ASSERTE(m_Thread.joinable()); m_Thread.join(); }
		void setFinishSignal(bool vFinish) { m_Close = vFinish; }
		void doFirstFrame();

		bool hasLoadTask() const { return m_pPipelineInputFromRendering->getNumDataInOutputBufferV() > 0; }
		
		void useRecord(bool vUseLoadLog) { m_UseLoadLog = vUseLoadLog; }
		void setMaxLogNum(unsigned int vNum) { m_MaxLogNum = vNum; }
		unsigned int getLoadedNumThisFrame() const { return m_LoadedNumThisFrame; }
		std::uintmax_t getLoadedSizeThisFrame() const { return m_LoadedSizeThisFrame; }
		std::vector<STileLoadLog>& getLogSet() { return m_LoadLogSet; }
		const std::vector<std::vector<double>>& getAllKnapsackLoadInfo() { _ASSERT(m_AllKnapsackLoadInfo.size() > 0); return m_AllKnapsackLoadInfo; }

		void resetRecentLoadSet() { m_RecentLoadGeometryNameSet.clear(); }

	private:
		std::shared_ptr<CPipelineRenderingTileNodeGenerator2TileNodeLoader> m_pPipelineInputFromRendering;
		std::shared_ptr<CPipelineTileNodeLoader2MemoryBufferManager> m_pPipelineOutputToMemoryBufferManager;

		void __workByRenderingTileNodeSet();
		void __processLoadTask(const std::shared_ptr<SRenderingGeneratorResult>& vRenderingGeneratorResult);
		void __LoadFromFile(const std::shared_ptr<SLoadTask>& vLoadTask, std::vector<std::shared_ptr<SMemoryMeshBuffer>>& voMeshBufferSet, std::vector<unsigned int>& voTileNodeUIDSet);


		std::shared_ptr<char> __getFileStream(const std::string& vFilePath);
		std::shared_ptr<SGeometry> __loadGeometryBufferFromFile(const std::string& vFilePath);
		std::shared_ptr<STexture> __loadTextureBufferFromFile(const std::string& vFilePath);
		void __processTextureBuffer(const void* vTexBufferStream, std::shared_ptr<STexture>& voTexture) const;
		void __processGeometryBuffer(const void* vGeoBufferStream, std::shared_ptr<SGeometry>& voGeometry) const;

		std::set<std::string> m_ThisLoadTaskTextureNameSet;
		int m_ClearLoadSetLimit = 100;
		int m_LoadFunctionCallTimes = 0;
		std::set<std::string> m_RecentLoadGeometryNameSet;
		
		std::thread m_Thread;
		bool m_Close = false;

		unsigned int m_LoadedNumThisFrame = 0;
		std::mutex m_LoadedNumThisFrameMutex;

		std::uintmax_t m_LoadedSizeThisFrame = 0;
		std::mutex m_LoadedSizeThisFrameMutex;

		bool m_UseLoadLog = false;
		unsigned int m_MaxLogNum = 10000;
		std::vector<STileLoadLog> m_LoadLogSet;

		const std::string m_LogOutputPath = "0424_MutiFrameForKnapsack.csv";
		unsigned int  m_LoadedNumThisK = 0;
		std::uintmax_t m_LoadedSizeThisK = 0;
		double m_LoadedTimeThisK = 0;
		std::vector<std::vector<double>> m_AllKnapsackLoadInfo;
	};
}
