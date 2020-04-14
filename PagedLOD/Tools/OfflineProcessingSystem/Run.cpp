#include ".\LODTreeGenerator\LODTree.h"
#include ".\LODTreeGenerator\LODTreeSerializer.h"

#include ".\ModelConverter\FileUtils.h"
#include ".\ModelConverter\ObjParser.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <iostream>

//const std::string hivePagedLOD::CLODTreeSerializer::OSG_FOLDER_PATH = "I:\\new\\OSGData\\";
const std::string hivePagedLOD::CLODTreeSerializer::OSG_FOLDER_PATH = "I:\\Resource\\CD\\OSG\\";
const std::string hivePagedLOD::CLODTreeSerializer::BINARY_FOLDER_PATH = "I:\\Resource\\CD\\Bin\\";
const std::string hivePagedLOD::CLODTreeSerializer::SERIALIZED_FOLDER_PATH = "I:\\new\\Serialized\\";

const std::string OBJ_SUFFIX = ".obj";
const std::string LOD_TREE_SUFFIX = ".lodtree";

//****************************************************************************
//FUNCTION:
static std::vector<std::string> getAllTileRootName(const std::string& vFolderPath)
{
	std::vector<std::string> Result;
	boost::filesystem::directory_iterator EndItr;
	for (boost::filesystem::directory_iterator Itr(vFolderPath); Itr != EndItr; ++Itr)
	{
		if (boost::filesystem::is_directory(Itr->status()))
		{
			auto TileName = Itr->path().filename().string();
			Result.push_back(TileName);
		}
	}
	return Result;
}

//****************************************************************************
//FUNCTION:
static void RunLODTreeGenerator()
{
	auto AllTileName = getAllTileRootName(hivePagedLOD::CLODTreeSerializer::OSG_FOLDER_PATH);
	for (unsigned int i = 0; i < AllTileName.size(); ++i)
	{
		auto TreeRoot = hivePagedLOD::CLODTree::buildTreeFromRootTileName(AllTileName[i]);
		hivePagedLOD::CLODTreeSerializer::saveLODTree2File(TreeRoot, hivePagedLOD::CLODTreeSerializer::SERIALIZED_FOLDER_PATH + AllTileName[i] + LOD_TREE_SUFFIX);
		std::cout << AllTileName[i] << " done" << std::endl;
	}
}

//****************************************************************************
//FUNCTION:
static void RunModelConverter()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	boost::filesystem::directory_iterator EndIt;
	for (boost::filesystem::directory_iterator It(FileUtils::ObjFilePath); It != EndIt; ++It)
	{
		if (boost::filesystem::is_directory(It->status()))
			for (boost::filesystem::directory_iterator SubIt(It->path().string()); SubIt != EndIt; ++SubIt)
			{
				if (SubIt->path().extension() == OBJ_SUFFIX && boost::filesystem::is_regular_file(SubIt->status()))
					FileUtils::generateFile(CObjParser(SubIt->path().string()));
			}
	}
}

//****************************************************************************
//FUNCTION:
static void outputAllBinFileSize()
{
	std::vector<std::string> Result;
	boost::filesystem::directory_iterator EndItr;
	uintmax_t BinSize = 0;
	uintmax_t BintSize = 0;
	for (boost::filesystem::directory_iterator Itr(hivePagedLOD::CLODTreeSerializer::BINARY_FOLDER_PATH); Itr != EndItr; ++Itr)
		if (boost::filesystem::is_directory(Itr->status()))
			for (boost::filesystem::directory_iterator Itr1(Itr->path().string() + "\\"); Itr1 != EndItr; ++Itr1)
				boost::ends_with(Itr1->path().filename().string(), ".bint") ? BintSize += file_size(Itr1->path()) : BinSize += file_size(Itr1->path());

	std::cout << "BintFileSize = " << static_cast<double>(BintSize) / 1024 / 1024 / 1024 << "GB" << std::endl;
	std::cout << "BinFileSize = " <<static_cast<double>(BinSize) / 1024 / 1024 / 1024 << "GB" << std::endl;
}

int main()
{
	RunLODTreeGenerator();
	//auto t = hivePagedLOD::CLODTreeSerializer::loadLODTreeFromFile(hivePagedLOD::CLODTreeSerializer::SERIALIZED_FOLDER_PATH + "Tile_1.lodtree");
	//RunModelConverter();
	return 0;
}