#include "PipelineRenderingTileNodeGenerator2TileNodeLoader.h"

using namespace hivePagedLOD;
using TDataType = CPipelineRenderingTileNodeGenerator2TileNodeLoader::TDataType;

//****************************************************************************
//FUNCTION:
TDataType::TOutputData CPipelineRenderingTileNodeGenerator2TileNodeLoader::__convertDataV(const TDataType::TInputData& vInput)
{
	return vInput;
}

//****************************************************************************
//FUNCTION:
TDataType::TInputData CPipelineRenderingTileNodeGenerator2TileNodeLoader::__getFirstDataInInputBufferV()
{
	_ASSERTE(!m_pInputBuffer->empty());
	return *m_pInputBuffer->unsafe_begin();
}

//****************************************************************************
//FUNCTION:
void CPipelineRenderingTileNodeGenerator2TileNodeLoader::__popFirstDataInInputBufferV()
{
	TDataType::TInputData ptr;
	m_pInputBuffer->try_pop(ptr);
}

//****************************************************************************
//FUNCTION:
bool CPipelineRenderingTileNodeGenerator2TileNodeLoader::__popFromInputBufferV(TDataType::TInputData& voData)
{
	return m_pInputBuffer->try_pop(voData);
}

//****************************************************************************
//FUNCTION:
bool CPipelineRenderingTileNodeGenerator2TileNodeLoader::__popFromOutputBufferV(TDataType::TOutputData& voData)
{
	return m_pOutputBuffer->try_pop(voData);
}

//****************************************************************************
//FUNCTION:
bool CPipelineRenderingTileNodeGenerator2TileNodeLoader::__push2InputBufferV(const TDataType::TInputData& vData)
{
	m_pInputBuffer->push(vData);
	return true;
}

//****************************************************************************
//FUNCTION:
bool CPipelineRenderingTileNodeGenerator2TileNodeLoader::__push2OutputBufferV(const TDataType::TOutputData& vData)
{
	m_pOutputBuffer->push(vData);
	return true;
}