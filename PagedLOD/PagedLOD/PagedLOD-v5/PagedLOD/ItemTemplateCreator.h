#pragma once

#include <vector>
#include "Scene.h"
#include "TileNode.h"

namespace hivePagedLOD
{
	class CItemTemplateCreator
	{
	public:
		CItemTemplateCreator() = delete;
		CItemTemplateCreator(int vTraverseMaxDeep) : m_TraverseMaxDeep(vTraverseMaxDeep) {}
		~CItemTemplateCreator();
		
		void generateAllTileItemTemplate(const std::string& vPath, unsigned int vTotalTileNum);
		void loadBaseNSetByRecords(const std::string& vPath, unsigned int vTotalTileNum);
		
		const std::vector<std::vector<std::vector<std::shared_ptr<CTileNode>>>>& getAllTileBaseNSet() const { return m_AllTileBaseNSet; }
		const std::vector<std::vector<SKnapsackItem>>& getAllTileItemTemplateSet() const { return m_AllTileItemTemplateSet; }
		int getTraverseMaxDeep() const { return m_TraverseMaxDeep; }

	private:
		int m_TraverseMaxDeep;
		std::vector<std::vector<std::vector<std::shared_ptr<CTileNode>>>> m_AllTileBaseNSet;
		std::vector<std::vector<SKnapsackItem>> m_AllTileItemTemplateSet;
	};
}