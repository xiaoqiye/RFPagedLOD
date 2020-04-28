#include "FrameGenerator.h"
#include "Timer.h"
#include "Utils.h"
#include <iostream>

using namespace hivePagedLOD;

CFrameGenerator::CFrameGenerator(const std::shared_ptr<CPipelineGPUThread2FrameGenerator>& vInputPipelineFromGPUThread, const std::shared_ptr<CPipelineFrameGenerator2PreferredTileNodeGenerator>& vOutputPipelineToPreferred)
	:m_pInputPipelineFromGPUThread(vInputPipelineFromGPUThread),
	m_pOutputPipelineToPreferred(vOutputPipelineToPreferred)
{
	_ASSERTE(m_pInputPipelineFromGPUThread && m_pOutputPipelineToPreferred);
}

//****************************************************************************
//FUNCTION:
void CFrameGenerator::start()
{
	m_Thread = std::thread([&] {__workByFrameState(); });
}

//****************************************************************************
//FUNCTION:
void CFrameGenerator::__workByFrameState()
{
	while (!m_Close)
	{
		if (m_pInputPipelineFromGPUThread->tryPop(1, m_FrameState))
		{
			++m_FrameID;
			CTimer::getInstance()->tick(__FUNCTION__);

			if (m_FrameState->EndFlag)
			{
				m_Close = true;
				break;
			}
			m_pOutputPipelineToPreferred->tryPush(m_FrameState->ViewInfo);
			m_HasFrameState = true;

			//CUtils::getInstance()->printCameraInfo(m_FrameState->ViewInfo.CameraInfo);

			CTimer::getInstance()->tock(__FUNCTION__);
			if (CTimer::getInstance()->needOutput() && CTimer::getInstance()->isRegistered(__FUNCTION__))
				std::cout << __FUNCTION__ << CTimer::getInstance()->getCostTimeByName(__FUNCTION__) << std::endl;
		}
		else
		{
			m_HasFrameState = false;
		}
	}
}

//****************************************************************************
//FUNCTION:
bool CFrameGenerator::hasFrameState(std::shared_ptr<IFrameState>& voFrameState)
{
	if (m_HasFrameState)
	{
		voFrameState = m_FrameState;
		m_HasFrameState = false;
		return true;
	}
	else
	{
		return false;
	}
}
