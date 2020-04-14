#include "PagedLODSystem.h"
#include "MyUISystem.h"
#include <iostream>
#include "Utils.h"
#include <chrono>
#include <thread>
#include "../../../PagedLOD/PagedLOD/Timer.h"

#include "OGLGPUOperator.h"

using namespace hivePagedLOD;


//****************************************************************************
//FUNCTION:
//vLegalTileName: tile name format like 'Tile_0' ('Tile_' + number) 
void CPagedLODSystem::init(const SInitValue& vInitValue)
{
	__initScene(vInitValue);
	__initPipeline();
	__initKernal(vInitValue.GPUInterface);
	__configureKernal(vInitValue);
	__initOthers(vInitValue);
}

//****************************************************************************
//FUNCTION:
void CPagedLODSystem::run()
{
	m_pFrameGeneratorThread->start();
	m_pPreferredTileNodeGeneratorThread->start();
	m_pRenderingTileNodeGeneratorThread->start();
	m_pTileNodeLoaderThread->start();
	m_pMemoryBufferManagerThread->start();
	m_pGPUThread->start();
	
	while (!m_pFrameGeneratorThread->getEndFlag())
	{	
		std::this_thread::sleep_for(std::chrono::microseconds(m_SleepTimeUS));
		std::unique_lock<std::mutex> Lock(m_FrameStateMutex);
		m_FrameStateSignal.wait_for(Lock, std::chrono::milliseconds(1), std::bind(&CPagedLODSystem::hasFrameState, this));
		//m_FrameStateSignal.wait_for(Lock, std::chrono::milliseconds(1), std::bind(&CPagedLODSystem::hasLoadTask, this));
	}
	__finalize();
}

//****************************************************************************
//FUNCTION:
bool CPagedLODSystem::hasFrameState()
{
	const bool hasFrameState = m_pFrameGeneratorThread->hasFrameState(m_FrameState);
	if (hasFrameState)
		__updateFrameState(m_FrameState);
	
	return hasFrameState;
}

//****************************************************************************
//FUNCTION:
bool CPagedLODSystem::hasLoadTask()
{
	const bool hasLoadTask = m_pTileNodeLoaderThread->hasLoadTask();
	if (hasLoadTask)
		return false;

	const bool hasFrameState = m_pFrameGeneratorThread->hasFrameState(m_FrameState);
	if (hasFrameState)
		__updateFrameState(m_FrameState);
	return hasLoadTask;
}

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
void CPagedLODSystem::__initScene(const SInitValue& vInitValue)
{
	CScene::getInstance()->init(vInitValue.SerializedPath, vInitValue.BinPath, vInitValue.LegalTileName, vInitValue.BeginTileNumber, vInitValue.EndTileNumber);
}

//****************************************************************************
//FUNCTION:
void CPagedLODSystem::__initPipeline()
{
	m_pPipelineF2P = std::make_shared<CPipelineFrameGenerator2PreferredTileNodeGenerator>();
	m_pPipelineF2P->initInputBuffer();
	m_pPipelineF2P->initOutputBuffer();
	
	m_pPipelineP2R = std::make_shared<CPipelinePreferred2RenderingTileNodeGenerator>();
	m_pPipelineP2R->initInputBuffer();
	m_pPipelineP2R->initOutputBuffer();
	
	m_pPipelineR2L = std::make_shared<CPipelineRenderingTileNodeGenerator2TileNodeLoader>();
	m_pPipelineR2L->initInputBuffer();
	m_pPipelineR2L->initOutputBuffer();
	
	m_pPipelineL2M = std::make_shared<CPipelineTileNodeLoader2MemoryBufferManager>();
	m_pPipelineL2M->initInputBuffer();
	m_pPipelineL2M->initOutputBuffer();

	m_pPipelineM2G = std::make_shared<CPipelineMemoryBufferManager2GPUThread>();
	m_pPipelineM2G->initInputBuffer();
	m_pPipelineM2G->initOutputBuffer();
	
	m_pPipelineG2F = std::make_shared<CPipelineGPUThread2FrameGenerator>();
	m_pPipelineG2F->initInputBuffer();
	m_pPipelineG2F->initOutputBuffer();
}

//****************************************************************************
//FUNCTION:
void CPagedLODSystem::__initKernal(const std::shared_ptr<IGPUBufferInterface>& vGPUInterface)
{
	m_pFrameGeneratorThread					= std::make_shared<CFrameGenerator>(m_pPipelineG2F, m_pPipelineF2P);
	m_pPreferredTileNodeGeneratorThread		= std::make_shared<CPreferredTileNodeGenerator>(m_pPipelineF2P, m_pPipelineP2R);
	m_pRenderingTileNodeGeneratorThread		= std::make_shared<CRenderingTileNodeGenerator>(m_pPipelineP2R, m_pPipelineR2L, nullptr);
	m_pTileNodeLoaderThread					= std::make_shared<CTileNodeLoader>(m_pPipelineR2L, m_pPipelineL2M);
	m_pMemoryBufferManagerThread			= std::make_shared<CMemoryBufferManager>(m_pPipelineL2M, m_pPipelineM2G);
	m_pGPUThread							= std::make_shared<CGPUThread>(m_pPipelineM2G, m_pPipelineG2F, vGPUInterface);
}

//****************************************************************************
//FUNCTION:
void CPagedLODSystem::__configureKernal(const SInitValue& vInitValue)
{
	m_pTileNodeLoaderThread->useRecord(false);
	m_pTileNodeLoaderThread->setMaxLogNum(10000);

	m_pMemoryBufferManagerThread->resetLoadCostMap();
	m_pMemoryBufferManagerThread->useLRUList(true);
	m_pMemoryBufferManagerThread->initMapPointer();
	
	m_pRenderingTileNodeGeneratorThread->setLoadCostMapPointer(m_pMemoryBufferManagerThread->getLoadCostMapPointer());
	m_pRenderingTileNodeGeneratorThread->initStrategyConfig(vInitValue.LoadStrategy, vInitValue.LoadLimitPerFrame, vInitValue.LoadParentMaxLevel);
	m_pRenderingTileNodeGeneratorThread->setFinishLoadFrameID(vInitValue.FinishLoadFrameID);
	m_pRenderingTileNodeGeneratorThread->setOutputInfo(vInitValue.OutputRenderingTileNodeGeneratorInfo);

	m_pGPUThread->fetchGPUBufferManager()->initMapPointer();
}

//****************************************************************************
//FUNCTION:
void CPagedLODSystem::__initOthers(const SInitValue& vInitValue)
{
	vInitValue.GPUInterface->setCameraDefaultPositionV(vInitValue.CameraPositionX, vInitValue.CameraPositionY, vInitValue.CameraPositionZ);
	auto FrameState = std::make_shared<SFrameState>(vInitValue.CameraSpeed, vInitValue.BufferLimitSize, vInitValue.LoadLimitPerFrame, vInitValue.LoadParentMaxLevel, m_pRenderingTileNodeGeneratorThread->getLoadStrategy());
	vInitValue.GPUInterface->initFirstFrameStateV(FrameState);
	vInitValue.GPUInterface->saveCaptureFlagV(vInitValue.SaveCapture);
}

//****************************************************************************
//FUNCTION:
void CPagedLODSystem::__updateFrameState(const std::shared_ptr<IFrameState>& vFrameState)
{
	const auto pFrameState = dynamic_cast<const SFrameState*>(vFrameState.get());
	//if (pFrameState->ClearBuffer)
	//{
	//	m_pMemoryBufferManagerThread->clearMemoryBuffers();
	//	m_pGPUThread->clearGPUBuffers();
	//	std::this_thread::sleep_for(std::chrono::seconds(1));
	//	m_pGPUThread->fetchGPUBufferManager()->clearGPUBufferMap();
	//}

	m_pMemoryBufferManagerThread->setMemoryLimit(pFrameState->BufferLimitSize);
	m_pRenderingTileNodeGeneratorThread->setLoadLimitSizePerFrame(pFrameState->LoadLimitSizePerFrame);
	m_pRenderingTileNodeGeneratorThread->setLoadParentMaxLevel(pFrameState->LoadParentMaxLevel);
	m_pRenderingTileNodeGeneratorThread->setLoadStrategy(pFrameState->LoadStrategy);
}

//****************************************************************************
//FUNCTION:
void CPagedLODSystem::__finalize()
{
	m_pPreferredTileNodeGeneratorThread->setFinishSignal(true);
	m_pRenderingTileNodeGeneratorThread->setFinishSignal(true);
	m_pTileNodeLoaderThread->setFinishSignal(true);
	m_pMemoryBufferManagerThread->setFinishSignal(true);
	m_pGPUThread->setFinishSignal(true);
	
	m_pFrameGeneratorThread->join();
	m_pPreferredTileNodeGeneratorThread->join();
	m_pRenderingTileNodeGeneratorThread->join();
	m_pTileNodeLoaderThread->join();
	m_pMemoryBufferManagerThread->join();
	m_pGPUThread->join();
}