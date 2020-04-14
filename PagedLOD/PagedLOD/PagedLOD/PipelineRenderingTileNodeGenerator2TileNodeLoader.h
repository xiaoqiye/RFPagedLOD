#pragma once
#include <tbb/concurrent_queue.h>
#include <vector>
#include "Pipeline.h"
#include "Common.h"
#include "TileNode.h"

namespace hivePagedLOD
{
	struct SPplR2LType
	{
		using TInputData = std::shared_ptr<SRenderingGeneratorResult>;
		using TInputBuffer = tbb::concurrent_queue<TInputData>;
		using TOutputData = std::shared_ptr<SRenderingGeneratorResult>;
		using TOutputBuffer = tbb::concurrent_queue<TOutputData>;
	};
	class CPipelineRenderingTileNodeGenerator2TileNodeLoader : public IPipeline <
		SPplR2LType::TInputData,
		SPplR2LType::TInputBuffer,
		SPplR2LType::TOutputData,
		SPplR2LType::TOutputBuffer>
	{
	public:
		using TDataType = SPplR2LType;

		CPipelineRenderingTileNodeGenerator2TileNodeLoader() = default;
		virtual ~CPipelineRenderingTileNodeGenerator2TileNodeLoader() = default;
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
