#pragma once
#include <tbb/concurrent_queue.h>
#include "Pipeline.h"
#include "Common.h"
#include "GPUTask.h"

namespace hivePagedLOD
{
	struct SPplM2GPUType
	{
		using TInputData = std::shared_ptr<SGPUTask>;
		using TInputBuffer = tbb::concurrent_queue<TInputData>;
		using TOutputData = std::shared_ptr<SGPUTask>;
		using TOutputBuffer = tbb::concurrent_queue<TOutputData>;
	};
	class CPipelineMemoryBufferManager2GPUThread: public IPipeline <
		SPplM2GPUType::TInputData,
		SPplM2GPUType::TInputBuffer,
		SPplM2GPUType::TOutputData,
		SPplM2GPUType::TOutputBuffer>
	{
	public:
		using TDataType = SPplM2GPUType;

		CPipelineMemoryBufferManager2GPUThread() = default;
		virtual ~CPipelineMemoryBufferManager2GPUThread() = default;
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