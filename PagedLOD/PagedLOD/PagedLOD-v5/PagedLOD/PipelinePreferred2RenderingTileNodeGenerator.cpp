#include "PipelinePreferred2RenderingTileNodeGenerator.h"

using namespace hivePagedLOD;
using TDataType = CPipelinePreferred2RenderingTileNodeGenerator::TDataType;

//****************************************************************************
//FUNCTION:
TDataType::TOutputData CPipelinePreferred2RenderingTileNodeGenerator::__convertDataV(const TDataType::TInputData& vInput)
{
	return vInput;
}

//****************************************************************************
//FUNCTION:
TDataType::TInputData CPipelinePreferred2RenderingTileNodeGenerator::__getFirstDataInInputBufferV()
{
	_ASSERTE(!m_pInputBuffer->empty());
	return *m_pInputBuffer->unsafe_begin();
}

//****************************************************************************
//FUNCTION:
void CPipelinePreferred2RenderingTileNodeGenerator::__popFirstDataInInputBufferV()
{
	TDataType::TInputData ptr;
	m_pInputBuffer->try_pop(ptr);
}

//****************************************************************************
//FUNCTION:
bool CPipelinePreferred2RenderingTileNodeGenerator::__popFromInputBufferV(TDataType::TInputData& voData)
{
	return m_pInputBuffer->try_pop(voData);
}

//****************************************************************************
//FUNCTION:
bool CPipelinePreferred2RenderingTileNodeGenerator::__popFromOutputBufferV(TDataType::TOutputData& voData)
{
	return m_pOutputBuffer->try_pop(voData);
}

//****************************************************************************
//FUNCTION:
bool CPipelinePreferred2RenderingTileNodeGenerator::__push2InputBufferV(const TDataType::TInputData& vData)
{
	m_pInputBuffer->push(vData);
	return true;
}

//****************************************************************************
//FUNCTION:
bool CPipelinePreferred2RenderingTileNodeGenerator::__push2OutputBufferV(const TDataType::TOutputData& vData)
{
	m_pOutputBuffer->push(vData);
	return true;
}