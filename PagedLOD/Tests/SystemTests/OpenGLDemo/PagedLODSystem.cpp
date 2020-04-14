#include "PagedLODSystem.h"
#include "MyUISystem.h"
#include <iostream>
#include "Utils.h"
#include <chrono>
#include <thread>
#include "../../../PagedLOD/PagedLOD/Timer.h"

using namespace hivePagedLOD;

std::shared_ptr<SMemoryMeshBuffer> G_MESHBUFFER;

//****************************************************************************
//FUNCTION:
//vLegalTileName: tile name format like 'Tile_0' ('Tile_' + number) 
void CPagedLODSystem::init(const SInitValue& vInitValue)
{
	__initScene(vInitValue.SerializedPath, vInitValue.BinPath, vInitValue.LegalTileName, vInitValue.BeginTileNumber, vInitValue.EndTileNumber);
	__initPipeline();
	__initKernal(vInitValue.GPUInterface);
	__configureKernal();
}

void CPagedLODSystem::__createFrameState()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(100 * 1000 * 1000));
		auto xy = m_pGPUThread->getWindowPositionTopLeft();
		float x = xy.first;
		float y = xy.second;
		auto ViewInfo = m_pGPUThread->fetchGPUInterface()->handleCameraV(x,y);
		m_pFrameState = std::make_shared<IFrameState>();
		m_pFrameState->ViewInfo = ViewInfo;
		m_pPipelineFromGPUThread->tryPush(m_pFrameState);

		//std::cout << "push view info" << std::endl;
	}
}

void CPagedLODSystem::__createRenderData()
{
	while (true)
	{
		while (m_pPipelineFromGPUThread->tryPop(m_pFrameState))
		{
			//std::cout << "pop frame state" << std::endl;
			//std::this_thread::sleep_for(std::chrono::milliseconds(1));
			std::vector<std::string> v;
			v.emplace_back("I:\\Resource\\CD\\Bin\\Tile_1\\Tile_1_0.bin");
			std::shared_ptr<std::vector<std::string>> MeshBufferSet = std::make_shared<std::vector<std::string>>(v);
			std::shared_ptr<SGPUTask> RenderTask = std::make_shared<hivePagedLOD::SGPUTaskRender>(MeshBufferSet, EMainLoopMeshBufferType::NORMAL);
			m_pGPUThread->pushTaskIntoInputPipeline(RenderTask);
		}
	}
}
void CPagedLODSystem::run()
{
	m_pGPUThread->start();
	m_pTileNodeLoaderThread->start();
	std::vector<std::pair<std::string, std::shared_ptr<STexture>>> LoadSet;
	LoadSet.emplace_back("I:\\Resource\\CD\\Bin\\Tile_1\\Tile_1_0.bin", std::make_shared<STexture>());
	SLoadTask LoadTask(ELoadTaskPriority::HIGH, LoadSet);
	m_pPipelineMemoryBufferManager2TileNodeLoader->tryPush(LoadTask);
	std::this_thread::sleep_for(std::chrono::seconds(5));
	m_pMemoryBufferManagerThread->processLoadedTileNodeIfExist();
	m_pMemoryBufferManagerThread->__generateMeshBufferToShow();
	m_Thread0 = std::thread([&] {__createFrameState(); });
	m_Thread1 = std::thread([&] {__createRenderData(); });
	while (true)
		std::this_thread::sleep_for(std::chrono::microseconds(100));
}

////****************************************************************************
////FUNCTION:
//void CPagedLODSystem::run()
//{
//	m_pGPUThread->start();
//	m_pTileNodeLoaderThread->start();
//
//	m_pFrameState = std::make_shared<IFrameState>();
//
//	while (true)
//	{
//		while (m_pPipelineFromGPUThread->tryPop(m_pFrameState))
//		{
//			if (!m_pFrameState->EndFlag)
//			{
//				++m_FrameID;
//				//std::cout << m_FrameID << std::endl;
//
//				CTimer::getInstance()->tick(__FUNCTION__);
//
//				__updateFrameState(m_pFrameState);
//
//				m_pPipelineToPreferredTileNodeGenerator->tryPush(m_pFrameState->ViewInfo);
//
//				m_pPreferredTileNodeGenerator->work();
//
//				m_pRenderingTileNodeGenerator->work(m_pMemoryBufferManager->getLoadCostMap());
//
//				m_pMemoryBufferManager->processLoadedTileNodeIfExist();
//				auto HasLowPriorityTask = m_pMemoryBufferManager->generateLowPriorityLoadTask();
//				if (HasLowPriorityTask)
//				{
//					m_pTileNodeLoaderThread->waitForLowPriorityTaskFinsih();
//					m_pMemoryBufferManager->processLoadedTileNodeIfExist();
//					m_pTileNodeLoaderThread->setHighPriorityTaskNum(0);
//					auto HasHighPriorityTask = m_pMemoryBufferManager->generateHighPriorityLoadTask();
//					if (HasHighPriorityTask.first)
//					{
//						m_pTileNodeLoaderThread->setHighPriorityTaskNum(HasHighPriorityTask.second);
//						m_pTileNodeLoaderThread->waitHighPriorityTaskFinsih();
//						m_pMemoryBufferManager->processLoadedTileNodeIfExist();
//					}
//				}
//
//				m_pMemoryBufferManager->generateMeshBufferToShow();
//
//				m_pMemoryBufferManager->manageMemory();
//
//				if (m_FrameID % CLEAR_RECENT_LOAD_SET_FRAME_GAP == 0)
//					m_pTileNodeLoaderThread->resetRencentLoadSet();
//		
//				CTimer::getInstance()->tock(__FUNCTION__);
//				if (CTimer::getInstance()->on())
//					std::cout << __FUNCTION__ <<" cost time(us): " << CTimer::getInstance()->getCostTimeByName(__FUNCTION__) << std::endl;
//			}
//			else
//			{
//				std::shared_ptr<SGPUTask> EndTask;
//				EndTask = std::make_shared<SGPUTaskEnd>();
//				m_pPipelineMemoryBufferManager2GPUThread->tryPush(EndTask);
//			}
//		}
//		if (m_pFrameState->EndFlag)
//		{
//			break;
//		}
//	}
//	__finalize();
//}

//****************************************************************************
//FUNCTION:
void CPagedLODSystem::printLog() const
{
	for (auto& Log : m_pTileNodeLoaderThread->getLogSet())
	{
		std::cout << Log.toString() << std::endl;
	}
}

//****************************************************************************
//FUNCTION:
void CPagedLODSystem::__initScene(const std::string& vSerializedPath, const std::string& vBinPath, bool vLegalTileName, unsigned int vBeginTileNumber, unsigned int vEndTileNumber)
{
	CScene::getInstance()->init(vSerializedPath, vBinPath, vLegalTileName, vBeginTileNumber, vEndTileNumber);
}

//****************************************************************************
//FUNCTION:
void CPagedLODSystem::__initPipeline()
{
	m_pPipelineFromGPUThread = std::make_shared<CPipelineFromGPUThread>();
	m_pPipelineFromGPUThread->initInputBuffer();
	m_pPipelineFromGPUThread->initOutputBuffer();

	m_pPipelineM2G = std::make_shared<CPipelineMemoryBufferManager2GPUThread>();
	m_pPipelineM2G->initInputBuffer();
	m_pPipelineM2G->initOutputBuffer();

	m_pPipelineMemoryBufferManager2TileNodeLoader = std::make_shared<CPipelineMemoryBufferManager2TileNodeLoader>();
	m_pPipelineMemoryBufferManager2TileNodeLoader->initInputBuffer();
	m_pPipelineMemoryBufferManager2TileNodeLoader->initOutputBuffer();

	m_pPipelineP2R = std::make_shared<CPipelinePreferred2RenderingTileNodeGenerator>();
	m_pPipelineP2R->initInputBuffer();
	m_pPipelineP2R->initOutputBuffer();

	m_pPipelineRenderingTileNodeGenerator2MemoryBufferManager = std::make_shared<CPipelineRenderingTileNodeGenerator2MemoryBufferManager>();
	m_pPipelineRenderingTileNodeGenerator2MemoryBufferManager->initInputBuffer();
	m_pPipelineRenderingTileNodeGenerator2MemoryBufferManager->initOutputBuffer();

	m_pPipelineL2M = std::make_shared<CPipelineTileNodeLoader2MemoryBufferManager>();
	m_pPipelineL2M->initInputBuffer();
	m_pPipelineL2M->initOutputBuffer();
	
	m_pPipelineToPreferredTileNodeGenerator = std::make_shared<CPipelineToPreferredTileNodeGenerator>();
	m_pPipelineToPreferredTileNodeGenerator->initInputBuffer();
	m_pPipelineToPreferredTileNodeGenerator->initOutputBuffer();
}

//****************************************************************************
//FUNCTION:
void CPagedLODSystem::__initKernal(const std::shared_ptr<IGPUBufferInterface>& vGPUInterface)
{
	m_pGPUThread					  =std::make_shared<CGPUThread>(m_pPipelineM2G, m_pPipelineFromGPUThread, vGPUInterface);
	m_pTileNodeLoaderThread			  =std::make_shared<CTileNodeLoader>(m_pPipelineMemoryBufferManager2TileNodeLoader, m_pPipelineL2M);
	m_pPreferredTileNodeGeneratorThread	  =std::make_shared<CPreferredTileNodeGenerator>(m_pPipelineToPreferredTileNodeGenerator, m_pPipelineP2R);
	m_pRenderingTileNodeGeneratorThread	  =std::make_shared<CRenderingTileNodeGenerator>(m_pPipelineP2R, m_pPipelineRenderingTileNodeGenerator2MemoryBufferManager);
	m_pMemoryBufferManagerThread			  =std::make_shared<CMemoryBufferManager>(m_pPipelineRenderingTileNodeGenerator2MemoryBufferManager, m_pPipelineL2M, m_pPipelineMemoryBufferManager2TileNodeLoader, m_pPipelineM2G);
}

//****************************************************************************
//FUNCTION:
void CPagedLODSystem::__configureKernal()
{
	m_pTileNodeLoaderThread->useRecord(false);
	m_pTileNodeLoaderThread->setMaxLogNum(10000);

	m_pMemoryBufferManagerThread->resetLoadCostMap();
	m_pMemoryBufferManagerThread->useLRUList(true);
}

//****************************************************************************
//FUNCTION:
void CPagedLODSystem::__updateFrameState(const std::shared_ptr<IFrameState>& vFrameState)
{
	const auto pFrameState = dynamic_cast<const SFrameState*>(vFrameState.get());
	if (pFrameState->ClearBuffer)
	{
		m_pMemoryBufferManagerThread->clearMemoryBuffers();
		m_pGPUThread->clearGPUBuffers();
		std::this_thread::sleep_for(std::chrono::seconds(1));
		m_pGPUThread->fetchGPUBufferManager()->clearGPUBufferMap();
	}

	m_pMemoryBufferManagerThread->setMemoryLimit(pFrameState->BufferLimitSizeMB * MEGABYTE);
	m_pRenderingTileNodeGeneratorThread->setLoadStrategy(pFrameState->LoadStrategy);
}

//****************************************************************************
//FUNCTION:
void CPagedLODSystem::__finalize()
{
	m_pTileNodeLoaderThread->setFinishSignal(true);
	m_pTileNodeLoaderThread->join();
	m_pGPUThread->join();
}