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
const STileNodeLoadCost& CRenderingTileNodeGenerator::getLoadCostByUID(unsigned int vUID) const
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
					m_ThisFrameResult.FrameID = static_cast<unsigned int>(m_FrameID);
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
				m_ThisFrameResult.DrawUIDSet.clear();
				__showAncestor(m_PreferredResult.PreferredTileNodeSet);
				m_pOutputPipelineToTileNodeLoader->tryPush(std::make_shared<SRenderingGeneratorResult>(m_ThisFrameResult));
				m_LastFrameDrawUIDSet = m_ThisFrameResult.DrawUIDSet;
			}
		}
	}
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__showAncestor(const std::vector<std::vector<std::shared_ptr<CTileNode>>>& vTileNodeSet)
{
	if (vTileNodeSet.empty()) return;

	const std::vector<std::shared_ptr<SLoadTask>> EmptyLoadTaskSet;
	std::set<unsigned int> DrawUIDSet;
	for (auto& TileNodeSet : vTileNodeSet)
	{
		for (auto& TileNode : TileNodeSet)
		{
			if (getLoadCostByUID(TileNode->getUID()).LoadCost == 0)
				DrawUIDSet.insert(TileNode->getUID());
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

	//FIXME:这里似乎并没有进行处理,DrawUIDVector中可能有重复项
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
			std::vector<std::vector<std::shared_ptr<CTileNode>>> TempTileNodeSet;
			TempTileNodeSet.emplace_back(m_ShowAncestorTileNodeSet);
			__showAncestor(TempTileNodeSet);
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
				//FIXME:新增的加载任务的节点ID没有更新到绘制ID向量中
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
	for (auto& OneTileItemSet : ItemSet)
	{
		for (auto& OneItem : OneTileItemSet)
		{
			__calculateCostAndTriangle(OneItem.TileNodeSet, OneItem.LoadCostForKnapsackItem, OneItem.TriangleCount);
			OneItem.LoadCostForKnapsackItem = OneItem.LoadCostForKnapsackItem / KILOBYTE == 0 ? 1 : OneItem.LoadCostForKnapsackItem / KILOBYTE;
		}
	}

	std::vector<unsigned int> NotLoadTileNumSet;
	const auto LoadLimitSizeKB = m_LimitLoadPerSolve / KILOBYTE;
	std::vector<SKnapsackItem> SelectedItemSet;
	m_pKPSolver->solveMultipleChoiceKnapsackProblem(ItemSet, LoadLimitSizeKB, NotLoadTileNumSet, SelectedItemSet);

	CTimer::getInstance()->tock(__FUNCTION__);
	if (CTimer::getInstance()->needOutput() && CTimer::getInstance()->isRegistered(__FUNCTION__))
		std::cout << __FUNCTION__ << "  " << CTimer::getInstance()->getCostTimeByName(__FUNCTION__) << std::endl;

	m_SelectedTileNodeSet.clear();
	for (auto& item : SelectedItemSet)
		for (auto& TileNode : item.TileNodeSet)
			m_SelectedTileNodeSet.emplace_back(TileNode);

	m_ShowAncestorTileNodeSet.clear();
	for (auto& NotLoadTileNum : NotLoadTileNumSet)
		for (auto& TileNode : m_PreferredResult.PreferredTileNodeSet[NotLoadTileNum])
			m_ShowAncestorTileNodeSet.emplace_back(TileNode);

	//experiment record
	uintmax_t KnapsackCost = 0;
	unsigned int KnapsackNeedLoadFileNum = 0;
	__calculateCostAndNeedLoadFileNum(m_SelectedTileNodeSet, KnapsackCost, KnapsackNeedLoadFileNum);

	std::vector<std::shared_ptr<CTileNode>> PreferredTileNodeSet;
	for (auto& TempPreferredTileNodeSet: m_PreferredResult.PreferredTileNodeSet)
		for (auto& PreferredTileNode: TempPreferredTileNodeSet)
			PreferredTileNodeSet.emplace_back(PreferredTileNode);

	uintmax_t PreferedCost = 0;
	unsigned int PreferedNeedLoadFileNum = 0;
	__calculateCostAndNeedLoadFileNum(PreferredTileNodeSet, PreferedCost, PreferedNeedLoadFileNum);

	std::vector<int> TempKnapsackResult = { static_cast<int>(KnapsackCost), static_cast<int>(KnapsackNeedLoadFileNum), static_cast<int>(PreferedCost), static_cast<int>(PreferedNeedLoadFileNum) };
	m_AllKnapsackResult.emplace_back(TempKnapsackResult);
	//experiment record
}

//****************************************************************************
//FUNCTION:
void CRenderingTileNodeGenerator::__generateByLoadAncestor()
{
	//FIXME:这个函数存在的意义？
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
			const auto& TempTileNode = TileNode->getParent().lock();
			if (TempTileNode)
			{
				AncestorTileNodeSet.insert(TempTileNode);
				AncestorUIDSet.insert(TempTileNode->getUID());
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

//****************************************************************************
//FUNCTION:for experiment
void hivePagedLOD::CRenderingTileNodeGenerator::__calculateCostAndNeedLoadFileNum(const std::vector<std::shared_ptr<CTileNode>>& vTileNodeSet, uintmax_t & voCost, unsigned int & voFileNum)
{
	//FIXME:与__addTileNodeToLoadTaskSet有重复代码
	_ASSERT(!vTileNodeSet.empty());
	voCost = 0;
	voFileNum = 0;

	std::set<std::string> TexNameSet;
	for (auto& TileNode : vTileNodeSet)
	{
		const auto& UID = TileNode->getUID();
		const auto& LoadCost = getLoadCostByUID(UID);
		const auto& TexName = TileNode->getTextureFileName();
		if (LoadCost.LoadCost != 0)
		{
			voCost += LoadCost.GeoSize;
			++voFileNum;
			if (!LoadCost.TexInMemory && TexNameSet.find(TexName) == TexNameSet.end())
			{
				voCost += LoadCost.TexSize;
				++voFileNum;
				TexNameSet.insert(TexName);
			}
		}
	}
}