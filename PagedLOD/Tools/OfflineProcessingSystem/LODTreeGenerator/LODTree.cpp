#include "LODTree.h"
#include "LODTreeSerializer.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>

using namespace hivePagedLOD;

boost::asio::thread_pool CLODTree::ThreadPool(4);

//应对如Tile_104_L26_00000102100_0.osg这种生成只有.bint而没有.bin的Tile的特殊情况，
//建树和序列化时不处理，反序列化时处理，不让其被反序列化出来
const std::string FILE_NOT_FOUND_NAME = "FILE_NOT_FOUND";
const unsigned int FILE_NOT_FOUND_SIZE = 987654321;
const unsigned int MAX_TREE_DEEP = 16;
const unsigned int BASE_MASK = 0x00FFFFFF;
const unsigned int PREFIX_OFFSET = 5;
const unsigned int OFFSET_BIT = 24;
const unsigned int MAX_BROTHER_NUM = 8;
//****************************************************************************
//FUNCTION:
std::shared_ptr<CTileNode> CLODTree::buildTreeFromRootTileName(const std::string& vRootTileName)
{
	std::string FolderName = vRootTileName;
	std::string OSGFilePath = CLODTreeSerializer::OSG_FOLDER_PATH + FolderName + "\\" + vRootTileName + ".osg";
	std::string BinFolderPath = CLODTreeSerializer::BINARY_FOLDER_PATH + FolderName + "\\";

	auto pRootParent = std::make_shared<CTileNode>();
	auto pRoot = std::make_shared<CTileNode>();
	pRootParent->addChild(pRoot);
	try
	{
		__parseTileFromFilePath(pRootParent, OSGFilePath, BinFolderPath);
	}
	catch (std::exception& Exception)
	{
		std::cout << Exception.what() << std::endl;
		_ASSERT(false);
		return nullptr;
	}

	std::vector<std::thread> threads;
	for (unsigned int i = 0; i < pRoot->getNumChildren(); ++i)
	{
		threads.emplace_back([=] { __buildTreeRecursively(pRoot, pRoot->getChildPointerAt(i)->getCreatedByFileName(), BinFolderPath); });
	}
	for (auto &thread : threads)
	{
		thread.join();
	}

	__postProcess(pRoot, FolderName);
	
	return pRoot;
}


//****************************************************************************
//FUNCTION:
void CLODTree::__postProcess(std::shared_ptr<CTileNode>& vRoot, const std::string& vTileName)
{
	std::vector<std::pair<CTileNode, unsigned int>> TreeRecordSet;

	const auto TileNum = vTileName.substr(PREFIX_OFFSET, vTileName.size() - PREFIX_OFFSET);
	const unsigned int TileNumMask = BASE_MASK | static_cast<unsigned int>((std::stoi(TileNum, nullptr)) << OFFSET_BIT);

	unsigned int ID = 0;
	__generateUIDNodeDeepAncestorSet(TileNumMask, vRoot, ID, 0);

	__generateBrotherSet(vRoot);
}

//****************************************************************************
//FUNCTION:
void CLODTree::__generateUIDNodeDeepAncestorSet(unsigned int vTileNumMask, std::shared_ptr<CTileNode>& voRoot, unsigned int& voID, unsigned int vDeep)
{
	////UID
	const unsigned int UID = (voID | ~BASE_MASK) & vTileNumMask;
	voRoot->setUID(UID);
	++voID;
	////Deep
	voRoot->setNodeDeep(vDeep);
	const auto CurrentDeep = ++vDeep;
	////AncestorSet
	auto CopyTileNode = voRoot;
	std::vector<unsigned int> AncestorSet(MAX_TREE_DEEP);
	while (CopyTileNode->getParent().lock())
	{
		CopyTileNode = CopyTileNode->getParent().lock();
		AncestorSet[CopyTileNode->getNodeDeep()] = CopyTileNode->getUID();
	}
	voRoot->setAncestorUIDSet(AncestorSet);

	if (voRoot->getNumChildren() != 0)
		for (unsigned int i = 0; i < voRoot->getNumChildren(); ++i)
			__generateUIDNodeDeepAncestorSet(vTileNumMask, voRoot->fetchChildPointerAt(i), voID, CurrentDeep);

}

//****************************************************************************
//FUNCTION:
void CLODTree::__generateBrotherSet(std::shared_ptr<CTileNode>& voRoot)
{
	////BrotherSet
	std::vector<unsigned int> BrotherSet(MAX_BROTHER_NUM);
	const auto ParentTileNode = voRoot->getParent().lock();
	if (ParentTileNode)
	{
		for (unsigned int i = 0; i < ParentTileNode->getNumChildren(); ++i)
		{
			if (voRoot->getGeometryFileName() != ParentTileNode->getChildPointerAt(i)->getGeometryFileName())
			{
				BrotherSet[i] = ParentTileNode->getChildPointerAt(i)->getUID();
			}
		}
	}
	voRoot->setBrotherUIDSet(BrotherSet);
	if (voRoot->getNumChildren() != 0)
		for (unsigned int i = 0; i < voRoot->getNumChildren(); ++i)
			__generateBrotherSet(voRoot->fetchChildPointerAt(i));
}

//****************************************************************************
//FUNCTION:
void CLODTree::traverseTree(const std::shared_ptr<CTileNode>& vRoot, std::vector<std::shared_ptr<CTileNode>>& voTileSet)
{
	if (!vRoot)
	{
		return;
	}
	voTileSet.emplace_back(vRoot);
	for (unsigned int i = 0; i < vRoot->getNumChildren(); ++i)
	{
		traverseTree(vRoot->getChildPointerAt(i), voTileSet);
	}
}

//******************************************************************
//FUNCTION:
void CLODTree::__buildTreeRecursively(std::shared_ptr<CTileNode> vParent, const std::string& vChildPath, const std::string& vBinFolderPath)
{
	if (vParent->getNumChildren() == 0) return;

	std::vector<std::shared_ptr<CTileNode>> ChildrenList;
	try
	{
		ChildrenList = __parseTileFromFilePath(vParent, vChildPath, vBinFolderPath);
	}
	catch (std::exception& Exception)
	{
		std::cout << Exception.what() << std::endl;
		_ASSERT(false);
		return;
	}

	std::vector<std::thread> threads;

	for (auto& Child : ChildrenList)
	{
		if (Child->getNumChildren() == 0) continue;
		threads.emplace_back([=] { __buildTreeRecursively(Child, Child->getChildAt(0).getCreatedByFileName(), vBinFolderPath); });
	}

	for (auto& thread : threads)
	{
		thread.join();
	}
}

//******************************************************************
//FUNCTION:
std::vector<std::shared_ptr<CTileNode>> CLODTree::__parseTileFromFilePath(std::shared_ptr<CTileNode>& vRoot, const std::string& vFilePath, const std::string& vBinDirPath)
{
	boost::filesystem::path FilePathOsg(vFilePath);
	if (!boost::filesystem::exists(FilePathOsg)) throw std::exception("no such file or directory");

	auto TileCount = __getTileNumByOpenFile(FilePathOsg.string());

	return __parseFile(vRoot, TileCount, FilePathOsg, vBinDirPath);
}

//****************************************************************************
//FUNCTION:
unsigned int hivePagedLOD::CLODTree::__getTileNumByOpenFile(const std::string& vFilename)
{
	unsigned int ChildNum = 0;
	std::ifstream FileStream(vFilename);
	std::string CurrentLine;

	while (std::getline(FileStream, CurrentLine))
	{
		std::string::const_iterator OneLineIterator = CurrentLine.cbegin();
		for (; *OneLineIterator == ' '; ++OneLineIterator);
		if (*OneLineIterator == 'n')
		{
			boost::trim_left(CurrentLine);
			if (boost::starts_with(CurrentLine, "num_children"))
			{
				ChildNum = std::stoi(CurrentLine.substr(WORD_SIZE_THIRTEEN));
				FileStream.close();
				return ChildNum;
			}
		}
	}
	return ChildNum;
}

//****************************************************************************
//FUNCTION:
std::vector<std::shared_ptr<CTileNode>> CLODTree::__parseFile(std::shared_ptr<CTileNode>& vTile, unsigned int vTileCount, const boost::filesystem::path& vFilePath, const std::string& vBinDirPath)
{
	std::ifstream FileStream(vFilePath.string());
	std::string FilePath = vFilePath.filename().string();

	for (unsigned int i = 0; i < vTileCount; ++i)
	{
		vTile->getChildAt(i).setCreatedByFileName(vFilePath.string());
		__setGeoTex(i, FilePath, vBinDirPath, vTile->fetchChildPointerAt(i));
	}
	__parsePagedLOD(FileStream, vFilePath.parent_path().string(), vTile->fetchAllChildren());
	return vTile->fetchAllChildren();
}

//****************************************************************************
//FUNCTION:
void CLODTree::__setGeoTex(unsigned int vI, const std::string& vFilename, const std::string& vBinDirPath, std::shared_ptr<CTileNode>& voTile)
{
	boost::filesystem::path FilePathOsg(vFilename);
	FilePathOsg = FilePathOsg.replace_extension(".bin");
	auto GeometryFilename = FilePathOsg.filename().string();
	FilePathOsg = FilePathOsg.replace_extension(".bint");
	auto TextureFilename = FilePathOsg.filename().string();
	auto InsterPos = GeometryFilename.find_last_of('.');
	GeometryFilename.insert(InsterPos, "_" + std::to_string(vI));
	auto GeoPath = vBinDirPath + GeometryFilename;
	auto TexPath = vBinDirPath + TextureFilename;

	boost::filesystem::path BinDirPath(vBinDirPath);
	auto TileName = BinDirPath.parent_path().filename().string() + "\\";

	bool GeoExists = boost::filesystem::exists(GeoPath);
	bool TexExists = boost::filesystem::exists(TexPath);
	if (GeoExists && TexExists)//normal tile
	{
		voTile->setGeometryFileName(TileName + GeometryFilename);
		voTile->setGeometryFileSize(boost::filesystem::file_size(GeoPath));
		voTile->setTextureFileName(TileName + TextureFilename);
		voTile->setTextureFileSize(boost::filesystem::file_size(TexPath));
	}
	else if (!GeoExists && TexExists)//special tile
	{
		voTile->setGeometryFileName(FILE_NOT_FOUND_NAME);
		voTile->setGeometryFileSize(FILE_NOT_FOUND_SIZE);
		voTile->setTextureFileName(FILE_NOT_FOUND_NAME);
		voTile->setTextureFileSize(FILE_NOT_FOUND_SIZE);
		std::cout << GeoPath << " not found" << std::endl;
	}
	else if (!GeoExists && !TexExists)//error
	{
		_ASSERTE(false);
	}
}

//****************************************************************************
//FUNCTION:
void CLODTree::__parsePagedLOD(std::ifstream& vFile, const std::string& vPathPrefix, std::vector<std::shared_ptr<CTileNode>>& voTileList)
{
	glm::vec3 TempCenter;
	std::string CurrentLine;
	unsigned int ID = 0;
	unsigned int ResultSize = static_cast<unsigned int>(voTileList.size());
	while (ID != ResultSize && std::getline(vFile, CurrentLine))
	{
		std::string::const_iterator OneLineIterator = CurrentLine.cbegin();
		for (; *OneLineIterator == ' '; ++OneLineIterator);
		if (*OneLineIterator == 'C' || *OneLineIterator == 'R' || *OneLineIterator == 'F' || *OneLineIterator == 'D')
		{
			boost::trim_left(CurrentLine);
			if (boost::starts_with(CurrentLine, "Center"))
			{
				std::string centerStr(CurrentLine.substr(WORD_SIZE_SEVEN));
				std::vector<std::string> centerVec;
				boost::split(centerVec, centerStr, boost::is_space());
				TempCenter = glm::vec3(std::stof(centerVec[0]), std::stof(centerVec[1]), std::stof(centerVec[2]));
			}
			else if (boost::starts_with(CurrentLine, "Radius"))
			{
				voTileList[ID]->setBoundingSphere(CBoundingSphere(TempCenter, std::stof(CurrentLine.substr(WORD_SIZE_SEVEN))));
			}
			else if (boost::starts_with(CurrentLine, "FileNameList"))
			{
				// jump two lines
				std::getline(vFile, CurrentLine);
				std::getline(vFile, CurrentLine);

				boost::trim(CurrentLine);
				CurrentLine.pop_back();
				auto ChildrenNum = __getTileNumByOpenFile(vPathPrefix + "\\" + CurrentLine);
				CurrentLine += "b";
				for (unsigned int i = 0; i < ChildrenNum; ++i)
				{
					std::string temp = CurrentLine;
					voTileList[ID]->addChild(std::make_shared<CTileNode>());
					voTileList[ID]->getChildAt(i).setParent(voTileList[ID]);
					temp.pop_back();
					voTileList[ID]->getChildAt(i).setCreatedByFileName(vPathPrefix + "\\" + temp);
				}
			}
			else if (boost::starts_with(CurrentLine, "DrawElementsUInt"))
			{
				voTileList[ID]->setTriangleCount(std::stoi(CurrentLine.substr(WORD_SIZE_TWENTY_SEVEN)));
				++ID;
			}
		}
	}
}