#pragma once
#include "../../../PagedLOD/PagedLOD/PipelineMemoryBufferManager2GPUThread.h"
#include "../../../PagedLOD/PagedLOD/PipelinePreferred2RenderingTileNodeGenerator.h"
#include "../../../PagedLOD/PagedLOD/PipelineTileNodeLoader2MemoryBufferManager.h"

#include "../../../PagedLOD/PagedLOD/GPUThread.h"
#include "../../../PagedLOD/PagedLOD/TileNodeLoader.h"
#include "../../../PagedLOD/PagedLOD/MemoryBufferManager.h"
#include "../../../PagedLOD/PagedLOD/RenderingTileNodeGenerator.h"
#include "../../../PagedLOD/PagedLOD/PreferredTileNodeGenerator.h"
#include "../../../PagedLOD/PagedLOD/FrameGenerator.h"

namespace hivePagedLOD
{
	struct SInitValue
	{
		unsigned int WindowWidth = 1080;
		unsigned int WindowHeight = 720;
		std::string SerializedPath;
		std::string BinPath;
		bool LegalTileName = false;
		unsigned int BeginTileNumber = 1;
		unsigned int EndTileNumber = 0;
		float CameraPositionX = 0.0f;
		float CameraPositionY = 0.0f;
		float CameraPositionZ = 0.0f;
		float CameraSpeed = 0.0f;
		unsigned int BufferLimitSize = 0;
		unsigned int MultiFrameCount = 1;
		std::string LoadStrategy = LOAD_STRATEGY_UNDEFINED;
		unsigned int LoadLimitPerFrame = 0;
		unsigned int LoadParentMaxLevel = 0;
		bool SaveCapture = false;
		unsigned int FinishLoadFrameID = 0;
		bool OutputRenderingTileNodeGeneratorInfo = false;
		std::shared_ptr<IGPUBufferInterface> GPUInterface;
		int TraverseMaxDeep = -1;
	};
	
	class CPagedLODSystem : public hiveOO::CSingleton<CPagedLODSystem>
	{
	public:
		~CPagedLODSystem() = default;
		const std::shared_ptr<CFrameGenerator>& getFrameGenerator() const { return m_pFrameGeneratorThread; }
		const std::shared_ptr<CPreferredTileNodeGenerator>& getPreferredTileNodeGenerator() const { return m_pPreferredTileNodeGeneratorThread; }
		const std::shared_ptr<CRenderingTileNodeGenerator>& getRenderingTileNodeGenerator() const { return m_pRenderingTileNodeGeneratorThread; }
		const std::shared_ptr<CTileNodeLoader>& getTileNodeLoader() const { return m_pTileNodeLoaderThread; }
		const std::shared_ptr<CMemoryBufferManager>& getMemoryBufferManager() const { return m_pMemoryBufferManagerThread; }
		const std::shared_ptr<CGPUThread>& getGPUThread() const { return m_pGPUThread; }

		void init(const SInitValue& vInitValue);
		void run();

		bool hasFrameState();
		bool hasLoadTask();
		
		void printLog() const;

	private:
		CPagedLODSystem() = default;
		//pipeline
		std::shared_ptr<CPipelineFrameGenerator2PreferredTileNodeGenerator>			m_pPipelineF2P;
		std::shared_ptr<CPipelinePreferred2RenderingTileNodeGenerator>				m_pPipelineP2R;
		std::shared_ptr<CPipelineRenderingTileNodeGenerator2TileNodeLoader>			m_pPipelineR2L;
		std::shared_ptr<CPipelineTileNodeLoader2MemoryBufferManager>				m_pPipelineL2M;
		std::shared_ptr<CPipelineMemoryBufferManager2GPUThread>						m_pPipelineM2G;
		std::shared_ptr<CPipelineGPUThread2FrameGenerator>							m_pPipelineG2F;
		
		//kernal
		std::shared_ptr<CFrameGenerator>			    m_pFrameGeneratorThread;
		std::shared_ptr<CPreferredTileNodeGenerator>	m_pPreferredTileNodeGeneratorThread;
		std::shared_ptr<CRenderingTileNodeGenerator>	m_pRenderingTileNodeGeneratorThread;
		std::shared_ptr<CTileNodeLoader>				m_pTileNodeLoaderThread;
		std::shared_ptr<CMemoryBufferManager>			m_pMemoryBufferManagerThread;
		std::shared_ptr<CGPUThread>						m_pGPUThread;

		std::mutex m_FrameStateMutex;
		std::condition_variable m_FrameStateSignal;
		
		void __initScene(const SInitValue& vInitValue);
		void __initPipeline();
		void __initKernal(const std::shared_ptr<IGPUBufferInterface>& vGPUInterface);
		void __configureKernal(const SInitValue& vInitValue);
		void __initOthers(const SInitValue& vInitValue);
		void __updateFrameState(const std::shared_ptr<IFrameState>& vFrameState);
		void __finalize();
		
		std::uintmax_t m_FrameID = 0;
		std::shared_ptr<IFrameState> m_FrameState;
		
		int m_SleepTimeUS = 100;
		
		friend class hiveOO::CSingleton<CPagedLODSystem>;
	};
}
