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
	m_LoadStrategy = ELoadStrategy::KNAPSACK;
	if (vStrategy == LOAD_STRATEGY_KNAPSACK)
		m_UseKP = true;
	m_LimitLoadPerFrame = vLimitSize;
	m_LoadParentMaxLevel = vLevel;
}

//****************************************************************************
//FUNCTION:
const STileNodeLoadCost& CRenderingTileNodeGenerator::getLoadCostByUID(unsigned vUID) const
{
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
				////视点不变化，加载根据策略生成的集合中的节点，全部加载完成后再显示
				if (!m_LoadFinish)
				{
					__generateResult();
					//
					/*int RealTimeLoadTaskNum = m_ThisFrameResult.LoadTaskSet.size();
					int RealTimeNeedDrawTileNodeNum = m_ThisFrameResult.DrawUIDSet.size();
					int PreferedTileNodeNum = 0;
					for (auto i = 0; i < m_PreferredResult.PreferredTileNodeSet.size(); ++i)
					{
						if (!m_PreferredResult.PreferredTileNodeSet[i].empty())
							for (auto k = 0; k < m_PreferredResult.PreferredTileNodeSet[i].size(); ++k)
								++PreferedTileNodeNum;
					}
					int SelectedTileNodeNum = m_SelectedTileNodeSet.size();
					int ShowAncestorTileNodeNum = m_ShowAncestorTileNodeSet.size();

					std::vector<int> Info = { static_cast<int>(m_FrameID), m_ViewPortNum,  PreferedTileNodeNum, RealTimeLoadTaskNum, RealTimeNeedDrawTileNodeNum,
					SelectedTileNodeNum, ShowAncestorTileNodeNum };

					const std::string LoadTaskInfo = "0419LoadTaskInfo.csv";

					std::ofstream OutputDataStream;
					OutputDataStream.open(LoadTaskInfo, std::ios::out | std::ios::app);
					if (!OutputDataStream.is_open())
						throw std::ios_base::failure("The file can't open!");

					for (auto Iterator = Info.begin(); Iterator != Info.end(); ++Iterator)
					{
						if (Iterator == Info.begin())
						{
							OutputDataStream << *Iterator;
							continue;
						}
						OutputDataStream << "," << *Iterator;
					}
					OutputDataStream << std::endl;
					OutputDataStream.close();*/
					//
					m_ThisFrameResult.FrameID = m_FrameID;
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
				++m_ViewPortNum;
				m_PreferredResult = PreferredResult;

				uintmax_t TotalCost = 0, TotalTriangle = 0;
				for (auto& TileNodeSet : m_PreferredResult.PreferredTileNodeSet)
				{
					uintmax_t Cost = 0, Triangle = 0;
					__calculateCostAndTriangle(TileNodeSet, Cost, Triangle);
					TotalCost += Cost;
					TotalTriangle += Triangle;
				}

				if (TotalCost > m_LimitLoadPerSolve)
				{
					if (m_UseKP)
						__generateByKnapsack();
					else
						__generateByLoadAncestor();
				}

				m_LoadFinish = false;

				//不考虑非法情况
				/*m_ThisFrameResult.DrawUIDSet.clear();
				__showAncestor(m_PreferredResult.PreferredTileNodeSet);
				m_pOutputPipelineToTileNodeLoader->tryPush(std::make_shared<SRenderingGeneratorResult>(m_ThisFrameResult));
				m_LastFrameDrawUIDSet = m_ThisFrameResult.DrawUIDSet;*/
				__generateResult();
				m_ThisFrameResult.FrameID = m_FrameID;
				m_pOutputPipelineToTileNodeLoader->tryPush(std::make_shared<SRenderingGeneratorResult>(m_ThisFrameResult));
			}
		}
	}
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__showAncestor(const std::vector<std::vector<std::shared_ptr<CTileNode>>>& vTileNodeSet)
{
	const std::vector<std::shared_ptr<SLoadTask>> EmptyLoadTaskSet;
	std::set<unsigned int> DrawUIDSet;
	for (auto& TileNodeSet : vTileNodeSet)
	{
		for (auto& TileNode : TileNodeSet)
		{
			if (getLoadCostByUID(TileNode->getUID()).LoadCost == 0)
			{
				DrawUIDSet.insert(TileNode->getUID());
			}
			else
			{
				auto& AncestorUIDSet = TileNode->getAncestorUIDSet();
				for (auto it = AncestorUIDSet.rbegin(); it != AncestorUIDSet.rend(); ++it)
				{
					if (*it != 0 && getLoadCostByUID(*it).LoadCost == 0)
					{
						DrawUIDSet.insert(*it);
						break;
					}
				}
			}
		}
	}

	std::vector<unsigned int> DrawUIDVector;
	for (auto& i : DrawUIDSet)
		DrawUIDVector.emplace_back(i);

	m_ThisFrameResult = SRenderingGeneratorResult(EmptyLoadTaskSet, DrawUIDVector);
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__generateResult()
{
	std::vector<std::shared_ptr<SLoadTask>> LoadTaskSet;
	std::vector<unsigned int> DrawUIDSet;

	//middle result or preferred result
	const bool MiddleResultFlag = !m_SelectedTileNodeSet.empty();

	uintmax_t LoadSize = 0;
	if (MiddleResultFlag)//middle result
	{
		__addTileNodeToLoadTaskSet(m_SelectedTileNodeSet, LoadTaskSet, LoadSize);
		m_ThisFrameResult = SRenderingGeneratorResult(LoadTaskSet, m_LastFrameDrawUIDSet);
		m_ThisFrameResult.IsKnapsackResult = true;
		//std::cout << "KNAP:" << m_ThisFrameResult.IsKnapsackResult << std::endl;
	}
	else//preferred result
	{
		for (auto& i : m_PreferredResult.PreferredTileNodeSet)
		{
			if (LoadSize >= m_LimitLoadPerFrame)
				break;
			__addTileNodeToLoadTaskSet(i, LoadTaskSet, LoadSize);
		}
		m_ThisFrameResult = SRenderingGeneratorResult(LoadTaskSet, m_LastFrameDrawUIDSet);
		m_ThisFrameResult.IsKnapsackResult = false;
		m_ThisFrameResult.IsKnapsackEnd = false;
	}

	//load TileNode finish, show result
	if (LoadTaskSet.empty())
	{
		if (MiddleResultFlag)
		{
			for (auto& TileNode : m_SelectedTileNodeSet)
				DrawUIDSet.emplace_back(TileNode->getUID());
			m_SelectedTileNodeSet.clear();

			m_ThisFrameResult.DrawUIDSet.clear();
			//show ancestor
			std::vector<std::vector<std::shared_ptr<CTileNode>>> t;
			t.emplace_back(m_ShowAncestorTileNodeSet);
			__showAncestor(t);
			//selected
			m_ThisFrameResult.DrawUIDSet.insert(m_ThisFrameResult.DrawUIDSet.end(), DrawUIDSet.begin(), DrawUIDSet.end());
			m_ShowAncestorTileNodeSet.clear();
			m_ThisFrameResult.IsKnapsackEnd = true;
		}
		else
		{
			for (auto& TileNodeSet : m_PreferredResult.PreferredTileNodeSet)
				for (auto& TileNode : TileNodeSet)
					DrawUIDSet.emplace_back(TileNode->getUID());
			m_LoadFinish = true;
			m_ThisFrameResult = SRenderingGeneratorResult(LoadTaskSet, DrawUIDSet);
		}
		m_LastFrameDrawUIDSet = m_ThisFrameResult.DrawUIDSet;
	}
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__addTileNodeToLoadTaskSet(const std::vector<std::shared_ptr<CTileNode>>& vTileNodeSet, std::vector<std::shared_ptr<SLoadTask>>& voLoadTaskSet, uintmax_t& voLoadSize)
{
	std::set<std::string> TexNameSet;
	for (auto& TileNode : vTileNodeSet)
	{
		if (voLoadSize < m_LimitLoadPerFrame)
		{
			const auto& UID = TileNode->getUID();
			const auto& LoadCost = getLoadCostByUID(UID);
			const auto& TexName = TileNode->getTextureFileName();
			if (LoadCost.LoadCost != 0)
			{
				voLoadSize += LoadCost.GeoSize;
				if (!LoadCost.TexInMemory && TexNameSet.find(TexName) == TexNameSet.end())
				{
					voLoadSize += LoadCost.TexSize;
					TexNameSet.insert(TexName);
				}
				voLoadTaskSet.emplace_back(std::make_shared<SLoadTask>(TileNode, !LoadCost.TexInMemory));
				//新增的加载任务的节点ID没有更新到绘制ID向量中
				m_LastFrameDrawUIDSet.emplace_back(UID);
			}
		}
		else
		{
			break;
		}
	}
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
	{
		voCost += i.second;
	}
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__generateByKnapsack()
{
	CTimer::getInstance()->tick(__FUNCTION__);

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
	const auto LoadSizeMB = m_LimitLoadPerSolve / KILOBYTE;
	std::vector<SKnapsackItem> SelectedItemSet;
	m_pKPSolver->solveMultipleChoiceKnapsackProblem(ItemSet, LoadSizeMB, NotLoadTileNumSet, SelectedItemSet);

	CTimer::getInstance()->tock(__FUNCTION__);
	if (CTimer::getInstance()->needOutput() && CTimer::getInstance()->isRegistered(__FUNCTION__))
		std::cout << __FUNCTION__ << "  " << CTimer::getInstance()->getCostTimeByName(__FUNCTION__) << std::endl;

	m_SelectedTileNodeSet.clear();
	for (auto& item : SelectedItemSet)
		for (auto& TileNode : item.TileNodeSet)
			m_SelectedTileNodeSet.emplace_back(TileNode);

	m_ShowAncestorTileNodeSet.clear();
	for (auto& i : NotLoadTileNumSet)
		for (auto& TileNode : m_PreferredResult.PreferredTileNodeSet[i])
			m_ShowAncestorTileNodeSet.emplace_back(TileNode);

	int TempCost = 0;
	int FileNumNeededLoad = 0;
	std::set<std::string> TexNameSet;
	for (auto& TileNode : m_SelectedTileNodeSet)
	{
		const auto& UID = TileNode->getUID();
		const auto& LoadCost = getLoadCostByUID(UID);
		const auto& TexName = TileNode->getTextureFileName();
		if (LoadCost.LoadCost != 0)
		{
			TempCost += LoadCost.GeoSize;
			++FileNumNeededLoad;
			if (!LoadCost.TexInMemory && TexNameSet.find(TexName) == TexNameSet.end())
			{
				TempCost += LoadCost.TexSize;
				++FileNumNeededLoad;
				TexNameSet.insert(TexName);
			}
		}
	}

	std::vector<int> TempKnapsackResult = { TempCost, FileNumNeededLoad };
	m_AllKnapsackResult.push_back(TempKnapsackResult);

	/*uintmax_t TempCost1 = 0;
	uintmax_t TriangleCount1 = 0;
	int NeedLoadFileNum = 0;
	__calculateCostAndTriangle1(m_SelectedTileNodeSet, TempCost1, TriangleCount1, NeedLoadFileNum);*/
	//add
	//从ItemSet的每个Tile中选择一个与SelectedItemSet中数据量大小最接近的替换SelectedItemSet中对应位置
	/*for (auto i = 0; i < SelectedItemSet.size(); ++i)
	{*/
	//int TempLoadLeastGap = INT_MAX;
	//int TempSubstituteIndex = INT_MAX;
	//for (auto k = 0; k < ItemSet[0].size(); ++k)
	//{
	//	int TempGap = std::abs(static_cast<int>(SelectedItemSet[0].LoadCostForKnapsackItem - ItemSet[0][k].LoadCostForKnapsackItem));
	//	if (TempGap < TempLoadLeastGap && TempGap > 0)
	//	{
	//		TempLoadLeastGap = std::abs(static_cast<int>(SelectedItemSet[0].LoadCostForKnapsackItem - ItemSet[0][k].LoadCostForKnapsackItem));
	//		TempSubstituteIndex = k;
	//	}
	//}
	//if (TempSubstituteIndex == INT_MAX)
	//	TempSubstituteIndex = 0;
	//assert(TempSubstituteIndex < ItemSet[0].size());
	//SelectedItemSet[0] = ItemSet[0][TempSubstituteIndex];

	///*}*/
	//m_SelectedTileNodeSetTemp.clear();
	//for (auto& item : SelectedItemSet)
	//	for (auto& TileNode : item.TileNodeSet)
	//		m_SelectedTileNodeSetTemp.emplace_back(TileNode);
	//从ItemSet的每个Tile中选择一个与SelectedItemSet中数据量大小最接近的替换SelectedItemSet中对应位置

	//统计可见Tile数量和期望节点数量
	/*int PrederedTileNum = 0;
	int PreferedTileNodeNum = 0;
	for (auto i = 0; i < m_PreferredResult.PreferredTileNodeSet.size(); ++i)
	{
		if (!m_PreferredResult.PreferredTileNodeSet[i].empty())
		{
			++PrederedTileNum;
			for (auto k = 0; k < m_PreferredResult.PreferredTileNodeSet[i].size(); ++k)
				++PreferedTileNodeNum;
		}
	}
	*/

	/*const std::string InfoSetOutputPath = "0424KnapsackSolveResult.csv";*/
	/*std::vector<int> InfoSet = { m_ViewPortNum, static_cast<int>(m_FrameID), PrederedTileNum, PreferedTileNodeNum, 
		static_cast<int>(ItemSet.size()),
		static_cast<int>(SelectedItemSet.size()), static_cast<int>(NotLoadTileNumSet.size()),
		static_cast<int>(m_SelectedTileNodeSet.size()), static_cast<int>(m_ShowAncestorTileNodeSet.size()) };*/
	//std::vector<int> InfoSet = { static_cast<int>(m_FrameID), TempCost, /*static_cast<int>(TempCost1), NeedLoadFileNum,*/ FileNumNeededLoad};
	//std::ofstream OutputDataStream;
	//OutputDataStream.open(InfoSetOutputPath, std::ios::out | std::ios::app);
	//if (!OutputDataStream.is_open())
	//	throw std::ios_base::failure("The file can't open!");

	//for (auto Iterator = InfoSet.begin(); Iterator != InfoSet.end(); ++Iterator)
	//{
	//	if (Iterator == InfoSet.begin())
	//	{
	//		OutputDataStream << *Iterator;
	//		continue;
	//	}
	//	OutputDataStream << "," << *Iterator;
	//}
	//OutputDataStream << "\n";
	//OutputDataStream.close();
	//
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__generateByLoadAncestor()
{
	CTimer::getInstance()->tick(__FUNCTION__);

	std::set<std::shared_ptr<CTileNode>> AncestorTileNodeSet;
	std::vector<std::shared_ptr<CTileNode>> AncestorTileNodeVector;
	std::set<unsigned int> AncestorUIDSet;

	uintmax_t LoadSize = 0;
	uintmax_t TriangleCount = 0;

	for (auto& i : m_PreferredResult.PreferredTileNodeSet)
		AncestorTileNodeVector.insert(AncestorTileNodeVector.end(), i.begin(), i.end());

	do
	{
		AncestorTileNodeSet.clear();
		AncestorUIDSet.clear();
		for (auto& TileNode : AncestorTileNodeVector)
		{
			if (TileNode->getParent().lock())
			{
				AncestorTileNodeSet.insert(TileNode->getParent().lock());
				AncestorUIDSet.insert(TileNode->getParent().lock()->getUID());
			}
		}

		AncestorTileNodeVector.clear();
		for (auto& i : AncestorTileNodeSet)
			AncestorTileNodeVector.emplace_back(i);
		if (m_RemoveDuplication)
			__removeDrawUIDSetDuplication(AncestorTileNodeVector, AncestorUIDSet);

		LoadSize = 0;
		TriangleCount = 0;
		__calculateCostAndTriangle(AncestorTileNodeVector, LoadSize, TriangleCount);
	} while (LoadSize > m_LimitLoadPerSolve);

	CTimer::getInstance()->tock(__FUNCTION__);

	m_ShowAncestorTileNodeSet.clear();

	m_SelectedTileNodeSet.clear();
	m_SelectedTileNodeSet = AncestorTileNodeVector;
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

////****************************************************************************
////FUNCTION:
//void CRenderingTileNodeGenerator::__generateResultForKnapsackInOneFrame()
//{
//	std::vector<std::shared_ptr<SLoadTask>> LoadTaskSet;
//	std::set<unsigned int> FinishProcessTileNodeUIDSet;
//	std::set<unsigned int> DrawUIDSet;
//	uintmax_t LoadSize = 0;
//	std::set<std::string> TexNameSet;
//	std::vector<std::shared_ptr<CTileNode>> DrawTileNodeSet;
//
//	////process selected item set
//	unsigned int LastAncestorUID = INT_MAX;
//	for (auto& TileNode : m_SelectedTileNodeSet)
//	{
//		const auto& UID = TileNode->getUID();
//		if (FinishProcessTileNodeUIDSet.find(UID) != FinishProcessTileNodeUIDSet.end())
//		{
//			DrawUIDSet.insert(UID);
//			continue;
//		}
//
//		if (LoadSize < m_LimitLoadPerFrame)
//		{
//			const auto& LoadCost = getLoadCostByUID(UID);
//			const auto& TexName = TileNode->getTextureFileName();
//			DrawUIDSet.insert(UID);
//			DrawTileNodeSet.emplace_back(TileNode);
//			FinishProcessTileNodeUIDSet.insert(UID);
//			if (LoadCost.LoadCost != 0)
//			{
//				LoadSize += LoadCost.GeoSize;
//				if (!LoadCost.TexInMemory && TexNameSet.find(TexName) == TexNameSet.end())
//				{
//					LoadSize += LoadCost.TexSize;
//					TexNameSet.insert(TexName);
//				}
//				LoadTaskSet.emplace_back(std::make_shared<SLoadTask>(TileNode, !LoadCost.TexInMemory));
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
//					DrawUIDSet.insert(*it);
//					DrawTileNodeSet.emplace_back(CScene::getInstance()->getTileNodePointerByUID(*it));
//					FinishProcessTileNodeUIDSet.insert(*it);
//					LastAncestorUID = *it;
//					break;
//				}
//			}
//		}
//	}
//
//	//// process show ancestor set
//	LastAncestorUID = INT_MAX;
//	for (auto& TileNode : m_ShowAncestorTileNodeSet)
//	{
//		const auto& UID = TileNode->getUID();
//		if (FinishProcessTileNodeUIDSet.find(UID) != FinishProcessTileNodeUIDSet.end())
//		{
//			DrawUIDSet.insert(UID);
//			continue;
//		}
//		//// has no load task
//		const auto& LoadCost = getLoadCostByUID(UID);
//		if (LoadCost.LoadCost == 0)
//		{
//			DrawUIDSet.insert(UID);
//			DrawTileNodeSet.emplace_back(TileNode);
//			FinishProcessTileNodeUIDSet.insert(UID);
//			continue;
//		}
//
//		auto& TileNodeAncestor = TileNode->getAncestorUIDSet();
//		auto it = TileNodeAncestor.rbegin();
//		for (; it != TileNodeAncestor.rend(); ++it)
//		{
//			if (LastAncestorUID == *it)
//				break;
//			if (*it != 0 && getLoadCostByUID(*it).LoadCost == 0)
//			{
//				DrawUIDSet.insert(*it);
//				DrawTileNodeSet.emplace_back(CScene::getInstance()->getTileNodePointerByUID(*it));
//				FinishProcessTileNodeUIDSet.insert(*it);
//				LastAncestorUID = *it;
//				break;
//			}
//		}
//	}
//
//	if (m_RemoveDuplication)
//		__removeDrawUIDSetDuplication(DrawTileNodeSet, DrawUIDSet);
//
//	std::vector<unsigned int> ResultDrawUIDSet;
//	for (auto& i : DrawUIDSet)
//		ResultDrawUIDSet.emplace_back(i);
//
//	////load preferred
//	if (LoadTaskSet.empty())
//	{
//		ResultDrawUIDSet.clear();
//		for (auto& TileNodeSet : m_PreferredResult.PreferredTileNodeSet)
//		{
//			for (auto& TileNode : TileNodeSet)
//			{
//				if (LoadSize < m_LimitLoadPerFrame)
//				{
//					const auto& UID = TileNode->getUID();
//					const auto& LoadCost = getLoadCostByUID(UID);
//					const auto& TexName = TileNode->getTextureFileName();
//					if (LoadCost.LoadCost != 0)
//					{
//						LoadSize += LoadCost.GeoSize;
//						if (!LoadCost.TexInMemory && TexNameSet.find(TexName) == TexNameSet.end())
//						{
//							LoadSize += LoadCost.TexSize;
//							TexNameSet.insert(TexName);
//						}
//						LoadTaskSet.emplace_back(std::make_shared<SLoadTask>(TileNode, !LoadCost.TexInMemory));
//					}
//					ResultDrawUIDSet.emplace_back(UID);
//				}
//				else
//				{
//					auto& AncestorUIDSet = TileNode->getAncestorUIDSet();
//					for (auto it = AncestorUIDSet.rbegin(); it != AncestorUIDSet.rend(); ++it)
//					{
//						if (*it != 0 && getLoadCostByUID(*it).LoadCost == 0)
//						{
//							ResultDrawUIDSet.emplace_back(*it);
//							break;
//						}
//					}
//				}
//			}
//		}
//	}
//
//	////finish load preferred
//	if (LoadTaskSet.empty())
//	{
//		ResultDrawUIDSet.clear();
//		for (auto& TileNodeSet : m_PreferredResult.PreferredTileNodeSet)
//			for (auto& TileNode : TileNodeSet)
//				ResultDrawUIDSet.emplace_back(TileNode->getUID());
//		m_LoadFinish = true;
//	}
//
//	m_ThisFrameResult = SRenderingGeneratorResult(LoadTaskSet, ResultDrawUIDSet);
//}

////****************************************************************************
////FUNCTION:
//void CRenderingTileNodeGenerator::__generateResultForDirectLoadInOneFrame()
//{
//	std::vector<std::shared_ptr<SLoadTask>> LoadTaskSet;
//	std::set<unsigned int> FinishProcessTileNodeUIDSet;
//
//	std::set<unsigned int> DrawUIDSet;
//	uintmax_t LoadSize = 0;
//	std::set<std::string> TexNameSet;
//	std::vector<std::shared_ptr<CTileNode>> DrawTileNodeSet;
//
//	for (auto& TileNodeSet : m_PreferredResult.PreferredTileNodeSet)
//	{
//		unsigned int LastAncestorUID = INT_MAX;
//		for (auto& TileNode : TileNodeSet)
//		{
//			const auto& UID = TileNode->getUID();
//			if (FinishProcessTileNodeUIDSet.find(UID) != FinishProcessTileNodeUIDSet.end())
//			{
//				DrawUIDSet.insert(UID);
//				continue;
//			}
//
//			if (LoadSize < m_LimitLoadPerFrame)
//			{
//				const auto& LoadCost = getLoadCostByUID(UID);
//				const auto& TexName = TileNode->getTextureFileName();
//				DrawUIDSet.insert(UID);
//				DrawTileNodeSet.emplace_back(TileNode);
//				FinishProcessTileNodeUIDSet.insert(UID);
//				if (LoadCost.LoadCost != 0)
//				{
//					LoadSize += LoadCost.GeoSize;
//					if (!LoadCost.TexInMemory && TexNameSet.find(TexName) == TexNameSet.end())
//					{
//						LoadSize += LoadCost.TexSize;
//						TexNameSet.insert(TexName);
//					}
//					LoadTaskSet.emplace_back(std::make_shared<SLoadTask>(TileNode, !LoadCost.TexInMemory));
//				}
//			}
//			else
//			{
//				auto& TileNodeAncestor = TileNode->getAncestorUIDSet();
//				auto it = TileNodeAncestor.rbegin();
//				for (; it != TileNodeAncestor.rend(); ++it)
//				{
//					if (LastAncestorUID == *it)
//						break;
//					if (*it != 0 && getLoadCostByUID(*it).LoadCost == 0)
//					{
//						DrawUIDSet.insert(*it);
//						DrawTileNodeSet.emplace_back(CScene::getInstance()->getTileNodePointerByUID(*it));
//						FinishProcessTileNodeUIDSet.insert(*it);
//						LastAncestorUID = *it;
//						break;
//					}
//				}
//			}
//		}
//	}
//
//	if (m_RemoveDuplication)
//		__removeDrawUIDSetDuplication(DrawTileNodeSet, DrawUIDSet);
//
//	std::vector<unsigned int> t;
//	for (auto& i : DrawUIDSet)
//		t.emplace_back(i);
//
//	if (LoadTaskSet.empty())
//		m_LoadFinish = true;
//
//	m_ThisFrameResult = SRenderingGeneratorResult(LoadTaskSet, t);
//}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__calculateCostAndTriangle1(const std::vector<std::shared_ptr<CTileNode>>& TileNodeSet, uintmax_t& voCost, uintmax_t& voTriangle, int& voLoadFileNum)
{
	voCost = 0;
	voTriangle = 0;
	voLoadFileNum = 0;
	std::map<std::string, uintmax_t> freq;
	for (auto& Tile : TileNodeSet)
	{
		auto& LoadCost = getLoadCostByUID(Tile->getUID());
		voTriangle += LoadCost.TriangleCount;
		if (LoadCost.TexInMemory)
		{
			if(!LoadCost.GeoInMemory)
				++voLoadFileNum;
			voCost += LoadCost.LoadCost;
		}
		else
		{
			voCost += LoadCost.GeoSize;
			++voLoadFileNum;
			freq[Tile->getTextureFileName()] = LoadCost.TexSize;
		}
	}
	for (auto& i : freq)
	{
		voCost += i.second;
		++voLoadFileNum;
	}
}