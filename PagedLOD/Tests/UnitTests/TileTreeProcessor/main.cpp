#include "TileTreeProcessor.h"
#include "ConsoleLog.h"

struct SKnapsackItem
{
	std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> TileNodeSet;
	uintmax_t LoadCostForKnapsackItem = 0;
	uintmax_t TriangleCount = 0;
	SKnapsackItem() = default;
	SKnapsackItem(const std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& vTileNodeSet, uintmax_t vLoadCostForKnapsackItem, uintmax_t vTriangleCount)
		: TileNodeSet(vTileNodeSet), LoadCostForKnapsackItem(vLoadCostForKnapsackItem), TriangleCount(vTriangleCount) {};
};

struct SSolution
{
	unsigned int X;
	unsigned int Y;
	uintmax_t TriangleCount;
	SSolution() = default;
	SSolution(unsigned int vX, unsigned int vY, uintmax_t vTriangleCount) : X(vX), Y(vY), TriangleCount(vTriangleCount) {}
};

std::vector<std::vector<SSolution>> solveGroupKnapsack(const std::vector<std::vector<SKnapsackItem>>& vAllTileItem, uintmax_t vLoadLimit)
{
	std::vector<std::vector<uintmax_t>> voResult(vAllTileItem.size());
	std::vector<std::vector<SSolution>> voSolution(vAllTileItem.size());
	// init size
	for (unsigned int i = 0; i < voResult.size(); ++i)
	{
		voResult[i].resize(vLoadLimit + 1);
		voSolution[i].resize(vLoadLimit + 1);
		for (unsigned int k = 0; k < voResult[i].size(); ++k)
		{
			voSolution[i][k] = SSolution(INT_MAX, INT_MAX, 0);
		}
	}
	// init first line
	uintmax_t LastTriangleCount = 0;
	for (unsigned int i = 0; i <= static_cast<unsigned int>(vLoadLimit); i++)
	{
		if (i < vAllTileItem[0].size())
		{
			uintmax_t MaxTriangleCount = 0;
			for (unsigned int k = 0; k < vAllTileItem[0].size(); ++k)
			{
				if (vAllTileItem[0][k].LoadCostForKnapsackItem <= i && vAllTileItem[0][k].TriangleCount >= MaxTriangleCount)
				{
					MaxTriangleCount = vAllTileItem[0][k].TriangleCount;
				}
			}
			voResult[0][i] = MaxTriangleCount;
			voSolution[0][i] = SSolution(0, 0, MaxTriangleCount);
			LastTriangleCount = MaxTriangleCount;
		}
		else
		{
			voResult[0][i] = LastTriangleCount;
			voSolution[0][i] = SSolution(0, 0, LastTriangleCount);
		}
	}

	for (unsigned int i = 1; i < vAllTileItem.size(); i++)
	{
		for (unsigned int k = 1; k <= static_cast<unsigned int>(vLoadLimit); k++)
		{
			voResult[i][k] = voResult[i - 1][k];
			for (unsigned int x = 1; x <= vAllTileItem[i].size(); x++)
			{
				if (k >= vAllTileItem[i][x - 1].LoadCostForKnapsackItem)
				{
					auto Temp = vAllTileItem[i][x - 1].TriangleCount + voResult[i - 1][k - vAllTileItem[i][x - 1].LoadCostForKnapsackItem];
					if (voResult[i][k] <= Temp)
					{
						voResult[i][k] = Temp;
						voSolution[i][k] = SSolution(i - 1, static_cast<unsigned int>(k - vAllTileItem[i][x - 1].LoadCostForKnapsackItem), static_cast<unsigned int>(vAllTileItem[i][x - 1].TriangleCount));
					}
				}
			}
		}
	}
	std::cout << voResult[vAllTileItem.size() - 1][vLoadLimit] << std::endl;
	return voSolution;
}

//****************************************************************************
//FUNCTION:找出产生最大价值组合
std::vector<SKnapsackItem> generateSet(const std::vector<std::vector<SKnapsackItem>>& vAllTileItem, const std::vector<std::vector<SSolution>>& vSolution)
{
	std::vector<SKnapsackItem> Res(vAllTileItem.size());
	auto x = static_cast<unsigned int>(vSolution.size() - 1);
	auto y = static_cast<unsigned int>(vSolution[x].size() - 1);
	auto Times = static_cast<int>(vSolution.size() - 1);

	while (Times >= 0)
	{
		if (vSolution[x][y].X == INT_MAX && vSolution[x][y].Y == INT_MAX)
			--x;
		else
		{
			for (auto& BaseNSet : vAllTileItem[x])
			{
				if (BaseNSet.TriangleCount == vSolution[x][y].TriangleCount)
				{
					Res[x] = BaseNSet;
					break;
				}
			}
			auto tx = vSolution[x][y].X;
			auto ty = vSolution[x][y].Y;
			x = tx;
			y = ty;
		}
		--Times;
	}
	return Res;
}

int main()
{
	_MEMORY_LEAK_DETECTOR;

	CTileTreeProcessor* TileTreeProcessor = new CTileTreeProcessor();
	TileTreeProcessor->init();

	////calculate legal count
	for (auto& i : TileTreeProcessor->getRootSet())
	{
		const auto UID = i->getUID();
		auto TileNum = UID & TILE_MUN_MASK;
		TileNum >>= BIT_OFFSET;
		std::cout << "Tile" << TileNum << "\tTotal count = " << TileTreeProcessor->calculateTotalLegalTileNodeSetCount(i, TileTreeProcessor->getTraverseMaxDeep()) << std::endl;
	}

	auto& RootSet = TileTreeProcessor->getRootSet();

	////generate all legal set
	//for (auto& Root : RootSet)
	//{
	//	auto OneTileAllLegalSet = TileTreeProcessor->generateAllLegalTileNodeSet(Root, TileTreeProcessor->getTraverseMaxDeep());
	//	auto BestSet = TileTreeProcessor->generateBestSet(OneTileAllLegalSet);
	//	auto TemplateSet = TileTreeProcessor->generateTemplateSet(OneTileAllLegalSet);
	//}

	////generate all BaseN set
	TileTreeProcessor->generateAllTileBaseNSet(static_cast<unsigned int>(RootSet.size()));

	//{
	//	uintmax_t LoadCost = 0;
	//	uintmax_t TriangleCount = 0;
	//	TileTreeProcessor->calculateLoadCostTriangleCountForTileNodeSet(TileTreeProcessor->getAllTileBaseNSet()[0][6], LoadCost, TriangleCount, 37);
	//	std::cout << LoadCost << "  " << TriangleCount << std::endl;
	//}

	////solve knapsack
	auto t = TileTreeProcessor->getAllTileBaseNLegalTileNodeSet();
	for (auto& i : t)
	{
		int Size = i.size();
		for (int k = 0; k < Size - 7; ++k)
		{
			i.pop_back();
		}
	}

	std::vector<std::vector<SKnapsackItem>> ItemSet(static_cast<unsigned int>(RootSet.size()));
	for (unsigned int i = 0; i < t.size(); ++i)
	{
		for (unsigned int k = 0; k < t[i].size(); ++k)
		{
			//auto LoadCost = t[i][k].LoadCost / hivePagedLOD::MEGABYTE == 0 ? 1 : t[i][k].LoadCost / hivePagedLOD::MEGABYTE;
			auto LoadCost = t[i][k].LoadCost / hivePagedLOD::MEGABYTE + 1;
			ItemSet[i].emplace_back(*(t[i][k].pTileNodeSet), LoadCost, t[i][k].TriangleCount);
		}
	}
	const auto SolutionSet = solveGroupKnapsack(ItemSet, TileTreeProcessor->getLimit() / hivePagedLOD::MEGABYTE);
	auto Result = generateSet(ItemSet, SolutionSet);
	uintmax_t Cost = 0;
	for (auto& i : Result)
	{
		Cost += i.LoadCostForKnapsackItem;
	}
	std::cout << Cost << std::endl;

	//std::vector<std::vector<NodeSet>> save;
	//for (unsigned int i = 0; i < t.size(); ++i)
	//{
	//	save.emplace_back(TileTreeProcessor->generateAllLegalTileNodeSet(t[i][0].pTileNodeSet->at(0), 6));
	//}
	//auto SaveSize = save.size();
	//for (unsigned int i = 0; i < SaveSize - 1; ++i)
	//{
	//	auto Size = save[save.size() - 1].size() * save[save.size() - 2].size();
	//	std::vector<NodeSet> Temp;
	//	
	//	for (unsigned int k = 0; k < save[save.size() - 1].size(); ++k)
	//	{
	//		for (unsigned int x = 0; x < save[save.size() - 2].size(); ++x)
	//		{
	//			Temp.emplace_back(save[save.size() - 1][k]);
	//			Temp.rbegin()->insert(Temp.rbegin()->end(), save[save.size() - 2][x].begin(), save[save.size() - 2][x].end());
	//		}
	//	}
	//	save.pop_back();
	//	save.pop_back();
	//	save.emplace_back(Temp);
	//}
	//std::vector<std::pair<uintmax_t, uintmax_t>> CostTriangleSet;
	//for (auto& i : save[0])
	//{
	//	uintmax_t Cost = 0;
	//	uintmax_t TriangleCount = 0;
	//	std::set<std::string> TexNameSet;
	//	for (auto& TileNode : i)
	//	{
	//		Cost += TileNode->getGeometryFileSize();
	//		if (TexNameSet.find(TileNode->getTextureFileName()) == TexNameSet.end())
	//		{
	//			Cost += TileNode->getTextureFileSize();
	//			TexNameSet.insert(TileNode->getTextureFileName());
	//		}
	//		TriangleCount += TileNode->getTriangleCount();
	//	}
	//	CostTriangleSet.emplace_back(std::make_pair(Cost, TriangleCount));
	//}
	//std::sort(CostTriangleSet.begin(), CostTriangleSet.end(), [](const std::pair<uintmax_t, uintmax_t>& lhs, const std::pair<uintmax_t, uintmax_t>& rhs) { return lhs.second > rhs.second; });
	//for (unsigned int i = 0; i < CostTriangleSet.size(); ++i)
	//{
	//	if (CostTriangleSet[i].first <= TileTreeProcessor->getLimit())
	//	{
	//		std::cout << CostTriangleSet[i].first << "  " << CostTriangleSet[i].second << std::endl;
	//		break;
	//	}
	//}


	std::vector<std::vector<std::pair<uintmax_t, uintmax_t>>> save(TileTreeProcessor->getRootSet().size());
	for (unsigned int i = 0; i < TileTreeProcessor->getRootSet().size(); ++i)
	{
		auto LegalSet = TileTreeProcessor->generateAllLegalTileNodeSet(TileTreeProcessor->getRootSet()[i], 6);
		for (unsigned int k = 0; k < LegalSet.size(); ++k)
		{
			uintmax_t LoadCost = 0, TriangleCount = 0;
			TileTreeProcessor->calculateLoadCostTriangleCountForTileNodeSet(LegalSet[k], LoadCost, TriangleCount);
			save[i].emplace_back(std::make_pair(LoadCost, TriangleCount));
		}
	}
	auto SaveSize = save.size();
	for (unsigned int i = 0; i < SaveSize - 1; ++i)
	{
		auto Size = save[save.size() - 1].size() * save[save.size() - 2].size();
		std::vector<std::pair<uintmax_t, uintmax_t>> Temp;

		for (unsigned int k = 0; k < save[save.size() - 1].size(); ++k)
		{
			auto& Last = save[save.size() - 1][k];
			for (unsigned int x = 0; x < save[save.size() - 2].size(); ++x)
			{
				if (save[save.size() - 2][x].first + Last.first <= TileTreeProcessor->getLimit())
					Temp.emplace_back(std::make_pair(save[save.size() - 2][x].first + Last.first, save[save.size() - 2][x].second + Last.second));
			}
		}
		save.pop_back();
		save.pop_back();
		save.emplace_back(Temp);
	}
	std::sort(save[0].begin(), save[0].end(), [](const std::pair<uintmax_t, uintmax_t>& lhs, const std::pair<uintmax_t, uintmax_t>& rhs) { return lhs.second > rhs.second; });
	for (unsigned int i = 0; i < save[0].size(); ++i)
	{
		if (save[0][i].first <= TileTreeProcessor->getLimit())
		{
			std::cout << save[0][i].first << "  " << save[0][i].second << std::endl;
			break;
		}
	}

	delete TileTreeProcessor;
	return 0;
}
