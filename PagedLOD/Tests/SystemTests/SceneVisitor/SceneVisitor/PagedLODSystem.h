#pragma once
#include "PipelineMemoryBufferManager2GPUThread.h"
#include "PipelinePreferred2RenderingTileNodeGenerator.h"
#include "PipelineTileNodeLoader2MemoryBufferManager.h"

#include "GPUThread.h"
#include "TileNodeLoader.h"
#include "MemoryBufferManager.h"
#include "RenderingTileNodeGenerator.h"
#include "PreferredTileNodeGenerator.h"
#include "FrameGenerator.h"

namespace hivePagedLOD
{
	enum class EVisitStrategy
	{
		GO_STRAIGHT,
		RANDOM,
		BY_RECORD,
		NO_STRATEGY,
		UNDEFINED
	};
	struct SCameraInitValue
	{
		float Speed = 1.0;
		float Far = 5000.0;
		float Near = static_cast<float>(0.1);
		float PositionX = 80.0;
		float PositionY = 480.0;
		float PositionZ = -20.0;
	};
	struct SVisitInitValue {
		float StopPositionZ = -20.0;
		float Speed = static_cast<float>(0.3);
		unsigned int EndFrameID = 100000000;
		unsigned int ChangeDirectionFrameID = 10;
		unsigned int ResetCameraPositionFrameID = 100000;
		unsigned int Seed = 1230355;
		unsigned int AreaWidth = 80;
		unsigned int AreaHeight = 180;
		std::string LogPath;
		std::string VisitFilePath;
		std::string SaveVisitFilePath;
		bool SaveRecordSignal = false;
		unsigned int VisitStrategy = 4;
		unsigned int WaitFrameNum = 2;
	};
	struct SInitValue
	{
		unsigned int WindowWidth = 1080;
		unsigned int WindowHeight = 720;
		std::string SerializedPath;
		std::string BinPath;
		std::string BaseNPath;
		unsigned int LoadTileCount = 1;
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
		unsigned int LoadLimitPerSolve = 0;
		unsigned int LoadParentMaxLevel = 0;
		bool SaveCapture = false;
		unsigned int FinishLoadFrameID = 0;
		bool OutputRenderingTileNodeGeneratorInfo = false;
		std::shared_ptr<IGPUBufferInterface> GPUInterface;
		int TraverseMaxDeep = -1;
		SCameraInitValue CameraInit;
		SVisitInitValue VisitInit;
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
		template<class Type>
		void writeInfo2File(const std::vector<std::vector<Type>>& vInfo, const std::string& vFilePath);

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
