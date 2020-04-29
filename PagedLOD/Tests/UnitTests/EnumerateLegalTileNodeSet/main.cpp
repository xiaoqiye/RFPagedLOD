#include <vector>
#include <string>
#include <iostream>
#include <set>
#include "Scene.h"

//struct TreeNode
//{
//	int val;
//	std::vector<TreeNode*> ChildrenSet;
//	TreeNode(int x) : val(x) {}
//	void setChildren(const std::vector<TreeNode*>& vChildren) { ChildrenSet = vChildren; }
//};
//
//int main()
//{
//	TreeNode* Root = new TreeNode(1);
//	TreeNode* Node2 = new TreeNode(2);
//	TreeNode* Node3 = new TreeNode(3);
//	TreeNode* Node4 = new TreeNode(4);
//	TreeNode* Node5 = new TreeNode(5);
//	TreeNode* Node6 = new TreeNode(6);
//	TreeNode* Node7 = new TreeNode(7);
//	TreeNode* Node8 = new TreeNode(8);
//	TreeNode* Node9 = new TreeNode(9);
//	TreeNode* Node10 = new TreeNode(10);
//	TreeNode* Node11 = new TreeNode(11);
//	TreeNode* Node12 = new TreeNode(12);
//	TreeNode* Node13 = new TreeNode(13);
//	TreeNode* Node14 = new TreeNode(14);
//	TreeNode* Node15 = new TreeNode(15);
//	TreeNode* Node16 = new TreeNode(16);
//	TreeNode* Node17 = new TreeNode(17);
//	TreeNode* Node18 = new TreeNode(18);
//	TreeNode* Node19 = new TreeNode(19);
//	TreeNode* Node20 = new TreeNode(20);
//
//	std::vector<TreeNode*> t;
//	t.emplace_back(Node2);
//	t.emplace_back(Node3);
//	t.emplace_back(Node4);
//	Root->setChildren(t);
//	t.clear();
//	t.emplace_back(Node5);
//	t.emplace_back(Node6);
//	t.emplace_back(Node7);
//	Node2->setChildren(t);
//	t.clear();
//	t.emplace_back(Node8);
//	t.emplace_back(Node9);
//	Node4->setChildren(t);
//	t.clear();
//	t.emplace_back(Node10);
//	t.emplace_back(Node11);
//	Node5->setChildren(t);
//	t.clear();
//	t.emplace_back(Node15);
//	Node6->setChildren(t);
//	t.clear();
//	t.emplace_back(Node12);
//	Node7->setChildren(t);
//	t.clear();
//	t.emplace_back(Node13);
//	t.emplace_back(Node14);
//	Node8->setChildren(t);
//	t.clear();
//	t.emplace_back(Node16);
//	t.emplace_back(Node17);
//	Node15->setChildren(t);
//
//	//t.emplace_back(Node2);
//	//t.emplace_back(Node3);
//	//Root->setChildren(t);
//	//t.clear();
//	//t.emplace_back(Node4);
//	//t.emplace_back(Node5);
//	//Node2->setChildren(t);
//	//t.clear();
//	//t.emplace_back(Node8);
//	//t.emplace_back(Node9);
//	//Node4->setChildren(t);
//	//t.clear();
//	//t.emplace_back(Node10);
//	//t.emplace_back(Node11);
//	//Node5->setChildren(t);
//	//t.clear();
//	//t.emplace_back(Node15);
//	//t.emplace_back(Node16);
//	//Node8->setChildren(t);
//	//t.clear();
//	//t.emplace_back(Node17);
//	//Node9->setChildren(t);
//	//t.clear();
//	//t.emplace_back(Node18);
//	//Node10->setChildren(t);
//	//t.clear();
//	//t.emplace_back(Node6);
//	//t.emplace_back(Node7);
//	//Node3->setChildren(t);
//	//t.clear();
//	//t.emplace_back(Node12);
//	//t.emplace_back(Node13);
//	//t.emplace_back(Node14);
//	//Node6->setChildren(t);
//	//t.clear();
//	//t.emplace_back(Node19);
//	//Node12->setChildren(t);
//	//t.clear();
//	//t.emplace_back(Node20);
//	//Node14->setChildren(t);
//	std::vector<std::vector<TreeNode*>> AllSet;
//	AllSet = traverse(Root);
//	return 0;
//}

uintmax_t calculateTotalLegalTileNodeSet(const std::shared_ptr<hivePagedLOD::CTileNode>& vRoot, int vTraverseMaxDeep = -1) //parent -- add, brother -- multiplication
{
	if (vRoot->getAllChildren().empty() || vRoot->getNodeDeep() == vTraverseMaxDeep)
		return 1;
	std::vector<uintmax_t> BrotherNodeReturnValueSet;
	for (const auto& TileNode : vRoot->getAllChildren())
	{
		auto BrotherTileNodeLegalCount = calculateTotalLegalTileNodeSet(TileNode, vTraverseMaxDeep);
		BrotherNodeReturnValueSet.emplace_back(BrotherTileNodeLegalCount);
	}
	uintmax_t Res = 1;
	for (auto& i : BrotherNodeReturnValueSet)
		Res *= i;
	return Res + 1;
}

typedef std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> NodeSet;

void __combineTwoSet(std::vector<std::vector<NodeSet>>& voBrotherNodeResultSet)
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

////from back to front -- pop_back()
////combine two set to one set -- total times == vBrotherNodeResultSet.size() - 1
std::vector<NodeSet> __generateResultNodeSet(std::vector<std::vector<NodeSet>>& vBrotherNodeResultSet)
{
	if (vBrotherNodeResultSet.size() == 1)
		return vBrotherNodeResultSet[0];

	const auto RunTimes = vBrotherNodeResultSet.size();
	for (unsigned int i = 1; i < RunTimes; ++i)
		__combineTwoSet(vBrotherNodeResultSet);

	return *vBrotherNodeResultSet.rbegin();
}

////potential values == voAllSet.size();
////parent -- add, brother -- multiplication
////vTraverseMaxDeep == -1 -- traverse all node
std::vector<NodeSet> traverse(const std::shared_ptr<hivePagedLOD::CTileNode>& vRoot, int vTraverseMaxDeep = -1)
{
	auto& ChildrenSet = vRoot->getAllChildren();
	if (ChildrenSet.empty() || vRoot->getNodeDeep() == vTraverseMaxDeep)
		return std::vector<NodeSet>{ NodeSet{ vRoot } };

	std::vector<std::vector<NodeSet>> BrotherNodeResultSet;
	for (auto& TreeNode : ChildrenSet)
	{
		std::vector<NodeSet> ResultSet;
		BrotherNodeResultSet.emplace_back(traverse(TreeNode, vTraverseMaxDeep));
	}

	std::vector<NodeSet> ResultNodeSet = __generateResultNodeSet(BrotherNodeResultSet);

	////add self to ResultNodeSet
	ResultNodeSet.emplace_back(NodeSet{ vRoot });
	return ResultNodeSet;
}

struct SLegalTileNodeSet
{
	std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>* pTileNodeSet;
	uintmax_t LoadCost = 0;
	uintmax_t TriangleCount = 0;
	SLegalTileNodeSet(std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>* vTileNodeSet, uintmax_t vLoadCost, uintmax_t vTriangleCount) : pTileNodeSet(vTileNodeSet), LoadCost(vLoadCost), TriangleCount(vTriangleCount) {}
};

int main()
{
	std::string SerializedPath = "G:\\Resource\\NewSerialized\\";
	std::string	BinPath = "G:\\Resource\\Bin\\";
	hivePagedLOD::CScene::getInstance()->init(SerializedPath, BinPath, true, 1, 1);
	auto RootSet = hivePagedLOD::CScene::getInstance()->getTileSet();
	int TraverseMaxDeep = 6;
	for (auto& i : RootSet)
	{
		auto UID = i->getUID();
		auto TileNum = UID & 0xFF000000;
		TileNum >>= 24;
		std::cout << "Tile" << TileNum << "\tTotal count = " << calculateTotalLegalTileNodeSet(i, TraverseMaxDeep) << std::endl;
	}
	
	auto AllSet = traverse(RootSet[0], TraverseMaxDeep);
	std::vector<SLegalTileNodeSet> LegalTileNodeSetForOneTile;

	for (unsigned int i = 0; i < AllSet.size(); ++i)
	{
		uintmax_t TriangleCount = 0;
		std::set<std::string> TextureSet;
		uintmax_t LoadSize = 0;
		for (unsigned int k = 0; k < AllSet[i].size(); ++k)
		{
			TriangleCount += AllSet[i][k]->getTriangleCount();
			LoadSize += AllSet[i][k]->getGeometryFileSize();
			if (TextureSet.find(AllSet[i][k]->getTextureFileName()) == TextureSet.end())
			{
				LoadSize += AllSet[i][k]->getTextureFileSize();
				TextureSet.insert(AllSet[i][k]->getTextureFileName());
			}
		}
		LegalTileNodeSetForOneTile.emplace_back(&AllSet[i], LoadSize, TriangleCount);
	}
	std::sort(LegalTileNodeSetForOneTile.begin(), LegalTileNodeSetForOneTile.end(), [](const SLegalTileNodeSet& lhs, const SLegalTileNodeSet& rhs) {return lhs.TriangleCount > rhs.TriangleCount; });

	uintmax_t MaxLoadCost = 10 * 1024 * 1024;
	SLegalTileNodeSet* Best = nullptr;
	for (auto& i : LegalTileNodeSetForOneTile)
	{
		if (i.LoadCost < MaxLoadCost)
		{
			Best = &i;
			break;
		}
	}
	for (auto& TileNode : *(Best->pTileNodeSet))
	{
		std::cout << TileNode->getGeometryFileName() << std::endl;
	}
	return 0;
}