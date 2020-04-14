#include "PipelineMemoryBufferManager2GPUThread.h"

using namespace hivePagedLOD;
using TDataType = CPipelineMemoryBufferManager2GPUThread::TDataType;

//****************************************************************************
//FUNCTION:
TDataType::TOutputData CPipelineMemoryBufferManager2GPUThread::__convertDataV(const TDataType::TInputData& vInput)
{
	return vInput;
}

//****************************************************************************
//FUNCTION:
TDataType::TInputData CPipelineMemoryBufferManager2GPUThread::__getFirstDataInInputBufferV()
{
	_ASSERTE(!m_pInputBuffer->empty());
	return *m_pInputBuffer->unsafe_begin();
}

//****************************************************************************
//FUNCTION:
void CPipelineMemoryBufferManager2GPUThread::__popFirstDataInInputBufferV()
{
	TDataType::TInputData voData;
	m_pInputBuffer->try_pop(voData);
}

//****************************************************************************
//FUNCTION:
bool CPipelineMemoryBufferManager2GPUThread::__popFromInputBufferV(TDataType::TInputData& voData)
{
	if (m_pInputBuffer->empty()) return false;
	return m_pInputBuffer->try_pop(voData);
}

//****************************************************************************
//FUNCTION:
bool CPipelineMemoryBufferManager2GPUThread::__popFromOutputBufferV(TDataType::TOutputData& voData)
{
	if (m_pOutputBuffer->empty()) return false;
	return m_pOutputBuffer->try_pop(voData);
}

//****************************************************************************
//FUNCTION:
bool CPipelineMemoryBufferManager2GPUThread::__push2InputBufferV(const TDataType::TInputData& vData)
{
	m_pInputBuffer->push(vData);
	return true;
}

//****************************************************************************
//FUNCTION:
bool CPipelineMemoryBufferManager2GPUThread::__push2OutputBufferV(const TDataType::TOutputData& vData)
{
	m_pOutputBuffer->push(vData);
	return true;
}