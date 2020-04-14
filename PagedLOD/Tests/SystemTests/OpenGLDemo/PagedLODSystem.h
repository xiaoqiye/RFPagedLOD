#pragma once
#include "../../../PagedLOD/PagedLOD/PipelineFromGPUThread.h"
#include "../../../PagedLOD/PagedLOD/PipelineMemoryBufferManager2GPUThread.h"
#include "../../../PagedLOD/PagedLOD/PipelineMemoryBufferManager2TileNodeLoader.h"
#include "../../../PagedLOD/PagedLOD/PipelinePreferred2RenderingTileNodeGenerator.h"
#include "../../../PagedLOD/PagedLOD/PipelineRenderingTileNodeGenerator2MemoryBufferManager.h"
#include "../../../PagedLOD/PagedLOD/PipelineTileNodeLoader2MemoryBufferManager.h"
#include "../../../PagedLOD/PagedLOD/PipelineToPreferredTileNodeGenerator.h"

#include "../../../PagedLOD/PagedLOD/GPUThread.h"
#include "../../../PagedLOD/PagedLOD/TileNodeLoader.h"
#include "../../../PagedLOD/PagedLOD/MemoryBufferManager.h"
#include "../../../PagedLOD/PagedLOD/RenderingTileNodeGenerator.h"
#include "../../../PagedLOD/PagedLOD/PreferredTileNodeGenerator.h"

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
		std::shared_ptr<IGPUBufferInterface> GPUInterface;
	};

	class CPagedLODSystem : public hiveOO::CSingleton<CPagedLODSystem>
	{
	public:
		~CPagedLODSystem() = default;
		const std::shared_ptr<CPreferredTileNodeGenerator>& getPreferredTileNodeGenerator() const { return m_pPreferredTileNodeGeneratorThread; }
		const std::shared_ptr<CRenderingTileNodeGenerator>& getRenderingTileNodeGenerator() const { return m_pRenderingTileNodeGeneratorThread; }
		const std::shared_ptr<CTileNodeLoader>& getTileNodeLoader() const { return m_pTileNodeLoaderThread; }
		const std::shared_ptr<CGPUThread>& getGPUThread() const { return m_pGPUThread; }
		const std::shared_ptr<CMemoryBufferManager>& getMemoryBufferManager() const { return m_pMemoryBufferManagerThread; }

		void init(const SInitValue& vInitValue);
		void run();

		void printLog() const;

	private:
		CPagedLODSystem() = default;
		//pipeline
		std::shared_ptr<CPipelineFromGPUThread>										m_pPipelineFromGPUThread;
		std::shared_ptr<CPipelineMemoryBufferManager2GPUThread>						m_pPipelineM2G;
		std::shared_ptr<CPipelineMemoryBufferManager2TileNodeLoader>				m_pPipelineMemoryBufferManager2TileNodeLoader;
		std::shared_ptr<CPipelinePreferred2RenderingTileNodeGenerator>				m_pPipelineP2R;
		std::shared_ptr<CPipelineRenderingTileNodeGenerator2MemoryBufferManager>	m_pPipelineRenderingTileNodeGenerator2MemoryBufferManager;
		std::shared_ptr<CPipelineTileNodeLoader2MemoryBufferManager>				m_pPipelineL2M;
		std::shared_ptr<CPipelineToPreferredTileNodeGenerator>						m_pPipelineToPreferredTileNodeGenerator;
		//kernal
		std::shared_ptr<CGPUThread>						m_pGPUThread;
		std::shared_ptr<CTileNodeLoader>				m_pTileNodeLoaderThread;
		std::shared_ptr<CPreferredTileNodeGenerator>	m_pPreferredTileNodeGeneratorThread;
		std::shared_ptr<CRenderingTileNodeGenerator>	m_pRenderingTileNodeGeneratorThread;
		std::shared_ptr<CMemoryBufferManager>			m_pMemoryBufferManagerThread;

		void __initScene(const std::string& vSerializedPath, const std::string& vBinPath, bool vLegalTileName, unsigned int vBeginTileNumber, unsigned int vEndTileNumber);
		void __initPipeline();
		void __initKernal(const std::shared_ptr<IGPUBufferInterface>& vGPUInterface);
		void __configureKernal();
		void __updateFrameState(const std::shared_ptr<IFrameState>& vFrameState);
		void __finalize();
		void __createFrameState();
		void __createRenderData();
		std::shared_ptr<IFrameState> m_pFrameState;
		std::uintmax_t m_FrameID = 0;

		std::thread m_Thread0;
		std::thread m_Thread1;
		
		friend class hiveOO::CSingleton<CPagedLODSystem>;
	};
}