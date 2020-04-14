#include "PipelineTileNodeLoader2MemoryBufferManager.h"

using namespace hivePagedLOD;
using TDataType = CPipelineTileNodeLoader2MemoryBufferManager::TDataType;

//****************************************************************************
//FUNCTION:
TDataType::TOutputData CPipelineTileNodeLoader2MemoryBufferManager::__convertDataV(const TDataType::TInputData& vInput)
{
	return vInput;
}

//****************************************************************************
//FUNCTION:
TDataType::TInputData CPipelineTileNodeLoader2MemoryBufferManager::__getFirstDataInInputBufferV()
{
	_ASSERTE(!m_pInputBuffer->empty());
	return *m_pInputBuffer->unsafe_begin();
}

//****************************************************************************
//FUNCTION:
void CPipelineTileNodeLoader2MemoryBufferManager::__popFirstDataInInputBufferV()
{
	TDataType::TInputData ptr;
	m_pInputBuffer->try_pop(ptr);
}

//****************************************************************************
//FUNCTION:
bool CPipelineTileNodeLoader2MemoryBufferManager::__popFromInputBufferV(TDataType::TInputData& voData)
{
	return m_pInputBuffer->try_pop(voData);
}

//****************************************************************************
//FUNCTION:
bool CPipelineTileNodeLoader2MemoryBufferManager::__popFromOutputBufferV(TDataType::TOutputData& voData)
{
	return m_pOutputBuffer->try_pop(voData);
}

//****************************************************************************
//FUNCTION:
bool CPipelineTileNodeLoader2MemoryBufferManager::__push2InputBufferV(const TDataType::TInputData& vData)
{
	m_pInputBuffer->push(vData);
	return true;
}

//****************************************************************************
//FUNCTION:
bool CPipelineTileNodeLoader2MemoryBufferManager::__push2OutputBufferV(const TDataType::TOutputData& vData)
{
	m_pOutputBuffer->push(vData);
	return true;
}