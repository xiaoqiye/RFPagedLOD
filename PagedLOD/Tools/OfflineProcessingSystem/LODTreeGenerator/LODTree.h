#pragma once
#include "TileNode.h"
#include <string>
#include <memory>
#include <boost/asio.hpp>

namespace hivePagedLOD
{
	class CLODTree
	{
	public:
		static std::shared_ptr<CTileNode> buildTreeFromRootTileName(const std::string& vRootTileName);
		static void traverseTree(const std::shared_ptr<CTileNode>& vRoot, std::vector<std::shared_ptr<CTileNode>>& voTileSet);
	private:
		static std::vector<std::shared_ptr<CTileNode>> __parseTileFromFilePath(std::shared_ptr<CTileNode>& vRoot, const std::string& vFilePath, const std::string& vBinDirPath);
		static void __buildTreeRecursively(std::shared_ptr<CTileNode> vParent, const std::string& vChildPath, const std::string& vBinFolderPath);

		static unsigned int __getTileNumByOpenFile(const std::string& vFilename);
		static void __setGeoTex(unsigned int vI, const std::string& vFilename, const std::string& vBinDirPath, std::shared_ptr<CTileNode>& voTile);
		static std::vector<std::shared_ptr<CTileNode>> __parseFile(std::shared_ptr<CTileNode>& vTile, unsigned int vTileCount, const boost::filesystem::path& vFilePath, const std::string& vBinDirPath);
		static void __parsePagedLOD(std::ifstream& vFile, const std::string& vPathPrefix, std::vector<std::shared_ptr<CTileNode>>& voTileList);
		static void __postProcess(std::shared_ptr<CTileNode>& vRoot, const std::string& vTileName);
		static void __generateUIDNodeDeepAncestorSet(unsigned int vTileNumMask, std::shared_ptr<CTileNode>& voRoot, unsigned int& voID, unsigned int vDeep);
		static void __generateBrotherSet(std::shared_ptr<CTileNode>& voRoot);

		static boost::asio::thread_pool ThreadPool;
		static const std::size_t WORD_SIZE_THIRTEEN = 13;
		static const std::size_t WORD_SIZE_SEVEN = 7;
		static const std::size_t WORD_SIZE_TWENTY_SEVEN = 27;
	};
}
