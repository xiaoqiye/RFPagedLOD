#pragma once

#include <vector>
#include "Scene.h"
#include "TileNode.h"

namespace hivePagedLOD
{
	typedef std::vector<std::shared_ptr<CTileNode>> NodeSet;

	class CItemTemplateCreator
	{
	public:
		CItemTemplateCreator() = delete;
		CItemTemplateCreator(int vTraverseMaxDeep) : m_TraverseMaxDeep(vTraverseMaxDeep) {}
		~CItemTemplateCreator();
		
		void generateAllTileItemTemplate(const std::vector<std::shared_ptr<CTileNode>>& vRootSet);

		////enumerate all
		uintmax_t calculateTotalLegalTileNodeSetCount(const std::shared_ptr<CTileNode>& vRoot, int vTraverseMaxDeep = -1);
		std::vector<NodeSet> generateAllLegalTileNodeSet(const std::shared_ptr<CTileNode>& vRoot, int vTraverseMaxDeep = -1);

		////baseN
		void generateAllTileBaseNSet(unsigned int vSize, const std::vector<std::shared_ptr<CTileNode>>& vRootSet);

		static void calculateLoadCostTriangleCountForTileNodeSet(const std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& vTileNodeSet, uintmax_t& voLoadCost, uintmax_t& voTriangleCount, unsigned int vMaxTileNodeCount = INT_MAX);

		SKnapsackItem generateBestSet(std::vector<NodeSet>& vAllLegalSet);
		std::vector<SKnapsackItem> generateTemplateSet(std::vector<NodeSet>& vAllLegalSet);

		const std::vector<std::vector<std::vector<std::shared_ptr<CTileNode>>>>& getAllTileBaseNSet() const { return m_AllTileBaseNSet; }
		const std::vector<std::vector<SKnapsackItem>>& getAllTileItemTemplateSet() const { return m_AllTileItemTemplateSet; }
		int getTraverseMaxDeep() const { return m_TraverseMaxDeep; }
	private:
		////enumerate all
		void __combineTwoSet(std::vector<std::vector<NodeSet>>& voBrotherNodeResultSet);
		std::vector<NodeSet> __generateResultNodeSet(std::vector<std::vector<NodeSet>>& vBrotherNodeResultSet);

		////baseN
		unsigned int __generateBaseNSet(const std::shared_ptr<CTileNode>& vRoot, unsigned int vN, std::vector<std::shared_ptr<hivePagedLOD::CTileNode>>& voBaseNSet);
		void __outputBaseNSetInfo(const std::vector<std::vector<std::shared_ptr<CTileNode>>>& vBaseNSet);

		int m_TraverseMaxDeep;
		std::vector<std::vector<std::vector<std::shared_ptr<CTileNode>>>> m_AllTileBaseNSet;
		std::vector<std::vector<SKnapsackItem>> m_AllTileItemTemplateSet;
	};
}