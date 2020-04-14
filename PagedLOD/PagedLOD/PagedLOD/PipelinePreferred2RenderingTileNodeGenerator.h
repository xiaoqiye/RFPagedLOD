#pragma once
#include "Common.h"
#include "Pipeline.h"
#include "TileNode.h"
#include <tbb/concurrent_queue.h>
#include <vector>

namespace hivePagedLOD
{
	struct SPplP2RType
	{
		using TInputData = SPreferredResult;
		using TInputBuffer = tbb::concurrent_queue<TInputData>;
		using TOutputData = SPreferredResult;
		using TOutputBuffer = tbb::concurrent_queue<TInputData>;
	};
	class CPipelinePreferred2RenderingTileNodeGenerator : public IPipeline <
		SPplP2RType::TInputData,
		SPplP2RType::TInputBuffer,
		SPplP2RType::TOutputData,
		SPplP2RType::TOutputBuffer>
	{
	public:
		using TDataType = SPplP2RType;

		CPipelinePreferred2RenderingTileNodeGenerator() = default;
		virtual ~CPipelinePreferred2RenderingTileNodeGenerator() = default;
		virtual int getNumDataInOutputBufferV() const override { return static_cast<int>(m_pOutputBuffer->unsafe_size()); }
		virtual int getNumDataInInputBufferV()  const override { return static_cast<int>(m_pInputBuffer->unsafe_size()); }

	private:
		virtual TDataType::TOutputData __convertDataV(const TDataType::TInputData& vInput) override;
		virtual TDataType::TInputData  __getFirstDataInInputBufferV() override;

		virtual void __popFirstDataInInputBufferV() override;
		virtual bool __popFromInputBufferV(TDataType::TInputData & voData) override;
		virtual bool __popFromOutputBufferV(TDataType::TOutputData & voData) override;
		virtual bool __push2InputBufferV(const TDataType::TInputData & vData) override;
		virtual bool __push2OutputBufferV(const TDataType::TOutputData & vData) override;
	};
}