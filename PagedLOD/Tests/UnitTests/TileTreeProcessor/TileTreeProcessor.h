#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <set>
#include "../../../PagedLOD/PagedLOD/Scene.h"
#include "../../../PagedLOD/PagedLOD/TileNode.h"

const unsigned int TILE_MUN_MASK = 0xFF000000;
const unsigned int BIT_OFFSET = 24;
const unsigned int MAX_NODE_DEEP = 16;

typedef std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> NodeSet;

struct SLegalTileNodeSet
{
	std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>* pTileNodeSet = nullptr;
	uintmax_t LoadCost = 0;
	uintmax_t TriangleCount = 0;
	SLegalTileNodeSet(std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>* vTileNodeSet, uintmax_t vLoadCost, uintmax_t vTriangleCount) : pTileNodeSet(vTileNodeSet), LoadCost(vLoadCost), TriangleCount(vTriangleCount) {}
	SLegalTileNodeSet() = default;
};

class CTileTreeProcessor
{
public:
	CTileTreeProcessor() = default;
	void init();
	uintmax_t calculateTotalLegalTileNodeSetCount(const std::shared_ptr<hivePagedLOD::CTileNode>& vRoot, int vTraverseMaxDeep = -1);
	std::vector<NodeSet> generateAllLegalTileNodeSet(const std::shared_ptr<hivePagedLOD::CTileNode>& vRoot, int vTraverseMaxDeep = -1);
	SLegalTileNodeSet generateBestSet(std::vector<NodeSet>& vAllLegalSet);
	std::vector<SLegalTileNodeSet> generateTemplateSet(std::vector<NodeSet>& vAllLegalSet);

	void generateAllTileBaseNSet(unsigned int vSize);

	static void calculateLoadCostTriangleCountForTileNodeSet(const std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& vTileNodeSet, uintmax_t& voLoadCost, uintmax_t& voTriangleCount, unsigned int vMaxTileNodeCount = INT_MAX);

	const std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& getRootSet() const { return m_RootSet; }
	const std::vector<std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>>>& getAllTileBaseNSet() const { return m_AllTileBaseNSet; }
	const std::vector<std::vector<SLegalTileNodeSet>>& getAllTileBaseNLegalTileNodeSet() const { return m_AllTileBaseNLegalTileNodeSet; }
	int getTraverseMaxDeep() const { return m_TraverseMaxDeep; }
	uintmax_t getLimit() const { return m_MaxLoadCost; }
private:
	void __combineTwoSet(std::vector<std::vector<NodeSet>>& voBrotherNodeResultSet);
	std::vector<NodeSet> __generateResultNodeSet(std::vector<std::vector<NodeSet>>& vBrotherNodeResultSet);

	unsigned int __generateBaseNSet(const std::shared_ptr<hivePagedLOD::CTileNode>& vRoot, unsigned int vN, std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& voBaseNSet);
	void __outputBaseNSetInfo(const std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>>& vBaseNSet);

	std::vector<std::shared_ptr<hivePagedLOD::CTileNode>> m_RootSet;
	std::string m_SerializedPath;
	std::string	m_BinPath;
	int m_TraverseMaxDeep;
	uintmax_t m_MaxLoadCost;
	std::vector<std::vector<std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>>> m_AllTileBaseNSet;
	std::vector<std::vector<SLegalTileNodeSet>> m_AllTileBaseNLegalTileNodeSet;
};