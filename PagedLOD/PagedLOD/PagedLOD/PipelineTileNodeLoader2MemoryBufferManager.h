#pragma once
#include <tbb/concurrent_queue.h>

#include "Common.h"
#include "Pipeline.h"
#include "TileNode.h"
#include <vector>

namespace hivePagedLOD
{
	struct SPplL2MType
	{
		using TInputData = std::shared_ptr<SLoaderResult>;
		using TInputBuffer = tbb::concurrent_queue<TInputData>;
		using TOutputData = std::shared_ptr<SLoaderResult>;
		using TOutputBuffer = tbb::concurrent_queue<TOutputData>;
	};
	class CPipelineTileNodeLoader2MemoryBufferManager : public IPipeline <
		SPplL2MType::TInputData,
		SPplL2MType::TInputBuffer,
		SPplL2MType::TOutputData,
		SPplL2MType::TOutputBuffer>
	{
	public:
		using TDataType = SPplL2MType;

		CPipelineTileNodeLoader2MemoryBufferManager() = default;
		virtual ~CPipelineTileNodeLoader2MemoryBufferManager() = default;
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