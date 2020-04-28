#include "PipelineFrameGenerator2PreferredTileNodeGenerator.h"

using namespace hivePagedLOD;
using TDataType = CPipelineFrameGenerator2PreferredTileNodeGenerator::TDataType;

//****************************************************************************
//FUNCTION:
TDataType::TOutputData CPipelineFrameGenerator2PreferredTileNodeGenerator::__convertDataV(const TDataType::TInputData& vInput)
{
	return vInput;
}

//****************************************************************************
//FUNCTION:
TDataType::TInputData CPipelineFrameGenerator2PreferredTileNodeGenerator::__getFirstDataInInputBufferV()
{
	_ASSERTE(!m_pInputBuffer->empty());
	return *m_pInputBuffer->unsafe_begin();
}

//****************************************************************************
//FUNCTION:
void CPipelineFrameGenerator2PreferredTileNodeGenerator::__popFirstDataInInputBufferV()
{
	TDataType::TInputData ptr;
	m_pInputBuffer->try_pop(ptr);
}

//****************************************************************************
//FUNCTION:
bool CPipelineFrameGenerator2PreferredTileNodeGenerator::__popFromInputBufferV(TDataType::TInputData& voData)
{
	return m_pInputBuffer->try_pop(voData);
}

//****************************************************************************
//FUNCTION:
bool CPipelineFrameGenerator2PreferredTileNodeGenerator::__popFromOutputBufferV(TDataType::TOutputData& voData)
{
	return m_pOutputBuffer->try_pop(voData);
}

//****************************************************************************
//FUNCTION:
bool CPipelineFrameGenerator2PreferredTileNodeGenerator::__push2InputBufferV(const TDataType::TInputData& vData)
{
	m_pInputBuffer->push(vData);
	return true;
}

//****************************************************************************
//FUNCTION:
bool CPipelineFrameGenerator2PreferredTileNodeGenerator::__push2OutputBufferV(const TDataType::TOutputData& vData)
{
	m_pOutputBuffer->push(vData);
	return true;
}