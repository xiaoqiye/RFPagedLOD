#include "PipelineGPUThread2FrameGenerator.h"

using namespace hivePagedLOD;
using TDataType = CPipelineGPUThread2FrameGenerator::TDataType;

//****************************************************************************
//FUNCTION:
TDataType::TOutputData CPipelineGPUThread2FrameGenerator::__convertDataV(const TDataType::TInputData& vInput)
{
	return vInput;
}

//****************************************************************************
//FUNCTION:
TDataType::TInputData CPipelineGPUThread2FrameGenerator::__getFirstDataInInputBufferV()
{
	_ASSERTE(!m_pInputBuffer->empty());
	return *m_pInputBuffer->unsafe_begin();
}

//****************************************************************************
//FUNCTION:
void CPipelineGPUThread2FrameGenerator::__popFirstDataInInputBufferV()
{
	TDataType::TInputData ptr;
	m_pInputBuffer->try_pop(ptr);
}

//****************************************************************************
//FUNCTION:
bool CPipelineGPUThread2FrameGenerator::__popFromInputBufferV(TDataType::TInputData& voData)
{
	return m_pInputBuffer->try_pop(voData);
}

//****************************************************************************
//FUNCTION:
bool CPipelineGPUThread2FrameGenerator::__popFromOutputBufferV(TDataType::TOutputData& voData)
{
	return m_pOutputBuffer->try_pop(voData);
}

//****************************************************************************
//FUNCTION:
bool CPipelineGPUThread2FrameGenerator::__push2InputBufferV(const TDataType::TInputData& vData)
{
	m_pInputBuffer->push(vData);
	return true;
}

//****************************************************************************
//FUNCTION:
bool CPipelineGPUThread2FrameGenerator::__push2OutputBufferV(const TDataType::TOutputData& vData)
{
	m_pOutputBuffer->push(vData);
	return true;
}