#pragma once
#include "TileNode.h"

namespace hivePagedLOD
{
	typedef std::pair<CTileNode, int> NodeRecord;

	class CLODTreeSerializer  //REVIEW: 为什么不使用boost的序列化功能？
	{
	public:
		static void saveLODTree2File(const std::shared_ptr<CTileNode>& vRoot, const std::string& vFilename);
		static std::shared_ptr<CTileNode> loadLODTreeFromFile(const std::string& vFilename);
		static void setMAXLODLEVEL(int vLevel) { MAX_LOD_LEVEL = vLevel; };
		static void setMAXLODLEVELNAME(int vLevel) { MAX_LOD_LEVEL_NAME = vLevel; };
		static const std::string OSG_FOLDER_PATH;
		static const std::string BINARY_FOLDER_PATH;
		static const std::string SERIALIZED_FOLDER_PATH;

		static int MAX_LOD_LEVEL;
		static int MAX_LOD_LEVEL_NAME;	
	private:
		static std::vector<NodeRecord> __buildTreeRecordFromRoot(const std::shared_ptr<CTileNode>& vRoot);
		static void __recordTree(std::vector<NodeRecord>& voTR, const std::shared_ptr<CTileNode>& vCur, int vFather);
		static void __saveNodeRecord2File(std::ostream& voOs, const NodeRecord& vNR);
		static int __generateLODLevelByFilename(const std::string& vFilename);
		static std::vector<NodeRecord> __loadTreeRecordFromFile(const std::string& vFilename);
		static NodeRecord __loadNodeRecordFromFile(std::istream& vIs);
		static std::shared_ptr<CTileNode> __parseRecord(const std::vector<NodeRecord>& vTreeRecord);
	};
}