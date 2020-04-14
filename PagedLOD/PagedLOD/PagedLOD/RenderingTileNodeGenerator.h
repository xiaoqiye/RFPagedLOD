#pragma once

#include "Common.h"
#include "Scene.h"
#include "PipelinePreferred2RenderingTileNodeGenerator.h"
#include "PipelineRenderingTileNodeGenerator2TileNodeLoader.h"
#include <tbb/concurrent_unordered_map.h>
#include <set>
#include "KnapsackProblemSolver.h"
#include <queue>

namespace hivePagedLOD
{

	enum class ELoadStrategy
	{
		LOAD_PARENT,
		KNAPSACK,
		UNDEFINED
	};

	class CRenderingTileNodeGenerator
	{
	public:
		CRenderingTileNodeGenerator() = delete;
		CRenderingTileNodeGenerator(const std::shared_ptr<CPipelinePreferred2RenderingTileNodeGenerator>& vInputPipelineFromPreferred,
			const std::shared_ptr<CPipelineRenderingTileNodeGenerator2TileNodeLoader>& vOutputPipelineToTileNodeLoader,
			std::vector<tbb::concurrent_unordered_map<unsigned int, STileNodeLoadCost>>* vLoadCostMapPointer);
		~CRenderingTileNodeGenerator() = default;

		void start();
		void join() { _ASSERTE(m_Thread.joinable()); m_Thread.join(); }
		void setFinishSignal(bool vFinish) { m_Close = vFinish; }

		void initStrategyConfig(const std::string& vStrategy, uintmax_t vLimitSize, unsigned int vLevel);

		void setLoadStrategy(const ELoadStrategy& vStrategy) { m_LoadStrategy = vStrategy; }
		void setLoadCostMapPointer(std::vector<tbb::concurrent_unordered_map<unsigned int, STileNodeLoadCost>>* vPointer) { m_pLoadCostMapPointer = vPointer; }
		//FIXME:输入参数的合法性判定
		void setLoadLimitSizePerFrame(int vLimitSize) { m_LimitLoadPerFrame = static_cast<uintmax_t>(vLimitSize); }
		void setLoadParentMaxLevel(int vLevel) { m_LoadParentMaxLevel = static_cast<unsigned int>(vLevel); }
		void setFinishLoadFrameID(unsigned int vID) { m_FinishLoadFrameID = static_cast<uintmax_t>(vID); }
		void setOutputInfo(bool vFlag) { m_OutputInfo = vFlag; }

		const STileNodeLoadCost& getLoadCostByUID(unsigned int vUID) const;
		const ELoadStrategy& getLoadStrategy() const { return m_LoadStrategy; }
	private:
		void __workByPreferredTileNodeSet();

		void __generateByKnapsack();
		void __generateByLoadDirectly();
		
		void __calculateCostAndTriangle(const std::vector<std::shared_ptr<CTileNode>>& TileNodeSet, uintmax_t& voCost, uintmax_t& voTriangle);

		void __generateDrawUIDSetAndLoadTaskSetForKnapsack(std::vector<SKnapsackItem>& vNeedLoadItemSet, std::vector<std::vector<std::shared_ptr<CTileNode>>>& vNeedShowAncestorTileNodeSet);

		void __removeDrawUIDSetDuplication(std::vector<std::shared_ptr<CTileNode>>& voTileNodeSet, std::set<unsigned int>& voDrawUIDSet);

		void __generateResultForDirectLoadInOneFrame();
		void __generateResultForKnapsackInOneFrame();

		std::shared_ptr<CPipelinePreferred2RenderingTileNodeGenerator> m_pInputPipelineFromPreferred;
		std::shared_ptr<CPipelineRenderingTileNodeGenerator2TileNodeLoader> m_pOutputPipelineToTileNodeLoader;
		//FIXME:智能指针
		std::vector<tbb::concurrent_unordered_map<unsigned int, STileNodeLoadCost>>* m_pLoadCostMapPointer;

		std::thread m_Thread;
		bool m_Close = false;

		SPreferredResult m_PreferredResult;

		double m_TotalTime = 0.0;
		uintmax_t m_FrameID = 0;

		uintmax_t m_ThisFrameTriangleCount = 0;
		uintmax_t m_FinishLoadFrameID = 0;
		bool m_OutputInfo = false;

		uintmax_t m_LimitLoadPerFrame = 0;
		unsigned int m_MaxWaitFrame = 38;
		ELoadStrategy m_LoadStrategy = ELoadStrategy::UNDEFINED;
		unsigned int m_LoadParentMaxLevel = 1;

		std::vector<unsigned int> m_LastFrameDrawUIDSet;
		SRenderingGeneratorResult m_ThisFrameResult;
		
		bool m_DirectLoad = false;
		bool m_LoadFinish = false;
		
		////KPSolver
		std::shared_ptr<CKnapsackProblemSolver> m_pKPSolver;
		std::vector<std::shared_ptr<CTileNode>> m_SelectedTileNodeSet;
		std::vector<std::shared_ptr<CTileNode>> m_ShowAncestorTileNodeSet;
	};
}
