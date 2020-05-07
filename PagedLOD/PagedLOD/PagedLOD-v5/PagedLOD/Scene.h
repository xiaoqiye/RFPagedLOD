#pragma once
#include "TileNode.h"
#include "Common.h"
#include "ItemTemplateCreator.h"

#include <common/Singleton.h>

#include <tbb/concurrent_unordered_map.h>

#include <vector>

namespace hivePagedLOD
{
	typedef std::pair<std::shared_ptr<CTileNode>, int> NodeRecord;

	class CTileNode;
	class CItemTemplateCreator;
	
	class CScene : public hiveOO::CSingleton<CScene>
	{
	public:
		~CScene();
		bool init(const std::string& vSerializedPath, const std::string& vBinFolderPath, const std::string& vBaseNPath, bool vLegalTileName = false, unsigned int vBeginTileNumber = 1, unsigned int vEndTileNumber = 0, int vTraverseMaxDeep = -1);
		const std::vector<std::shared_ptr<CTileNode>>& getTileSet() const { return m_TileSet; }
		std::vector<std::shared_ptr<CTileNode>>& fetchTileSet() { return m_TileSet; }
		
		std::vector<tbb::concurrent_unordered_map<unsigned int, STileNodeLoadCost>>* getLoadCostMapPointer() { return &m_LoadCostMap; }
		std::vector<tbb::concurrent_unordered_map<unsigned int, std::vector<unsigned int>>>* getAncestorMapPointer() { return &m_TileNodeAncestorMap; }
		std::vector<tbb::concurrent_unordered_map<unsigned int, std::vector<unsigned int>>>* getBrotherMapPointer() { return &m_TileNodeBrotherMap; }
		const std::shared_ptr<CItemTemplateCreator>& getItemTemplateCreatorPointer() const { return m_pItemTemplateCreator; }
		const std::shared_ptr<CTileNode>& getTileNodePointerByUID(unsigned vUID);
		
		void resetLoadCostMap() { m_LoadCostMap = m_LoadCostMapRaw; }
		void resetTileNodeStatus(const std::vector<unsigned int>& vTileNumSet);
		
		void outputLoadCostMap(const std::string& vFunctionName);

#ifdef _UNIT_TEST
		NodeRecord loadNodeRecordFromFile(std::istream& vIs) 
		{ 
			return __loadNodeRecordFromFile(vIs);
		}
		std::shared_ptr<CTileNode> parseRecord(const std::vector<NodeRecord>& vTreeRecord) { return __parseRecord(vTreeRecord); }
#endif

	private:
		CScene() = default;
		bool __initMap(unsigned int vMaxTileNum);
		bool __initAll();
		bool __initRange(unsigned int vBeginTileNumber, unsigned int vEndTileNumber);
		bool __deserializeFile(const std::string& vTilePath);
		std::vector<NodeRecord> __loadTreeRecordFromFile(const std::string& vFilename);
		NodeRecord __loadNodeRecordFromFile(std::istream& vIs);
		std::shared_ptr<CTileNode> __parseRecord(const std::vector<NodeRecord>& vTreeRecord);
		void __processBrother(std::shared_ptr<CTileNode>& vRoot);
		
		std::vector<std::shared_ptr<CTileNode>> m_TileSet;
		//std::vector<std::vector<CTileNode>> m_TileNodeSet;
		std::vector<std::map<unsigned int, std::shared_ptr<CTileNode>>> m_TileNodeMap;
		
		std::string m_BinFolderPath;
		std::string m_SerializedPath;

		std::vector<tbb::concurrent_unordered_map<unsigned int, STileNodeLoadCost>> m_LoadCostMap;
		std::vector<tbb::concurrent_unordered_map<unsigned int, STileNodeLoadCost>> m_LoadCostMapRaw;

		std::vector<tbb::concurrent_unordered_map<unsigned int, std::vector<unsigned int>>> m_TileNodeAncestorMap;
		std::vector<tbb::concurrent_unordered_map<unsigned int, std::vector<unsigned int>>> m_TileNodeBrotherMap;

		std::shared_ptr<CItemTemplateCreator> m_pItemTemplateCreator;
	friend class hiveOO::CSingleton<CScene>;
	};
}

