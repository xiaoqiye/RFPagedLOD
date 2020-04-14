#include "KnapsackProblemSolver.h"
#include "Common.h"

#include <vector>
#include <tbb/concurrent_unordered_map.h>
#include <set>

using namespace hivePagedLOD;

//****************************************************************************
//FUNCTION:
std::vector<std::vector<SKnapsackItem>> CKnapsackProblemSolver::prepareKnapsackItem(const SPreferredResult& vPreferredResult, const std::vector<std::vector<SKnapsackItem>>& vAllTemplateItemSet)
{
	std::vector<std::vector<SKnapsackItem>> ResultKnapsackItemSet;
	auto& PreferredTileNodeSet = vPreferredResult.PreferredTileNodeSet;
	for (unsigned int i = 0; i < static_cast<unsigned int>(PreferredTileNodeSet.size()); ++i)
	{
		if (PreferredTileNodeSet[i].empty())
			continue;
		ResultKnapsackItemSet.emplace_back(__generateOneTileKnapsackItemSet(PreferredTileNodeSet[i], vAllTemplateItemSet[i], vPreferredResult.MaxDeepSet[i]));
	}
	return ResultKnapsackItemSet;
}

//****************************************************************************
//FUNCTION:
void CKnapsackProblemSolver::solveMultipleChoiceKnapsackProblem(const std::vector<std::vector<SKnapsackItem>>& vAllTileItem, uintmax_t vLoadLimit, std::vector<unsigned int>& voNotLoadTileNumSet, std::vector<SKnapsackItem>& voResultItemSet)
{
	__generateKPResultItemSet(vAllTileItem, __fillDPTable(vAllTileItem, vLoadLimit), voNotLoadTileNumSet, voResultItemSet);
}

//****************************************************************************
//FUNCTION:
std::vector<SKnapsackItem> CKnapsackProblemSolver::__generateOneTileKnapsackItemSet(const std::vector<std::shared_ptr<CTileNode>>& vOneTilePreferredTileNodeSet, const std::vector<SKnapsackItem>& vOneTileTemplateSet, unsigned int vMaxDeep)
{
	auto CopyTemplateSet = vOneTileTemplateSet;

	unsigned int Index = 0;
	////delete not visible TileNode, not visible must be not match LOD
	for (auto& BaseNSet : CopyTemplateSet)
	{
		if (Index > vMaxDeep)
			break;

		std::set<std::shared_ptr<CTileNode>> WaitAddSet;
		std::set<std::shared_ptr<CTileNode>> WaitDeleteSet;
		for (auto it = BaseNSet.TileNodeSet.begin(); it != BaseNSet.TileNodeSet.end(); ++it)
		{
			auto& TileNode = (*it);
			if (WaitDeleteSet.find(TileNode) != WaitDeleteSet.end())
				continue;

			if (!TileNode->getStatus().IsVisible)////delete not visible TileNode
			{
				WaitDeleteSet.insert(TileNode);

				bool HasBrotherVisible = false;
				for (auto& i : TileNode->getBrotherSet())
				{
					if (i.lock()->getStatus().IsVisible)
					{
						HasBrotherVisible = true;
						break;
					}
				}
				if (!HasBrotherVisible)//// all brother not visible means LOD match ancestor node, find ancestor
				{
					auto Parent = TileNode->getParent().lock();
					while (Parent)
					{
						auto& Status = Parent->getStatus();
						if (Status.IsVisible && Status.MatchLOD)////2 of 3 possible
						{
							WaitAddSet.insert(Parent);
							break;
						}
						else if (!Status.MatchLOD)//// not visible or its ancestor match LOD
						{
							Parent = Parent->getParent().lock();
						}
					}

					for (auto& Brother : TileNode->getBrotherSet())
						WaitDeleteSet.insert(Brother.lock());
				}
			}
		}
		for (auto& i : WaitDeleteSet)
		{
			const auto UID = i->getUID();
			for (auto DelIt = BaseNSet.TileNodeSet.begin(); DelIt != BaseNSet.TileNodeSet.end(); ++DelIt)
			{
				if ((*DelIt)->getUID() == UID)
				{
					BaseNSet.TileNodeSet.erase(DelIt);
					break;
				}
			}
		}
		for (auto& i : WaitAddSet)
			BaseNSet.TileNodeSet.emplace_back(i);

		++Index;
	}

	CopyTemplateSet.erase(CopyTemplateSet.begin() + Index, CopyTemplateSet.end());

	return CopyTemplateSet;
}

//****************************************************************************
//FUNCTION:
std::vector<std::vector<SKPSolution>> CKnapsackProblemSolver::__fillDPTable(const std::vector<std::vector<SKnapsackItem>>& vAllTileItem, uintmax_t vLoadLimit)
{
	std::vector<std::vector<uintmax_t>> DPTable;
	std::vector<std::vector<SKPSolution>> Solution;
	__initDPTable(vAllTileItem, vLoadLimit, DPTable, Solution);

	for (unsigned int i = 1; i < vAllTileItem.size(); i++)
	{
		for (unsigned int k = 1; k <= static_cast<unsigned int>(vLoadLimit); k++)
		{
			DPTable[i][k] = DPTable[i - 1][k];
			for (unsigned int x = 1; x <= vAllTileItem[i].size(); x++)
			{
				if (k >= vAllTileItem[i][x - 1].LoadCostForKnapsackItem)
				{
					const auto Temp = vAllTileItem[i][x - 1].TriangleCount + DPTable[i - 1][k - vAllTileItem[i][x - 1].LoadCostForKnapsackItem];
					if (DPTable[i][k] <= Temp)
					{
						DPTable[i][k] = Temp;
						Solution[i][k] = SKPSolution(i - 1, static_cast<unsigned int>(k - vAllTileItem[i][x - 1].LoadCostForKnapsackItem), static_cast<unsigned int>(vAllTileItem[i][x - 1].TriangleCount));
					}
				}
			}
		}
	}
	//std::cout << "Knapsack Problem Result = " << DPTable[vAllTileItem.size() - 1][vLoadLimit] << std::endl;

	return Solution;
}

//****************************************************************************
//FUNCTION:
void CKnapsackProblemSolver::__initDPTable(const std::vector<std::vector<SKnapsackItem>>& vAllTileItem, uintmax_t vLoadLimit, std::vector<std::vector<uintmax_t>>& voDPTable, std::vector<std::vector<SKPSolution>>& voSolution)
{
	const auto GroupNum = static_cast<unsigned int>(vAllTileItem.size());
	voDPTable.resize(GroupNum);
	voSolution.resize(GroupNum);
	// init size
	for (unsigned int i = 0; i < GroupNum; ++i)
	{
		voDPTable[i].resize(vLoadLimit + 1);
		voSolution[i].resize(vLoadLimit + 1);
		for (unsigned int k = 0; k < voDPTable[i].size(); ++k)
		{
			voSolution[i][k] = SKPSolution(INT_MAX, INT_MAX, 0);
		}
	}
	// init first line
	uintmax_t LastTriangleCount = 0;
	for (unsigned int i = 0; i <= static_cast<unsigned int>(vLoadLimit); ++i)
	{
		if (i < static_cast<unsigned int>(vAllTileItem[0].size()))
		{
			uintmax_t MaxTriangleCount = 0;
			for (const auto& Item : vAllTileItem[0])
			{
				if (static_cast<unsigned int>(Item.LoadCostForKnapsackItem) <= i && Item.TriangleCount >= MaxTriangleCount)
				{
					MaxTriangleCount = Item.TriangleCount;
				}
			}
			voDPTable[0][i] = MaxTriangleCount;
			voSolution[0][i] = SKPSolution(0, 0, MaxTriangleCount);
			LastTriangleCount = MaxTriangleCount;
		}
		else
		{
			voDPTable[0][i] = LastTriangleCount;
			voSolution[0][i] = SKPSolution(0, 0, LastTriangleCount);
		}
	}
}

//****************************************************************************
//FUNCTION:
void CKnapsackProblemSolver::__generateKPResultItemSet(const std::vector<std::vector<SKnapsackItem>>& vAllTileItem, const std::vector<std::vector<SKPSolution>>& vSolution, std::vector<unsigned int>& voNotLoadTileNumSet, std::vector<SKnapsackItem>& voResultSet)
{
	auto x = static_cast<unsigned int>(vSolution.size() - 1);
	auto y = static_cast<unsigned int>(vSolution[x].size() - 1);
	auto Times = static_cast<int>(vSolution.size() - 1);

	while (Times >= 0)
	{
		if (vSolution[x][y].X == INT_MAX && vSolution[x][y].Y == INT_MAX)
		{
			voNotLoadTileNumSet.emplace_back(x);
			--x;
		}
		else
		{
			for (auto& Item : vAllTileItem[x])
			{
				if (Item.TriangleCount == vSolution[x][y].TriangleCount)
				{
					voResultSet.emplace_back(Item);
					break;
				}
			}
			const auto tx = vSolution[x][y].X;
			const auto ty = vSolution[x][y].Y;
			x = tx;
			y = ty;
		}
		--Times;
	}
}