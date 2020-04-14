#pragma once
#include <tbb/concurrent_queue.h>
#include "Pipeline.h"
#include "Common.h"

namespace hivePagedLOD
{
	struct SPplF2PType
	{
		using TInputData = SViewInfo;
		using TInputBuffer = tbb::concurrent_queue<TInputData>;
		using TOutputData = SViewInfo;
		using TOutputBuffer = tbb::concurrent_queue<TOutputData>;
	};
	class CPipelineFrameGenerator2PreferredTileNodeGenerator : public IPipeline <
		SPplF2PType::TInputData,
		SPplF2PType::TInputBuffer,
		SPplF2PType::TOutputData,
		SPplF2PType::TOutputBuffer>
	{
	public:
		using TDataType = SPplF2PType;

		CPipelineFrameGenerator2PreferredTileNodeGenerator() = default;
		virtual ~CPipelineFrameGenerator2PreferredTileNodeGenerator() = default;
		virtual int getNumDataInOutputBufferV() const override { return static_cast<int>(m_pOutputBuffer->unsafe_size()); }
		virtual int getNumDataInInputBufferV()  const override { return static_cast<int>(m_pInputBuffer->unsafe_size()); }

	private:
		virtual TDataType::TOutputData __convertDataV(const TDataType::TInputData& vInput) override;
		virtual TDataType::TInputData  __getFirstDataInInputBufferV() override;

		virtual void __popFirstDataInInputBufferV() override;
		virtual bool __popFromInputBufferV(TDataType::TInputData& voData) override;
		virtual bool __popFromOutputBufferV(TDataType::TOutputData& voData) override;
		virtual bool __push2InputBufferV(const TDataType::TInputData& vData) override;
		virtual bool __push2OutputBufferV(const TDataType::TOutputData& vData) override;
	};
}