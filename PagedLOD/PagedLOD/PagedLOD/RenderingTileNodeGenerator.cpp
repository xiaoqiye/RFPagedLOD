#include "RenderingTileNodeGenerator.h"
#include "Scene.h"
#include "Timer.h"
#include <iostream>

using namespace hivePagedLOD;

CRenderingTileNodeGenerator::CRenderingTileNodeGenerator(const std::shared_ptr<CPipelinePreferred2RenderingTileNodeGenerator>& vInputPipelineFromPreferred, const std::shared_ptr<CPipelineRenderingTileNodeGenerator2TileNodeLoader>& vOutputPipelineToTileNodeLoader, std::vector<tbb::concurrent_unordered_map<unsigned int, STileNodeLoadCost>>* vLoadCostMapPointer)
	:m_pInputPipelineFromPreferred(vInputPipelineFromPreferred),
	m_pOutputPipelineToTileNodeLoader(vOutputPipelineToTileNodeLoader),
	m_pLoadCostMapPointer(vLoadCostMapPointer)
{
	_ASSERTE(m_pInputPipelineFromPreferred && m_pOutputPipelineToTileNodeLoader);
	m_pKPSolver = std::make_shared<CKnapsackProblemSolver>();
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::start()
{
	m_Thread = std::thread([&] {__workByPreferredTileNodeSet(); });
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::initStrategyConfig(const std::string& vStrategy, uintmax_t vLimitSize, unsigned int vLevel)
{
	//FIXME:输入参数都需要有效性判定 vLimitSize>0 vLevel>=0
	_ASSERTE(vStrategy == LOAD_STRATEGY_KNAPSACK);
	m_LoadStrategy = ELoadStrategy::KNAPSACK;

	m_LimitLoadPerFrame = vLimitSize;
	m_LoadParentMaxLevel = vLevel;
}

//****************************************************************************
//FUNCTION:
const STileNodeLoadCost& CRenderingTileNodeGenerator::getLoadCostByUID(unsigned vUID) const
{
	//FIXME:输入参数
	unsigned int TileNum = vUID & UID_TILE_NUM_MASK;
	TileNum >>= OFFSET_BIT;
	return m_pLoadCostMapPointer->at(TileNum).find(vUID)->second;
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__workByPreferredTileNodeSet()
{
	while (!m_Close)
	{
		SPreferredResult PreferredResult;

		if (m_pInputPipelineFromPreferred->tryPop(1, PreferredResult))
		{
			++m_FrameID;

			if (PreferredResult.PreferredTileNodeSet.empty() && PreferredResult.MaxDeepSet.empty())
			{
				//question:什么时候会出现视点不变化还需要加载数据的情况，是往更精细的方向？
				////视点不变化，根据策略生成每一帧DrawUIDSet和LoadTaskSet
				if (!m_LoadFinish)
				{
					if (m_DirectLoad)
						__generateResultForDirectLoadInOneFrame();
					else
						__generateResultForKnapsackInOneFrame();

					m_LastFrameDrawUIDSet = m_ThisFrameResult.DrawUIDSet;
					m_pOutputPipelineToTileNodeLoader->tryPush(std::make_shared<SRenderingGeneratorResult>(m_ThisFrameResult));
				}
				////视点不变化并且加载完成，显示前一帧内容
				else
				{
					std::vector<std::shared_ptr<SLoadTask>> LoadTaskSet;
					m_pOutputPipelineToTileNodeLoader->tryPush(std::make_shared<SRenderingGeneratorResult>(LoadTaskSet, m_LastFrameDrawUIDSet));
				}
			}
			////视点变化，需要重新生成模板解
			else
			{
				m_PreferredResult = PreferredResult;

				uintmax_t TotalCost = 0, TotalTriangle = 0;
				for (auto& TileNodeSet : m_PreferredResult.PreferredTileNodeSet)
				{
					uintmax_t Cost = 0, Triangle = 0;
					__calculateCostAndTriangle(TileNodeSet, Cost, Triangle);
					TotalCost += Cost;
					TotalTriangle += Triangle;
				}

				//question：m_LimitLoadPerFrame  m_MaxWaitFrame代表的含义
				if (TotalCost > m_LimitLoadPerFrame * m_MaxWaitFrame)
				{
					__generateByKnapsack();
					m_DirectLoad = false;
				}
				else
					m_DirectLoad = true;

				std::vector<std::shared_ptr<SLoadTask>> LoadTaskSet;
				std::vector<unsigned int> DrawUIDSet;

				m_LoadFinish = false;

				for (auto& Root : CScene::getInstance()->getTileSet())
				{
					DrawUIDSet.emplace_back(Root->getUID());
				}
				m_pOutputPipelineToTileNodeLoader->tryPush(std::make_shared<SRenderingGeneratorResult>(LoadTaskSet, DrawUIDSet));
			}
		}
		//++m_FrameID;
		//CTimer::getInstance()->tick(__FUNCTION__);
		////frame time
		//if (m_FrameID == 1)
		//	CTimer::getInstance()->tick(__FUNCTION__);
		//else
		//{
		//	CTimer::getInstance()->tock(__FUNCTION__);
		//	m_TotalTime += CTimer::getInstance()->getCostTimeByName(__FUNCTION__);
		//	if (CTimer::getInstance()->needOutput() && CTimer::getInstance()->isRegistered(__FUNCTION__))
		//		std::cout << __FUNCTION__ << CTimer::getInstance()->getCostTimeByName(__FUNCTION__) << std::endl;
		//	if (CTimer::getInstance()->needOutput() && m_FrameID == m_FinishLoadFrameID)
		//		std::cout << m_TotalTime << std::endl;
		//	CTimer::getInstance()->tick(__FUNCTION__);
		//}
		//m_ThisFrameTriangleCount = 0;
		//if (LoadTaskSet.empty() && m_OutputInfo)
		//	std::cout << "finish " << m_FrameID << std::endl;
		//if (m_OutputInfo)
		//	std::cout << "triangle: " << m_ThisFrameTriangleCount << std::endl;
		//CTimer::getInstance()->tock(__FUNCTION__);
		//if (CTimer::getInstance()->needOutput())
		//	std::cout << __FUNCTION__ << CTimer::getInstance()->getCostTimeByName(__FUNCTION__) << std::endl;
	}
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__generateResultForKnapsackInOneFrame()
{
	std::vector<std::shared_ptr<SLoadTask>> LoadTaskSet;
	std::set<unsigned int> FinishProcessTileNodeUIDSet;
	std::set<unsigned int> DrawUIDSet;
	uintmax_t LoadSize = 0;
	std::set<std::string> TexNameSet;
	std::vector<std::shared_ptr<CTileNode>> DrawTileNodeSet;

	////process selected item set

	unsigned int LastAncestorUID = INT_MAX;
	for (auto& TileNode : m_SelectedTileNodeSet)
	{
		const auto& UID = TileNode->getUID();
		if (FinishProcessTileNodeUIDSet.find(UID) != FinishProcessTileNodeUIDSet.end())
		{
			DrawUIDSet.insert(UID);
			continue;
		}

		if (LoadSize < m_LimitLoadPerFrame)
		{
			const auto& LoadCost = getLoadCostByUID(UID);
			const auto& TexName = TileNode->getTextureFileName();
			DrawUIDSet.insert(UID);
			DrawTileNodeSet.emplace_back(TileNode);
			FinishProcessTileNodeUIDSet.insert(UID);
			if (LoadCost.LoadCost != 0)
			{
				LoadSize += LoadCost.GeoSize;
				if (!LoadCost.TexInMemory && TexNameSet.find(TexName) == TexNameSet.end())
				{
					LoadSize += LoadCost.TexSize;
					TexNameSet.insert(TexName);
				}
				LoadTaskSet.emplace_back(std::make_shared<SLoadTask>(TileNode, !LoadCost.TexInMemory));
			}
		}
		else
		{
			auto& TileNodeAncestor = TileNode->getAncestorUIDSet();
			auto it = TileNodeAncestor.rbegin();
			for (; it != TileNodeAncestor.rend(); ++it)
			{
				if (LastAncestorUID == *it)
					break;
				if (*it != 0 && getLoadCostByUID(*it).LoadCost == 0)
				{
					DrawUIDSet.insert(*it);
					DrawTileNodeSet.emplace_back(CScene::getInstance()->getTileNodePointerByUID(*it));
					FinishProcessTileNodeUIDSet.insert(*it);
					LastAncestorUID = *it;
					break;
				}
			}
		}
	}

	//// process show ancestor set
	LastAncestorUID = INT_MAX;
	for (auto& TileNode : m_ShowAncestorTileNodeSet)
	{
		const auto& UID = TileNode->getUID();
		if (FinishProcessTileNodeUIDSet.find(UID) != FinishProcessTileNodeUIDSet.end())
		{
			DrawUIDSet.insert(UID);
			continue;
		}
		//// has no load task
		const auto& LoadCost = getLoadCostByUID(UID);
		if (LoadCost.LoadCost == 0)
		{
			DrawUIDSet.insert(UID);
			DrawTileNodeSet.emplace_back(TileNode);
			FinishProcessTileNodeUIDSet.insert(UID);
			continue;
		}
		
		auto& TileNodeAncestor = TileNode->getAncestorUIDSet();
		auto it = TileNodeAncestor.rbegin();
		for (; it != TileNodeAncestor.rend(); ++it)
		{
			if (LastAncestorUID == *it)
				break;
			if (*it != 0 && getLoadCostByUID(*it).LoadCost == 0)
			{
				DrawUIDSet.insert(*it);
				DrawTileNodeSet.emplace_back(CScene::getInstance()->getTileNodePointerByUID(*it));
				FinishProcessTileNodeUIDSet.insert(*it);
				LastAncestorUID = *it;
				break;
			}
		}
	}

	__removeDrawUIDSetDuplication(DrawTileNodeSet, DrawUIDSet);

	std::vector<unsigned int> ResultDrawUIDSet;
	for (auto& i : DrawUIDSet)
		ResultDrawUIDSet.emplace_back(i);

	if (LoadTaskSet.empty())
		std::cout << "finish frame = "<<m_FrameID << std::endl;
	
	////load preferred
	if (LoadTaskSet.empty())
	{
		for (auto& TileNodeSet : m_PreferredResult.PreferredTileNodeSet)
		{
			for (auto& TileNode : TileNodeSet)
			{
				if (LoadSize < m_LimitLoadPerFrame)
				{
					const auto& UID = TileNode->getUID();
					const auto& LoadCost = getLoadCostByUID(UID);
					const auto& TexName = TileNode->getTextureFileName();
					if (LoadCost.LoadCost != 0)
					{
						LoadSize += LoadCost.GeoSize;
						if (!LoadCost.TexInMemory && TexNameSet.find(TexName) == TexNameSet.end())
						{
							LoadSize += LoadCost.TexSize;
							TexNameSet.insert(TexName);
						}
						LoadTaskSet.emplace_back(std::make_shared<SLoadTask>(TileNode, !LoadCost.TexInMemory));
					}
				}
				else
					break;
			}
		}
	}
	
	////finish load preferred
	if (LoadTaskSet.empty())
	{
		std::cout << "really finish frame = " << m_FrameID << std::endl;

		ResultDrawUIDSet.clear();
		for (auto& TileNodeSet : m_PreferredResult.PreferredTileNodeSet)
			for (auto& TileNode : TileNodeSet)
				ResultDrawUIDSet.emplace_back(TileNode->getUID());
		m_LoadFinish = true;
	}

	m_ThisFrameResult = SRenderingGeneratorResult(LoadTaskSet, ResultDrawUIDSet);
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__generateResultForDirectLoadInOneFrame()
{
	std::vector<std::shared_ptr<SLoadTask>> LoadTaskSet;
	std::set<unsigned int> FinishProcessTileNodeUIDSet;

	std::set<unsigned int> DrawUIDSet;
	uintmax_t LoadSize = 0;
	std::set<std::string> TexNameSet;
	std::vector<std::shared_ptr<CTileNode>> DrawTileNodeSet;

	for (auto& TileNodeSet : m_PreferredResult.PreferredTileNodeSet)
	{
		unsigned int LastAncestorUID = INT_MAX;
		for (auto& TileNode : TileNodeSet)
		{
			const auto& UID = TileNode->getUID();
			//qusetion:PreferredTileNodeSet的不同Tile可能出现相同的TileNode？
			if (FinishProcessTileNodeUIDSet.find(UID) != FinishProcessTileNodeUIDSet.end())
			{
				DrawUIDSet.insert(UID);
				continue;
			}

			if (LoadSize < m_LimitLoadPerFrame)
			{
				const auto& LoadCost = getLoadCostByUID(UID);
				const auto& TexName = TileNode->getTextureFileName();
				DrawUIDSet.insert(UID);
				DrawTileNodeSet.emplace_back(TileNode);
				FinishProcessTileNodeUIDSet.insert(UID);
				//FIXME:命名不好
				if (LoadCost.LoadCost != 0)
				{
					LoadSize += LoadCost.GeoSize;
					if (!LoadCost.TexInMemory && TexNameSet.find(TexName) == TexNameSet.end())
					{
						LoadSize += LoadCost.TexSize;
						TexNameSet.insert(TexName);
					}
					LoadTaskSet.emplace_back(std::make_shared<SLoadTask>(TileNode, !LoadCost.TexInMemory));
				}
			}
			else
			{
				auto& TileNodeAncestor = TileNode->getAncestorUIDSet();
				auto it = TileNodeAncestor.rbegin();
				for (; it != TileNodeAncestor.rend(); ++it)
				{
					if (LastAncestorUID == *it)
						break;
					//question
					if (*it != 0 && getLoadCostByUID(*it).LoadCost == 0)
					{
						DrawUIDSet.insert(*it);
						DrawTileNodeSet.emplace_back(CScene::getInstance()->getTileNodePointerByUID(*it));
						FinishProcessTileNodeUIDSet.insert(*it);
						LastAncestorUID = *it;
						break;
					}
				}
			}
		}
	}

	//question:为什么会出现重复
	__removeDrawUIDSetDuplication(DrawTileNodeSet, DrawUIDSet);

	//FIXME:命名
	std::vector<unsigned int> t;
	for (auto& i : DrawUIDSet)
		t.emplace_back(i);

	if (LoadTaskSet.empty())
		m_LoadFinish = true;

	m_ThisFrameResult = SRenderingGeneratorResult(LoadTaskSet, t);
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__calculateCostAndTriangle(const std::vector<std::shared_ptr<CTileNode>>& TileNodeSet, uintmax_t& voCost, uintmax_t& voTriangle)
{
	voCost = 0;
	voTriangle = 0;
	std::map<std::string, uintmax_t> freq;
	for (auto& Tile : TileNodeSet)
	{
		auto& LoadCost = getLoadCostByUID(Tile->getUID());
		voTriangle += LoadCost.TriangleCount;
		if (LoadCost.TexInMemory)
			voCost += LoadCost.LoadCost;
		else
		{
			voCost += LoadCost.GeoSize;
			freq[Tile->getTextureFileName()] = LoadCost.TexSize;
		}
	}
	for (auto& i : freq)
		voCost += i.second;
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__generateByLoadDirectly()
{
	std::vector<std::shared_ptr<SLoadTask>> LoadTaskSet;
	std::set<unsigned int> FinishProcessTileNodeUIDSet;

	do
	{
		LoadTaskSet.clear();
		std::set<unsigned int> DrawUIDSet;
		uintmax_t LoadSize = 0;
		std::set<std::string> TexNameSet;
		for (auto& TileNodeSet : m_PreferredResult.PreferredTileNodeSet)
		{
			unsigned int LastAncestorUID = INT_MAX;
			for (auto& TileNode : TileNodeSet)
			{
				const auto& UID = TileNode->getUID();
				if (FinishProcessTileNodeUIDSet.find(UID) != FinishProcessTileNodeUIDSet.end())
				{
					DrawUIDSet.insert(UID);
					continue;
				}

				if (LoadSize < m_LimitLoadPerFrame)
				{
					const auto& LoadCost = getLoadCostByUID(UID);
					const auto& TexName = TileNode->getTextureFileName();
					DrawUIDSet.insert(UID);
					FinishProcessTileNodeUIDSet.insert(UID);
					if (LoadCost.LoadCost != 0)
					{
						LoadSize += LoadCost.GeoSize;
						if (!LoadCost.TexInMemory && TexNameSet.find(TexName) == TexNameSet.end())
						{
							LoadSize += LoadCost.TexSize;
							TexNameSet.insert(TexName);
						}
						LoadTaskSet.emplace_back(std::make_shared<SLoadTask>(TileNode, !LoadCost.TexInMemory));
					}
				}
				else
				{
					auto& TileNodeAncestor = TileNode->getAncestorUIDSet();
					auto it = TileNodeAncestor.rbegin();
					for (; it != TileNodeAncestor.rend(); ++it)
					{
						if (LastAncestorUID == *it)
							break;
						if (*it != 0 && getLoadCostByUID(*it).LoadCost == 0)
						{
							DrawUIDSet.insert(*it);
							FinishProcessTileNodeUIDSet.insert(*it);
							LastAncestorUID = *it;
							break;
						}
					}
				}
			}
		}

		//__removeDrawUIDSetDuplication(m_PreferredResult.PreferredTileNodeSet, DrawUIDSet);

		std::vector<unsigned int> t;
		for (auto& i : DrawUIDSet)
			t.emplace_back(i);

		m_ThisFrameResult = SRenderingGeneratorResult(LoadTaskSet, t);

	} while (!LoadTaskSet.empty());
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__generateByKnapsack()
{
	std::vector<std::shared_ptr<SLoadTask>> voLoadTaskSet;
	std::vector<unsigned int> voDrawUIDSet;

	auto ItemSet = m_pKPSolver->prepareKnapsackItem(m_PreferredResult, CScene::getInstance()->getItemTemplateCreatorPointer()->getAllTileItemTemplateSet());

	for (auto& i : ItemSet)
	{
		for (auto& k : i)
		{
			__calculateCostAndTriangle(k.TileNodeSet, k.LoadCostForKnapsackItem, k.TriangleCount);
			k.LoadCostForKnapsackItem = k.LoadCostForKnapsackItem / KILOBYTE == 0 ? 1 : k.LoadCostForKnapsackItem / KILOBYTE;
		}
	}

	std::vector<unsigned int> NotLoadTileNumSet;
	const auto LoadSizeKB = m_LimitLoadPerFrame * m_MaxWaitFrame / KILOBYTE;
	std::vector<SKnapsackItem> SelectedItemSet;
	m_pKPSolver->solveMultipleChoiceKnapsackProblem(ItemSet, LoadSizeKB, NotLoadTileNumSet, SelectedItemSet);
	
	m_ShowAncestorTileNodeSet.clear();
	for (auto& i : NotLoadTileNumSet)
		for (auto& TileNode : m_PreferredResult.PreferredTileNodeSet[i])
			m_ShowAncestorTileNodeSet.emplace_back(TileNode);
	m_SelectedTileNodeSet.clear();
	for (auto& item : SelectedItemSet)
		for (auto& TileNode : item.TileNodeSet)
			m_SelectedTileNodeSet.emplace_back(TileNode);

	//// output info
	uintmax_t TotalCost = 0, TotalTriangle = 0;
	for (auto& i : SelectedItemSet)
	{
		uintmax_t Cost = 0, Triangle = 0;
		__calculateCostAndTriangle(i.TileNodeSet, Cost, Triangle);
		TotalCost += Cost;
		TotalTriangle += Triangle;
	}
	std::cout << "KP Result: Triangle = " << TotalTriangle << " Cost = " << TotalCost << " TileNodeCount = " << m_SelectedTileNodeSet.size() << std::endl;

}

////****************************************************************************
////FUNCTION:
//void CRenderingTileNodeGenerator::__generateDrawUIDSetAndLoadTaskSetForKnapsack(std::vector<SKnapsackItem>& vNeedLoadItemSet, std::vector<std::vector<std::shared_ptr<CTileNode>>>& vNeedShowAncestorTileNodeSet, std::vector<std::vector<unsigned int>>& voDrawUIDSet, std::vector<std::vector<std::shared_ptr<SLoadTask>>>& voLoadTaskSet)
//{
//	////process not load preferred tile node to show
//	std::set<unsigned int >NotLoadDrawUIDSet;
//	////find ancestor to show
//	for (auto& TileNodeSet : vNeedShowAncestorTileNodeSet)
//	{
//		unsigned int LastAncestorUID = INT_MAX;
//		for (auto& TileNode : TileNodeSet)
//		{
//			auto& TileNodeAncestor = TileNode->getAncestorUIDSet();
//			auto it = TileNodeAncestor.rbegin();
//			for (; it != TileNodeAncestor.rend(); ++it)
//			{
//				if (LastAncestorUID == *it)
//					break;
//				if (*it != 0 && getLoadCostByUID(*it).LoadCost == 0)
//				{
//					NotLoadDrawUIDSet.insert(*it);
//					LastAncestorUID = *it;
//					break;
//				}
//			}
//		}
//	}
//
//	__removeDrawUIDSetDuplication(vNeedShowAncestorTileNodeSet, NotLoadDrawUIDSet);
//
//	////process need load preferred tile node to show
//	for (unsigned int i = 1; i < static_cast<unsigned int>(vNeedLoadItemSet.size()); ++i)
//		vNeedLoadItemSet[0].TileNodeSet.insert(vNeedLoadItemSet[0].TileNodeSet.end(), vNeedLoadItemSet[i].TileNodeSet.begin(), vNeedLoadItemSet[i].TileNodeSet.end());
//
//	uintmax_t LoadSize = 0;
//	std::set<std::string> TexNameSet;
//	std::vector<unsigned int> NeedLoadDrawUIDSetForOneFrame;
//	std::vector<std::shared_ptr<SLoadTask>> NeedLoadTaskSetForOneFrame;
//	
//	unsigned int LastAncestorUID = INT_MAX;
//	for (auto& TileNode : vNeedLoadItemSet[0].TileNodeSet)
//	{
//		if (LoadSize < m_LimitLoadPerFrame)
//		{
//			const auto& UID = TileNode->getUID();
//			const auto& LoadCost = getLoadCostByUID(UID);
//			const auto& TexName = TileNode->getTextureFileName();
//			NeedLoadDrawUIDSetForOneFrame.emplace_back(UID);
//			if (LoadCost.LoadCost != 0)
//			{
//				LoadSize += LoadCost.GeoSize;
//				if (!LoadCost.TexInMemory && TexNameSet.find(TexName) == TexNameSet.end())
//				{
//					LoadSize += LoadCost.TexSize;
//					TexNameSet.insert(TexName);
//				}
//				NeedLoadTaskSetForOneFrame.emplace_back(std::make_shared<SLoadTask>(TileNode, !LoadCost.TexInMemory));
//			}
//		}
//		else
//		{
//			auto& TileNodeAncestor = TileNode->getAncestorUIDSet();
//			auto it = TileNodeAncestor.rbegin();
//			for (; it != TileNodeAncestor.rend(); ++it)
//			{
//				if (LastAncestorUID == *it)
//					break;
//				if (*it != 0 && getLoadCostByUID(*it).LoadCost == 0)
//				{
//					DrawUIDSet.emplace_back(*it);
//					LastAncestorUID = *it;
//					break;
//				}
//			}
//		}
//	}
//}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__generateDrawUIDSetAndLoadTaskSetForKnapsack(std::vector<SKnapsackItem>& vNeedLoadItemSet, std::vector<std::vector<std::shared_ptr<CTileNode>>>& vNeedShowAncestorTileNodeSet)
{
	////process not load preferred tile node to show
	std::set<unsigned int >DrawUIDSet;
	////find ancestor to show
	for (auto& TileNodeSet : vNeedShowAncestorTileNodeSet)
	{
		unsigned int LastAncestorUID = INT_MAX;
		for (auto& TileNode : TileNodeSet)
		{
			auto& TileNodeAncestor = TileNode->getAncestorUIDSet();
			auto it = TileNodeAncestor.rbegin();
			for (; it != TileNodeAncestor.rend(); ++it)
			{
				if (LastAncestorUID == *it)
					break;
				if (*it != 0 && getLoadCostByUID(*it).LoadCost == 0)
				{
					DrawUIDSet.insert(*it);
					LastAncestorUID = *it;
					break;
				}
			}
		}
	}

	auto CopyEveryFrameNotLoadDrawUIDSet = DrawUIDSet;

	////process need load preferred tile node to show
	for (unsigned int i = 1; i < static_cast<unsigned int>(vNeedLoadItemSet.size()); ++i)
		vNeedLoadItemSet[0].TileNodeSet.insert(vNeedLoadItemSet[0].TileNodeSet.end(), vNeedLoadItemSet[i].TileNodeSet.begin(), vNeedLoadItemSet[i].TileNodeSet.end());

	uintmax_t LoadSize = 0;
	std::set<std::string> TexNameSet;
	std::vector<std::shared_ptr<SLoadTask>> NeedLoadTaskSetForOneFrame;

	unsigned int LastAncestorUID = INT_MAX;
	for (auto& TileNode : vNeedLoadItemSet[0].TileNodeSet)
	{
		if (LoadSize < m_LimitLoadPerFrame)
		{
			const auto& UID = TileNode->getUID();
			const auto& LoadCost = getLoadCostByUID(UID);
			const auto& TexName = TileNode->getTextureFileName();
			CopyEveryFrameNotLoadDrawUIDSet.insert(UID);
			if (LoadCost.LoadCost != 0)
			{
				LoadSize += LoadCost.GeoSize;
				if (!LoadCost.TexInMemory && TexNameSet.find(TexName) == TexNameSet.end())
				{
					LoadSize += LoadCost.TexSize;
					TexNameSet.insert(TexName);
				}
				NeedLoadTaskSetForOneFrame.emplace_back(std::make_shared<SLoadTask>(TileNode, !LoadCost.TexInMemory));
			}
		}
		else
		{
			auto& TileNodeAncestor = TileNode->getAncestorUIDSet();
			auto it = TileNodeAncestor.rbegin();
			for (; it != TileNodeAncestor.rend(); ++it)
			{
				if (LastAncestorUID == *it)
					break;
				if (*it != 0 && getLoadCostByUID(*it).LoadCost == 0)
				{
					CopyEveryFrameNotLoadDrawUIDSet.insert(*it);
					LastAncestorUID = *it;
					break;
				}
			}
		}
	}

	//__removeDrawUIDSetDuplication(vNeedShowAncestorTileNodeSet, CopyEveryFrameNotLoadDrawUIDSet);

	std::vector<unsigned int> t;
	for (auto& i : CopyEveryFrameNotLoadDrawUIDSet)
		t.emplace_back(i);

	m_ThisFrameResult = SRenderingGeneratorResult(NeedLoadTaskSetForOneFrame, t);
}


//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__removeDrawUIDSetDuplication(std::vector<std::shared_ptr<CTileNode>>& voTileNodeSet, std::set<unsigned int>& voDrawUIDSet)
{
	auto Size = static_cast<int>(voTileNodeSet.size());
	for (int i = 0; i < Size; ++i)
	{
		auto& AncestorSet = voTileNodeSet[i]->getAncestorUIDSet();
		for (auto& AncestorTileNodeUID : AncestorSet)
		{
			if (AncestorTileNodeUID == 0)
				break;
			//question:有祖先則刪除
			if (voDrawUIDSet.find(AncestorTileNodeUID) != voDrawUIDSet.end())
			{
				voTileNodeSet.erase(voTileNodeSet.begin() + i);
				--i;
				Size = static_cast<int>(voTileNodeSet.size());
				break;
			}
		}
	}

	voDrawUIDSet.clear();
	for (auto& i : voTileNodeSet)
		voDrawUIDSet.insert(i->getUID());
}