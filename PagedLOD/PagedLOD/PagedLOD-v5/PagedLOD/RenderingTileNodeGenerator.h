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
		void setLoadLimitSizePerFrame(int vLimitSize) { m_LimitLoadPerFrame = static_cast<uintmax_t>(vLimitSize); }
		void setLoadParentMaxLevel(int vLevel) { m_LoadParentMaxLevel = static_cast<unsigned int>(vLevel); }
		void setFinishLoadFrameID(unsigned int vID) { m_FinishLoadFrameID = static_cast<uintmax_t>(vID); }
		void setOutputInfo(bool vFlag) { m_OutputInfo = vFlag; }
		void setLoadLimitSizePerSolve(int vLimitSize) {m_LimitLoadPerSolve = static_cast<uintmax_t>(vLimitSize); }
		void setUseKP(bool vFlag) { m_UseKP = vFlag; }
		const std::vector<std::vector<int>>& getAllKnapsackResult() { _ASSERT(m_AllKnapsackResult.size() > 0); return m_AllKnapsackResult; }
		
		const STileNodeLoadCost& getLoadCostByUID(unsigned int vUID) const;
		const ELoadStrategy& getLoadStrategy() const { return m_LoadStrategy; }
	private:	
		void __workByPreferredTileNodeSet();

		void __generateByKnapsack();
		
		void __generateByLoadAncestor();

		void __calculateCostAndTriangle(const std::vector<std::shared_ptr<CTileNode>>& TileNodeSet, uintmax_t& voCost, uintmax_t& voTriangle);
		void __calculateCostAndTriangle1(const std::vector<std::shared_ptr<CTileNode>>& TileNodeSet, uintmax_t& voCost, uintmax_t& voTriangle, int& voLoadFileNum);

		void __removeDrawUIDSetDuplication(std::vector<std::shared_ptr<CTileNode>>& voTileNodeSet, std::set<unsigned int>& voDrawUIDSet);

		//void __generateResultForDirectLoadInOneFrame();
		//void __generateResultForKnapsackInOneFrame();

		void __showAncestor(const std::vector<std::vector<std::shared_ptr<CTileNode>>>& vTileNodeSet);
		void __generateResult();
		void __addTileNodeToLoadTaskSet(const std::vector<std::shared_ptr<CTileNode>>& vTileNodeSet, std::vector<std::shared_ptr<SLoadTask>>& voLoadTaskSet, uintmax_t& voLoadSize);

		std::shared_ptr<CPipelinePreferred2RenderingTileNodeGenerator> m_pInputPipelineFromPreferred;
		std::shared_ptr<CPipelineRenderingTileNodeGenerator2TileNodeLoader> m_pOutputPipelineToTileNodeLoader;
		std::vector<tbb::concurrent_unordered_map<unsigned int, STileNodeLoadCost>>* m_pLoadCostMapPointer;

		std::thread m_Thread;
		bool m_Close = false;

		SPreferredResult m_PreferredResult;

		double m_TotalTime = 0.0;
		uintmax_t m_FrameID = 0;

		uintmax_t m_ThisFrameTriangleCount = 0;
		uintmax_t m_FinishLoadFrameID = 0;
		bool m_OutputInfo = false;

		//QUESTION:为什么会有下面两个限制
		uintmax_t m_LimitLoadPerFrame = 0;
		uintmax_t m_LimitLoadPerSolve = 0;
		ELoadStrategy m_LoadStrategy = ELoadStrategy::UNDEFINED;
		unsigned int m_LoadParentMaxLevel = 1;

		std::vector<unsigned int> m_LastFrameDrawUIDSet;
		SRenderingGeneratorResult m_ThisFrameResult;
		
		bool m_LoadFinish = false;
		bool m_RemoveDuplication = false;
		
		////KPSolver
		bool m_UseKP = false;
		std::shared_ptr<CKnapsackProblemSolver> m_pKPSolver;
		std::vector<std::shared_ptr<CTileNode>> m_SelectedTileNodeSet;//empty==load preferred directly
		std::vector<std::shared_ptr<CTileNode>> m_ShowAncestorTileNodeSet;

		//
		int m_ViewPortNum = 0;

		//add
		std::vector<std::vector<int>> m_AllKnapsackResult;
	};
}
