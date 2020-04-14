#include <vector>
#include <string>
#include <iostream>
#include "../../../PagedLOD/PagedLOD/Scene.h"
#include <set>

unsigned int traverse(const std::shared_ptr<hivePagedLOD::CTileNode>& vRoot, unsigned int vN, std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& voBaseNSet)
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
				auto Deep = traverse(TileNode, vN, voBaseNSet);
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

void calculateLoadCostTriangleCountForTileNodeSet(const std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& vTileNodeSet, uintmax_t& voLoadCost, uintmax_t& voTriangleCount, unsigned int vMaxTileNodeCount = INT_MAX)
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

void outputInfo(const std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>>& vBaseNSet)
{
	uintmax_t MaxLevelLoadSize = 0;
	uintmax_t MaxLevelTriangleCount = 0;

	for (int i = vBaseNSet.size() - 1; i >= 0; --i)
	{
		uintmax_t TriangleCount = 0;
		uintmax_t LoadSize = 0;
		calculateLoadCostTriangleCountForTileNodeSet(vBaseNSet[i], LoadSize, TriangleCount);
		if (i == vBaseNSet.size() - 1)
		{
			MaxLevelLoadSize = LoadSize;
			MaxLevelTriangleCount = TriangleCount;
		}
		std::cout << "Base" << i << "\tTriangle count = " << TriangleCount << "  \tLoad size = " << LoadSize << "  \tTriangle = " << static_cast<float>(TriangleCount) / MaxLevelTriangleCount * 100 << "%" << "  \tLoad = " << static_cast<float>(LoadSize) / MaxLevelLoadSize * 100 << "%" << std::endl;
	}
}

int main()
{
	std::string SerializedPath = "G:\\Resource\\NewSerialized\\";
	std::string	BinPath = "G:\\Resource\\Bin\\";
	hivePagedLOD::CScene::getInstance()->init(SerializedPath, BinPath, true, 1, 1);
	//auto Tile1Root = hivePagedLOD::CScene::getInstance()->getTileSet()[0];
	//std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>> BaseNSet(16);
	//unsigned int MaxDeep = INT_MAX;
	//for (unsigned int i = 0; i < 16; ++i)
	//{
	//	const auto BeforeDeep = MaxDeep;
	//	MaxDeep = traverse(Tile1Root, i, BaseNSet[i]);
	//	if (BeforeDeep == MaxDeep)
	//		break;
	//}
	//BaseNSet.resize(MaxDeep + 1);

	auto& TileRootSet = hivePagedLOD::CScene::getInstance()->getTileSet();
	std::vector<std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>>> BaseNSet(TileRootSet.size());
	for (auto& i : BaseNSet)
	{
		i.resize(16);
	}

	for (unsigned int i = 0; i < TileRootSet.size(); ++i)
	{
		unsigned int MaxDeep = INT_MAX;
		for (unsigned int k = 0; k < 16; ++k)
		{
			const auto BeforeDeep = MaxDeep;
			MaxDeep = traverse(TileRootSet[i], k, BaseNSet[i][k]);
			if (BeforeDeep == MaxDeep)
				break;
		}
		BaseNSet[i].resize(static_cast<uintmax_t>(MaxDeep) + 1);
		auto UID = TileRootSet[i]->getUID();
		auto TileNum = UID & 0xFF000000;
		TileNum >>= 24;
		std::cout << "-------------Tile " << TileNum << "-------------" << std::endl;
		outputInfo(BaseNSet[i]);
	}
	uintmax_t TriangleCount = 0;
	uintmax_t LoadSize = 0;
	calculateLoadCostTriangleCountForTileNodeSet(BaseNSet[0][5], LoadSize, TriangleCount, 4);
	return 0;
}