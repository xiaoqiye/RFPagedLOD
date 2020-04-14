#pragma once
#include "Common.h"
#include "PipelinePreferred2RenderingTileNodeGenerator.h"
#include "PipelineFrameGenerator2PreferredTileNodeGenerator.h"
#include "Scene.h"

namespace hivePagedLOD
{
	class CPreferredTileNodeGenerator
	{
	public:
		CPreferredTileNodeGenerator() = delete;
		CPreferredTileNodeGenerator(const std::shared_ptr<CPipelineFrameGenerator2PreferredTileNodeGenerator>& vInputPipeline, const std::shared_ptr<CPipelinePreferred2RenderingTileNodeGenerator>& vOutputPipeline);
		~CPreferredTileNodeGenerator() = default;

		void start();
		void join() { _ASSERTE(m_Thread.joinable()); m_Thread.join(); }
		void setFinishSignal(bool vFinish) { m_Close = vFinish; }

		std::shared_ptr<CPipelineFrameGenerator2PreferredTileNodeGenerator>& fetchInputPipeline() { return m_pInputPipeline; }
		std::shared_ptr<CPipelinePreferred2RenderingTileNodeGenerator>& fetchOutputPipeline() { return m_pOutputPipeline; }

	private:
		void __workByViewInfo();
		void __generatePreferredTileNodeSet(std::shared_ptr<CTileNode>& vTileNode, std::vector<std::shared_ptr<CTileNode>>& voPreferredTileNode, unsigned int& voPreferredSetMinDeep, unsigned int& voPreferredSetMaxDeep) const;
		bool __isPreferredTileNode(const std::shared_ptr<CTileNode>& vTileNode) const;
		bool __isTileVisible(const std::shared_ptr<CTileNode>& vTile) const;

		std::shared_ptr<CPipelineFrameGenerator2PreferredTileNodeGenerator> m_pInputPipeline;
		std::shared_ptr<CPipelinePreferred2RenderingTileNodeGenerator> m_pOutputPipeline;

		std::thread m_Thread;
		bool m_Close = false;

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		SViewInfo m_ViewInfo;

		SViewInfo m_LastViewInfo = SViewInfo();
		std::vector<unsigned int> m_LastPreferredTileNumSet;
		
		std::vector<glm::vec4> m_FrustumPlanes;
	};
}