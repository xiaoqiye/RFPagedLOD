#pragma once
#include <queue>
#include <tbb/concurrent_queue.h>
#include "Pipeline.h"
#include "Common.h"
#include "FrameState.h"

namespace hivePagedLOD
{
	struct SPplG2FType
	{
		using TInputData = std::shared_ptr<IFrameState>;
		using TInputBuffer = tbb::concurrent_queue<TInputData>;
		using TOutputData = std::shared_ptr<IFrameState>;
		using TOutputBuffer = tbb::concurrent_queue<TOutputData>;
	};
	class CPipelineGPUThread2FrameGenerator : public IPipeline <
		SPplG2FType::TInputData,
		SPplG2FType::TInputBuffer,
		SPplG2FType::TOutputData,
		SPplG2FType::TOutputBuffer>
	{
	public:
		using TDataType = SPplG2FType;

		CPipelineGPUThread2FrameGenerator() = default;
		virtual ~CPipelineGPUThread2FrameGenerator() = default;
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