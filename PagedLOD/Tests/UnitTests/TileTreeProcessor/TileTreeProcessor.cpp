#include "TileTreeProcessor.h"

//****************************************************************************
//FUNCTION:
void CTileTreeProcessor::init()
{
	std::ifstream File("./InitValue");
	bool LegalTileName;
	unsigned int BeginTileNumber;
	unsigned int EndTileNumber;
	if (File.is_open())
	{
		std::string CurrentLine;
		std::getline(File, CurrentLine);
		m_SerializedPath = CurrentLine.substr(CurrentLine.find_last_of('=') + 1);
		std::getline(File, CurrentLine);
		m_BinPath = CurrentLine.substr(CurrentLine.find_last_of('=') + 1);
		std::getline(File, CurrentLine);
		LegalTileName = CurrentLine.substr(CurrentLine.find_last_of('=') + 1) == "1" ? true : false;
		std::getline(File, CurrentLine);
		BeginTileNumber = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		EndTileNumber = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		m_TraverseMaxDeep = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		m_MaxLoadCost = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
	}
	else
	{
		throw std::exception("cannot open InitValue file.");
	}
	File.close();

	hivePagedLOD::CScene::getInstance()->init(m_SerializedPath, m_BinPath, LegalTileName, BeginTileNumber, EndTileNumber);
	m_RootSet = hivePagedLOD::CScene::getInstance()->getTileSet();
}

//****************************************************************************
//FUNCTION:
////parent -- add, brother -- multiplication
uintmax_t CTileTreeProcessor::calculateTotalLegalTileNodeSetCount(const std::shared_ptr<hivePagedLOD::CTileNode>& vRoot, int vTraverseMaxDeep)
{
	if (vRoot->getAllChildren().empty() || vRoot->getNodeDeep() == static_cast<unsigned int>(vTraverseMaxDeep))
		return 1;
	std::vector<uintmax_t> BrotherNodeReturnValueSet;
	for (const auto& TileNode : vRoot->getAllChildren())
	{
		auto BrotherTileNodeLegalCount = calculateTotalLegalTileNodeSetCount(TileNode, vTraverseMaxDeep);
		BrotherNodeReturnValueSet.emplace_back(BrotherTileNodeLegalCount);
	}
	uintmax_t Res = 1;
	for (auto& i : BrotherNodeReturnValueSet)
		Res *= i;
	return Res + 1;
}

//****************************************************************************
//FUNCTION:
////potential values == voAllSet.size();
////parent -- add, brother -- multiplication
////vTraverseMaxDeep == -1 -- traverse all node
std::vector<NodeSet> CTileTreeProcessor::generateAllLegalTileNodeSet(const std::shared_ptr<hivePagedLOD::CTileNode>& vRoot, int vTraverseMaxDeep)
{
	auto& ChildrenSet = vRoot->getAllChildren();
	if (ChildrenSet.empty() || vRoot->getNodeDeep() == static_cast<unsigned int>(vTraverseMaxDeep))
		return std::vector<NodeSet>{ NodeSet{ vRoot } };

	std::vector<std::vector<NodeSet>> BrotherNodeResultSet;
	for (auto& TreeNode : ChildrenSet)
	{
		std::vector<NodeSet> ResultSet;
		BrotherNodeResultSet.emplace_back(generateAllLegalTileNodeSet(TreeNode, vTraverseMaxDeep));
	}

	std::vector<NodeSet> ResultNodeSet = __generateResultNodeSet(BrotherNodeResultSet);

	////add self to ResultNodeSet
	ResultNodeSet.emplace_back(NodeSet{ vRoot });
	return ResultNodeSet;
}

//****************************************************************************
//FUNCTION:
SLegalTileNodeSet CTileTreeProcessor::generateBestSet(std::vector<NodeSet>& vAllLegalSet)
{
	std::vector<SLegalTileNodeSet> LegalTileNodeSetForOneTile;
	for (auto& i : vAllLegalSet)
	{
		uintmax_t TriangleCount = 0;
		uintmax_t LoadCost = 0;
		calculateLoadCostTriangleCountForTileNodeSet(i, LoadCost, TriangleCount);

		LegalTileNodeSetForOneTile.emplace_back(&i, LoadCost, TriangleCount);
	}
	std::sort(LegalTileNodeSetForOneTile.begin(), LegalTileNodeSetForOneTile.end(), [](const SLegalTileNodeSet& lhs, const SLegalTileNodeSet& rhs) {return lhs.TriangleCount > rhs.TriangleCount; });

	SLegalTileNodeSet* Best = nullptr;
	for (auto& i : LegalTileNodeSetForOneTile)
	{
		if (i.LoadCost <= m_MaxLoadCost)
		{
			Best = &i;
			break;
		}
	}
	for (auto& TileNode : *(Best->pTileNodeSet))
	{
		std::cout << TileNode->getGeometryFileName() << std::endl;
	}
	return *Best;
}

//****************************************************************************
//FUNCTION:
std::vector<SLegalTileNodeSet> CTileTreeProcessor::generateTemplateSet(std::vector<NodeSet>& vAllLegalSet)
{
	std::vector<SLegalTileNodeSet> LegalTileNodeSetForOneTile;
	for (auto& i : vAllLegalSet)
	{
		uintmax_t TriangleCount = 0;
		uintmax_t LoadCost = 0;
		calculateLoadCostTriangleCountForTileNodeSet(i, LoadCost, TriangleCount);

		LegalTileNodeSetForOneTile.emplace_back(&i, LoadCost, TriangleCount);
	}
	std::sort(LegalTileNodeSetForOneTile.begin(), LegalTileNodeSetForOneTile.end(), [](const SLegalTileNodeSet& lhs, const SLegalTileNodeSet& rhs) {return lhs.LoadCost > rhs.LoadCost; });

	std::vector<SLegalTileNodeSet> ResultSet;
	float Percent = 1.0f;
	auto LastIt = LegalTileNodeSetForOneTile.begin();
	const auto MaxLoadCost = LastIt->LoadCost;
	int Index = 0;
	for (int i = 10; i > 0; --i)
	{
		Percent = static_cast<float>(i) / 10;
		auto it = LastIt;
		while (it->LoadCost > Percent * MaxLoadCost)
		{
			++it;
			++Index;
		}
		ResultSet.emplace_back(*it);
		LastIt = it;
	}
	for (auto& TileNodeSet : ResultSet)
	{
		std::cout << "-------------------" << std::endl;
		for (auto& TileNode : *TileNodeSet.pTileNodeSet)
		{
			std::cout << TileNode->getGeometryFileName() << std::endl;
		}
	}
	return ResultSet;
}

//****************************************************************************
//FUNCTION:
void CTileTreeProcessor::generateAllTileBaseNSet(unsigned int vSize)
{
	m_AllTileBaseNSet.resize(vSize);
	for (auto& i : m_AllTileBaseNSet)
	{
		i.resize(MAX_NODE_DEEP);
	}

	for (unsigned int i = 0; i < m_RootSet.size(); ++i)
	{
		unsigned int MaxDeep = INT_MAX;
		for (unsigned int k = 0; k < 16; ++k)
		{
			const auto BeforeDeep = MaxDeep;
			MaxDeep = __generateBaseNSet(m_RootSet[i], k, m_AllTileBaseNSet[i][k]);
			if (BeforeDeep == MaxDeep)
				break;
		}
		m_AllTileBaseNSet[i].resize(static_cast<uintmax_t>(MaxDeep) + 1);
		//auto UID = m_RootSet[i]->getUID();
		//auto TileNum = UID & 0xFF000000;
		//TileNum >>= 24;
		//std::cout << "-------------Tile " << TileNum << "-------------" << std::endl;
		//__outputBaseNSetInfo(m_AllTileBaseNSet[i]);
	}
	m_AllTileBaseNLegalTileNodeSet.resize(m_AllTileBaseNSet.size());
	for (unsigned int i = 0; i < m_AllTileBaseNSet.size(); ++i)
	{
		for (unsigned int k = 0; k < m_AllTileBaseNSet[i].size(); ++k)
		{
			uintmax_t LoadCost = 0, TriangleCount = 0;
			calculateLoadCostTriangleCountForTileNodeSet(m_AllTileBaseNSet[i][k], LoadCost, TriangleCount);
			m_AllTileBaseNLegalTileNodeSet[i].emplace_back(&m_AllTileBaseNSet[i][k], LoadCost, TriangleCount);
		}
	}
}

//****************************************************************************
//FUNCTION:
void CTileTreeProcessor::__outputBaseNSetInfo(const std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>>& vBaseNSet)
{
	uintmax_t MaxLevelLoadSize = 0;
	uintmax_t MaxLevelTriangleCount = 0;
	const int Size = static_cast<int>(vBaseNSet.size()) - 1;
	for (int i = Size; i >= 0; --i)
	{
		uintmax_t TriangleCount = 0;
		uintmax_t LoadSize = 0;
		calculateLoadCostTriangleCountForTileNodeSet(vBaseNSet[i], LoadSize, TriangleCount);
		if (i == Size)
		{
			MaxLevelLoadSize = LoadSize;
			MaxLevelTriangleCount = TriangleCount;
		}
		std::cout << "Base" << i << "\tTriangle count = " << TriangleCount << "\t\tLoad size = " << LoadSize << "\tTriangle = " << static_cast<float>(TriangleCount) / MaxLevelTriangleCount * 100 << "%" << "\tLoad = " << static_cast<float>(LoadSize) / MaxLevelLoadSize * 100 << "%" << std::endl;
	}
}

//****************************************************************************
//FUNCTION:
unsigned int CTileTreeProcessor::__generateBaseNSet(const std::shared_ptr<hivePagedLOD::CTileNode>& vRoot, unsigned int vN, std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& voBaseNSet)
{
	if (vRoot->getNodeDeep() < vN)
	{
		auto& ChildrenSet = vRoot->getAllChildren();
		if (ChildrenSet.empty())
		{
			voBaseNSet.emplace_back(vRoot);
			return vRoot->getNodeDeep();
		}
		else
		{
			unsigned int MaxDeep = 0;
			for (auto& TileNode : ChildrenSet)
			{
				auto Deep = __generateBaseNSet(TileNode, vN, voBaseNSet);
				MaxDeep = Deep > MaxDeep ? Deep : MaxDeep;
			}
			return MaxDeep;
		}
	}
	else
	{
		_ASSERTE(vRoot->getNodeDeep() == vN);
		voBaseNSet.emplace_back(vRoot);
		return vRoot->getNodeDeep();
	}
}

//****************************************************************************
//FUNCTION:
void CTileTreeProcessor::__combineTwoSet(std::vector<std::vector<NodeSet>>& voBrotherNodeResultSet)
{
	std::vector<NodeSet> Res;
	auto& FirstNodeSet = voBrotherNodeResultSet[voBrotherNodeResultSet.size() - 1];
	auto& SecondNodeSet = voBrotherNodeResultSet[voBrotherNodeResultSet.size() - 2];
	for (unsigned int i = 0; i < FirstNodeSet.size(); ++i)
	{
		NodeSet t;
		for (unsigned int k = 0; k < FirstNodeSet[i].size(); ++k)
			t.emplace_back(FirstNodeSet[i][k]);
		for (unsigned int m = 0; m < SecondNodeSet.size(); ++m)
		{
			auto copyT = t;
			for (unsigned int n = 0; n < SecondNodeSet[m].size(); ++n)
				copyT.emplace_back(SecondNodeSet[m][n]);
			Res.emplace_back(copyT);
		}
	}

	voBrotherNodeResultSet.pop_back();
	voBrotherNodeResultSet.pop_back();
	voBrotherNodeResultSet.emplace_back(Res);
}

//****************************************************************************
//FUNCTION:
////from back to front -- pop_back()
////combine two set to one set -- total times == vBrotherNodeResultSet.size() - 1
std::vector<NodeSet> CTileTreeProcessor::__generateResultNodeSet(std::vector<std::vector<NodeSet>>& vBrotherNodeResultSet)
{
	if (vBrotherNodeResultSet.size() == 1)
		return vBrotherNodeResultSet[0];

	const auto RunTimes = vBrotherNodeResultSet.size();
	for (unsigned int i = 1; i < RunTimes; ++i)
		__combineTwoSet(vBrotherNodeResultSet);

	return *vBrotherNodeResultSet.rbegin();
}

//****************************************************************************
//FUNCTION:
void CTileTreeProcessor::calculateLoadCostTriangleCountForTileNodeSet(const std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& vTileNodeSet, uintmax_t& voLoadCost, uintmax_t& voTriangleCount, unsigned int vMaxTileNodeCount)
{
	std::set<std::string> TextureSet;
	for (unsigned int i = 0; i < vTileNodeSet.size() && i < vMaxTileNodeCount; ++i)
	{
		voTriangleCount += vTileNodeSet[i]->getTriangleCount();
		voLoadCost += vTileNodeSet[i]->getGeometryFileSize();
		if (TextureSet.find(vTileNodeSet[i]->getTextureFileName()) == TextureSet.end())
		{
			voLoadCost += vTileNodeSet[i]->getTextureFileSize();
			TextureSet.insert(vTileNodeSet[i]->getTextureFileName());
		}
	}
}

////****************************************************************************
////FUNCTION:
//void CTileTreeProcessor::solveKnapsack()
//{
//	m_AllTileBaseNSet
//}

