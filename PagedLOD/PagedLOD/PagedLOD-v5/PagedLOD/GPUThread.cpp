#include "GPUThread.h"
#include <iostream>
#include "Timer.h"

using namespace hivePagedLOD;

CGPUThread::CGPUThread(const std::shared_ptr<CPipelineMemoryBufferManager2GPUThread>& vInputPipelineFromMemoryBufferManager, const std::shared_ptr<CPipelineGPUThread2FrameGenerator>& vOutputPipelineToFrameGenerator, const std::shared_ptr<IGPUBufferInterface>& vGPUInterface)
	:m_pInputPipelineFromMemoryBufferManager(vInputPipelineFromMemoryBufferManager),
	m_pOutputPipelineToFrameGenerator(vOutputPipelineToFrameGenerator),
	m_pGPUInterface(vGPUInterface)
{
	m_pGPUBufferManager = std::make_shared<CGPUBufferManager>();
	_ASSERTE(m_pInputPipelineFromMemoryBufferManager && m_pOutputPipelineToFrameGenerator && m_pGPUInterface);
}

//****************************************************************************
//FUNCTION:
void CGPUThread::doFirstFrame(unsigned int vWindowWidth, unsigned int vWindowHeight, unsigned int vMultiFrameCount)
{
	const std::shared_ptr<SGPUTask> initWindowTask = std::make_shared<hivePagedLOD::SGPUTaskInitWindow>(vWindowWidth, vWindowHeight);
	m_pInputPipelineFromMemoryBufferManager->tryPush(initWindowTask);
	const std::shared_ptr<SGPUTask> RenderNothingTask = std::make_shared<hivePagedLOD::SGPUTaskRender>(std::make_shared<std::vector<unsigned int>>());
	for (unsigned int i = 0; i < vMultiFrameCount; ++i)
		m_pInputPipelineFromMemoryBufferManager->tryPush(RenderNothingTask);
}

//****************************************************************************
//FUNCTION:
void CGPUThread::clearGPUBuffers()
{
	for (auto& GeoBuffer : m_pGPUBufferManager->getGeoBufferMap())
		m_pInputPipelineFromMemoryBufferManager->tryPush(std::make_shared<SGPUTaskDelGeoBuffer>(GeoBuffer.first));
	for (auto& TexBuffer : m_pGPUBufferManager->getTexBufferMap())
		m_pInputPipelineFromMemoryBufferManager->tryPush(std::make_shared<SGPUTaskDelTexBuffer>(TexBuffer.first));
}

//****************************************************************************
//FUNCTION:
void CGPUThread::start()
{
	m_Thread = std::thread([&] {__processTaskQueue(); });
}

//****************************************************************************
//FUNCTION:
void CGPUThread::__processTaskQueue()
{
	while (!m_Close)
	{
		std::shared_ptr<SGPUTask> Task;
		if (m_pInputPipelineFromMemoryBufferManager->tryPop(1, Task))
		{
			CTimer::getInstance()->tick(__FUNCTION__);

			switch (Task->TaskType)
			{
			case EGPUTaskType::INIT_WINDOW:
				__init(Task);
				break;
			case EGPUTaskType::GEN_GEOMETRY_BUFFER:
				__generateGeometryBuffer(Task);
				break;
			case EGPUTaskType::GEN_TEXTURE_BUFFER:
				__generateTextureBuffer(Task);
				break;
			case EGPUTaskType::DEL_GEOMETRY_BUFFER:
				__deleteGeometryBuffer(Task);
				break;
			case EGPUTaskType::DEL_TEXTURE_BUFFER:
				__deleteTextureBuffer(Task);
				break;
			case EGPUTaskType::MAIN_LOOP:
				__mainLoop(Task);
				break;
			case EGPUTaskType::END:
				m_pGPUInterface->endV();
				break;
			case EGPUTaskType::UNDEFINED:
				break;
			default:
				break;
			}
			CTimer::getInstance()->tock(__FUNCTION__);
			if (CTimer::getInstance()->needOutput() && CTimer::getInstance()->isRegistered(__FUNCTION__))
				std::cout << __FUNCTION__ << CTimer::getInstance()->getCostTimeByName(__FUNCTION__) << std::endl;
		}
	}
}

//****************************************************************************
//FUNCTION:
void CGPUThread::__generateGeometryBuffer(const std::shared_ptr<SGPUTask>& vTask) const
{
	_ASSERTE(vTask->TaskType == EGPUTaskType::GEN_GEOMETRY_BUFFER);
	const auto pTask = dynamic_cast<SGPUTaskGenGeoBuffer*>(vTask.get());
	_ASSERTE(pTask);

	auto it = m_pGPUBufferManager->getGeoBufferMap().find(pTask->GeoUID);
	if (it != m_pGPUBufferManager->getGeoBufferMap().end())
		return;

	m_pGPUBufferManager->saveGeometryBuffer(pTask->GeoUID, m_pGPUInterface->generateGeometryBufferForOSGV(pTask->Geometry));
}

//****************************************************************************
//FUNCTION:
void CGPUThread::__generateTextureBuffer(const std::shared_ptr<SGPUTask>& vTask) const
{
	_ASSERTE(vTask->TaskType == EGPUTaskType::GEN_TEXTURE_BUFFER);
	const auto pTask = dynamic_cast<SGPUTaskGenTexBuffer*>(vTask.get());
	_ASSERTE(pTask);

	auto it = m_pGPUBufferManager->getTexBufferMap().find(pTask->TexUID);
	if (it != m_pGPUBufferManager->getTexBufferMap().end())
		return;

	m_pGPUBufferManager->saveTextureBuffer(pTask->TexUID, m_pGPUInterface->generateTextureBufferForOSGV(pTask->Texture));
}

//****************************************************************************
void CGPUThread::__deleteGeometryBuffer(const std::shared_ptr<SGPUTask>& vTask) const
{
	_ASSERTE(vTask->TaskType == EGPUTaskType::DEL_GEOMETRY_BUFFER);
	const auto pTask = dynamic_cast<SGPUTaskDelGeoBuffer*>(vTask.get());
	_ASSERTE(pTask);

	auto GeoHandle = m_pGPUBufferManager->deleteGeometryBuffer(pTask->GeoUID);
	if (GeoHandle.VAO == 0)
		return;

	m_pGPUInterface->destroyGeometryBufferForOSGV(GeoHandle);
}

//****************************************************************************
//FUNCTION:
void CGPUThread::__deleteTextureBuffer(const std::shared_ptr<SGPUTask>& vTask) const
{
	_ASSERTE(vTask->TaskType == EGPUTaskType::DEL_TEXTURE_BUFFER);
	const auto pTask = dynamic_cast<SGPUTaskDelTexBuffer*>(vTask.get());
	_ASSERTE(pTask);

	auto TexID = m_pGPUBufferManager->deleteTextureBuffer(pTask->TexUID);
	if (TexID == 0)
		return;

	m_pGPUInterface->destroyTextureBufferForOSGV(TexID);
}

//****************************************************************************
//FUNCTION:
void CGPUThread::__mainLoop(const std::shared_ptr<SGPUTask>& vTask)
{
	CTimer::getInstance()->tick(__FUNCTION__);

	_ASSERTE(vTask->TaskType == EGPUTaskType::MAIN_LOOP);
	const auto pTask = dynamic_cast<SGPUTaskRender*>(vTask.get());
	m_pGPUInterface->prepareGUIV();
	
	auto FrameState = m_pGPUInterface->updateFrameStateV();
	m_pOutputPipelineToFrameGenerator->tryPush(FrameState);
	if (FrameState->EndFlag)
	{
		m_pGPUInterface->endV();
		while (m_pInputPipelineFromMemoryBufferManager->getNumDataInOutputBufferV() > 0)
		{
			std::shared_ptr<SGPUTask> Task;
			m_pInputPipelineFromMemoryBufferManager->tryPop(Task);
		}
		return;
	}
	m_pGPUInterface->renderV(m_pGPUBufferManager->generateRenderMeshBufferSet(*pTask->MeshBufferUIDSet));

	CTimer::getInstance()->tock(__FUNCTION__);
	if (CTimer::getInstance()->needOutput() && CTimer::getInstance()->isRegistered(__FUNCTION__))
		std::cout << __FUNCTION__ << CTimer::getInstance()->getCostTimeByName(__FUNCTION__) << std::endl;
}

//****************************************************************************
//FUNCTION:
void CGPUThread::__init(const std::shared_ptr<SGPUTask>& vTask) const
{
	_ASSERTE(vTask->TaskType == EGPUTaskType::INIT_WINDOW);

	auto pTask = dynamic_cast<SGPUTaskInitWindow*>(vTask.get());
	m_pGPUInterface->initV(pTask->Width, pTask->Height);
}