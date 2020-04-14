#include "LODTreeSerializer.h"
#include "BoundingSphere.h"

#include <fstream>
#include <iostream>

#include <boost/algorithm/string.hpp>
using namespace hivePagedLOD;

const std::string SERIALIZED_SUFFIX = ".lodtree";
const std::string FILE_NOT_FOUND_NAME = "FILE_NOT_FOUND";
const unsigned int FILE_NOT_FOUND_SIZE = 987654321;
int CLODTreeSerializer::MAX_LOD_LEVEL = 11;
int CLODTreeSerializer::MAX_LOD_LEVEL_NAME = 26;

const std::string SPLIT_MARK = "#";

//****************************************************************************
//FUNCTION:
void CLODTreeSerializer::saveLODTree2File(const std::shared_ptr<CTileNode>& vRoot, const std::string& vFilename)
{
	std::vector<NodeRecord> TreeRecord = __buildTreeRecordFromRoot(vRoot);
	std::ofstream LODTreeFileOutput;
	auto LastDot = vFilename.find_last_of('.');
	LODTreeFileOutput = LastDot == std::string::npos ? std::ofstream(vFilename + SERIALIZED_SUFFIX, std::ios::out) : std::ofstream(vFilename, std::ios::out | std::ios::binary);

	LODTreeFileOutput << TreeRecord.size() << std::endl;

	for (NodeRecord& Record : TreeRecord)
	{
		__saveNodeRecord2File(LODTreeFileOutput, Record);
	}
	LODTreeFileOutput.close();
}

//****************************************************************************
//FUNCTION:
std::vector<NodeRecord> CLODTreeSerializer::__buildTreeRecordFromRoot(const std::shared_ptr<CTileNode>& vRoot)
{
	std::vector<NodeRecord> TreeRecord;
	__recordTree(TreeRecord, vRoot, -1);
	return TreeRecord;
}

//****************************************************************************
//FUNCTION:
void CLODTreeSerializer::__recordTree(std::vector<NodeRecord>& voTR, const std::shared_ptr<CTileNode>& vCur, int vFather)
{
	CTileNode Temp = *vCur;
	Temp.clearChildren();
	voTR.emplace_back(NodeRecord(Temp, vFather));
	if (vCur->getNumChildren() != 0)
	{
		auto CurIndex = voTR.size() - 1;
		for (unsigned int i = 0; i < vCur->getNumChildren(); ++i)
		{
			__recordTree(voTR, vCur->getChildPointerAt(i), static_cast<int>(CurIndex));
		}
	}
}

//****************************************************************************
//FUNCTION:
void CLODTreeSerializer::__saveNodeRecord2File(std::ostream& voOs, const NodeRecord& vNR)
{
	auto BoundingSphere = vNR.first.getBoundingSphere();	//won't save this but the x/y/z/r values

	unsigned int UID = vNR.first.getUID();
	float
		X = BoundingSphere.getCenter().x,
		Y = BoundingSphere.getCenter().y,
		Z = BoundingSphere.getCenter().z,
		R = BoundingSphere.getRadius();
	std::string GeometryFileName = vNR.first.getGeometryFileName();
	uintmax_t GeometryFileSize = vNR.first.getGeometryFileSize();
	std::string TextureFileName = vNR.first.getTextureFileName();
	uintmax_t TextureFileSize = vNR.first.getTextureFileSize();
	uintmax_t TriangleCount = vNR.first.getTriangleCount();
	int LODLevel = __generateLODLevelByFilename(GeometryFileName);
	unsigned int NodeDeep = vNR.first.getNodeDeep();
	std::vector<unsigned int> AncestorSet = vNR.first.getAncestorUIDSet();
	std::vector<unsigned int> BrotherSet = vNR.first.getBrotherUIDSet();

	std::string AncestorUIDSet;
	for (auto& i : AncestorSet)
		AncestorUIDSet += std::to_string(i) + SPLIT_MARK;
	AncestorUIDSet.pop_back();
	
	std::string BrotherUIDSet;
	for (auto& i : BrotherSet)
		BrotherUIDSet += std::to_string(i) + SPLIT_MARK;
	BrotherUIDSet.pop_back();
	
	voOs << UID << std::endl
		<< X << std::endl
		<< Y << std::endl
		<< Z << std::endl
		<< R << std::endl
		<< GeometryFileName << std::endl
		<< GeometryFileSize << std::endl
		<< TextureFileName << std::endl
		<< TextureFileSize << std::endl
		<< TriangleCount << std::endl
		<< NodeDeep << std::endl
		<< LODLevel << std::endl
		<< vNR.second << std::endl
		<< AncestorUIDSet << std::endl
		<< BrotherUIDSet << std::endl;
}

//****************************************************************************
//FUNCTION:
int CLODTreeSerializer::__generateLODLevelByFilename(const std::string& vFilename)
{
	//find filename second '_'
	int Level = 0;
	std::string name = vFilename;					//	\\Tile_1_L17_00_0.bin
	name = name.substr(name.find_last_of('\\') + 1);//	Tile_1_L17_00_0.bin
	name = name.substr(name.find_first_of('_') + 1);//	1_L17_00_0.bin
	name = name.substr(name.find_first_of('_') + 1);//	L17_00_0.bin
	if (name[0] != 'L')								//	Tile_1_0.bin  ASSERT(name[0] == '0')
	{
		Level = CLODTreeSerializer::MAX_LOD_LEVEL;
	}
	else
	{
		name = name.substr(1, 2);//17
		Level = CLODTreeSerializer::MAX_LOD_LEVEL_NAME - std::stoi(name);
	}
	return Level;
}

//****************************************************************************
//FUNCTION:
std::shared_ptr<CTileNode> CLODTreeSerializer::loadLODTreeFromFile(const std::string& vFilename)
{
	try
	{
		std::vector<NodeRecord> TreeRecord = __loadTreeRecordFromFile(vFilename);
		return __parseRecord(TreeRecord);
	}
	catch (std::exception & Exception)
	{
		std::cout << Exception.what() << std::endl;
		_ASSERT(false);
	}
	return nullptr;
}

//****************************************************************************
//FUNCTION:
std::vector<NodeRecord> CLODTreeSerializer::__loadTreeRecordFromFile(const std::string& vFilename)
{
	if (vFilename.substr(vFilename.find_last_of('.')) != SERIALIZED_SUFFIX) throw std::exception("Wrong file type!");

	std::ifstream LODTreeFileInput(vFilename, std::ios::in);
	if (!LODTreeFileInput) throw std::exception("Can't open this file!");

	std::vector<NodeRecord> TreeRecord;

	int TotalRecordNum;
	LODTreeFileInput >> TotalRecordNum;

	for (int i = 0; i < TotalRecordNum; ++i)
	{
		TreeRecord.emplace_back(__loadNodeRecordFromFile(LODTreeFileInput));
	}

	LODTreeFileInput.close();
	return TreeRecord;
}

//****************************************************************************
//FUNCTION:
NodeRecord CLODTreeSerializer::__loadNodeRecordFromFile(std::istream& vIs)
{
	NodeRecord Result;
	unsigned int UID;
	float X, Y, Z, R;
	std::string GeometryFileName;
	uintmax_t GeometryFileSize;
	std::string TextureFileName;
	uintmax_t TextureFileSize;
	uintmax_t TriangleCount;
	unsigned int NodeDeep;
	int LODLevel;
	int Father;
	std::string AncestorString;
	std::string BrotherString;
	
	vIs >> UID;
	vIs >> X >> Y >> Z >> R;
	vIs.get();
	getline(vIs, GeometryFileName);
	vIs >> GeometryFileSize;
	vIs.get();
	getline(vIs, TextureFileName);
	vIs >> TextureFileSize;
	vIs >> TriangleCount;
	vIs >> NodeDeep >> LODLevel >> Father;
	vIs >> AncestorString;
	vIs >> BrotherString;

	std::vector<std::string> AncestorSet;
	boost::split(AncestorSet, AncestorString, boost::is_any_of(SPLIT_MARK), boost::token_compress_on);
	std::vector<unsigned int> AncestorUIDSet;
	for (auto& i : AncestorSet)
		AncestorUIDSet.emplace_back(static_cast<unsigned int>(std::stoul(i)));

	std::vector<std::string> BrotherSet;
	boost::split(BrotherSet, BrotherString, boost::is_any_of(SPLIT_MARK), boost::token_compress_on);
	std::vector<unsigned int> BrotherUIDSet;
	for (auto& i : BrotherSet)
		BrotherUIDSet.emplace_back(static_cast<unsigned int>(std::stoul(i)));

	Result.first.setUID(UID);
	Result.first.setBoundingSphere(CBoundingSphere(X, Y, Z, R));
	Result.first.setGeometryFileName(BINARY_FOLDER_PATH + GeometryFileName);
	Result.first.setTextureFileName(BINARY_FOLDER_PATH + TextureFileName);
	Result.first.setGeometryFileSize(GeometryFileSize);
	Result.first.setTextureFileSize(TextureFileSize);
	Result.first.setTriangleCount(TriangleCount);
	Result.first.setNodeDeep(NodeDeep);
	Result.first.setLODLevel(LODLevel);
	Result.second = Father;
	Result.first.setAncestorUIDSet(AncestorUIDSet);
	Result.first.setBrotherUIDSet(BrotherUIDSet);

	return Result;
}

//****************************************************************************
//FUNCTION:
std::shared_ptr<CTileNode> CLODTreeSerializer::__parseRecord(const std::vector<NodeRecord>& vTreeRecord)
{
	std::vector<std::shared_ptr<CTileNode>> Result;
	for (auto& Record : vTreeRecord)
	{
		Result.emplace_back(std::make_shared<CTileNode>(Record.first));
	}
	auto n = Result.size();
	for (int i = 1; i < n; ++i)
	{
		if (Result[i]->getGeometryFileSize() == FILE_NOT_FOUND_SIZE && Result[i]->getTextureFileSize() == FILE_NOT_FOUND_SIZE)
			continue;
		Result[i]->setParent(Result[vTreeRecord[i].second]);
		Result[vTreeRecord[i].second]->addChild(Result[i]);
	}
	return Result[0];
}