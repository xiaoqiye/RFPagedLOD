#include "Scene.h"
#include "BoundingSphere.h"

#include <boost\filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <fstream>
#include <string>
#include <iostream>

using namespace hivePagedLOD;

const std::string SERIALIZED_SUFFIX = ".lodtree";
const std::string FILE_NOT_FOUND_NAME = "FILE_NOT_FOUND";
const unsigned int FILE_NOT_FOUND_SIZE = 987654321;
const std::string SPLIT_MARK = "#";

//****************************************************************************
//FUNCTION:
CScene::~CScene()
{
	//FIXME:是否需要自己释放
	//m_TileNodeSet.clear();
	m_TileNodeMap.clear();
	m_TileSet.clear();
}

//****************************************************************************
//FUNCTION:
bool CScene::init(const std::string& vSerializedPath, const std::string& vBinFolderPath, const std::string& vBaseNPath, bool vLegalTileName, unsigned int vBeginTileNumber, unsigned int vEndTileNumber, int vTraverseMaxDeep)
{
	_ASSERTE(vBeginTileNumber <= vEndTileNumber);
	__initMap(vEndTileNumber);

	_ASSERTE(boost::filesystem::exists(vBinFolderPath));
	m_BinFolderPath = vBinFolderPath;

	_ASSERTE(boost::filesystem::exists(vSerializedPath));
	m_SerializedPath = vSerializedPath;

	bool Result = false;
	if (vLegalTileName && vBeginTileNumber <= vEndTileNumber)
		Result = __initRange(vBeginTileNumber, vEndTileNumber);
	else
		Result = __initAll();

	m_LoadCostMapRaw = m_LoadCostMap;
	m_pItemTemplateCreator = std::make_shared<CItemTemplateCreator>(vTraverseMaxDeep);
	m_pItemTemplateCreator->generateAllTileItemTemplate(vBaseNPath, m_TileSet.size());
	return Result;
}

//****************************************************************************
//FUNCTION:
const std::shared_ptr<CTileNode>& CScene::getTileNodePointerByUID(unsigned vUID)
{
	_ASSERT(vUID >= 0);

	unsigned int TileNum = UID_TILE_NUM_MASK & vUID;
	TileNum >>= OFFSET_BIT;
	return m_TileNodeMap[TileNum][vUID];
}

//****************************************************************************
//FUNCTION:
void CScene::resetTileNodeStatus(const std::vector<unsigned int>& vTileNumSet)
{
	_ASSERT(vTileNumSet.size() > 0);

	for (unsigned int i = 0; i < vTileNumSet.size(); ++i)
		for (auto& TileNode : m_TileNodeMap[vTileNumSet[i]])
			TileNode.second->resetStatus();
}

//****************************************************************************
//FUNCTION:
void CScene::outputLoadCostMap(const std::string& vFunctionName)
{
	_ASSERT(!vFunctionName.empty());

	for (auto& TileLoadCostSet : m_LoadCostMap)
		for (auto& TileNodeCost : TileLoadCostSet)
			std::cout << vFunctionName << TileNodeCost.first << " " << TileNodeCost.second.GeoInMemory << " " << TileNodeCost.second.GeoSize << " " << TileNodeCost.second.TexInMemory << " " << TileNodeCost.second.TexSize << " " << TileNodeCost.second.LoadCost << std::endl;
}

//****************************************************************************
//FUNCTION:
bool CScene::__initMap(unsigned int vMaxTileNum)
{
	_ASSERT(vMaxTileNum > 0);

	m_LoadCostMapRaw.clear();
	m_LoadCostMap.clear();
	m_TileNodeAncestorMap.clear();
	m_TileNodeBrotherMap.clear();

	const size_t Size = static_cast<size_t>(vMaxTileNum) + 1;//tile num start with 1
	m_LoadCostMapRaw.resize(Size);
	m_LoadCostMap.resize(Size);
	m_TileNodeAncestorMap.resize(Size);
	m_TileNodeBrotherMap.resize(Size);
	m_TileSet.resize(Size);
	m_TileNodeMap.resize(Size);
	return true;
}

//****************************************************************************
//FUNCTION:
bool CScene::__initAll()
{
	const boost::filesystem::directory_iterator EndItr;

	for (boost::filesystem::directory_iterator it(m_SerializedPath); it != EndItr; ++it)
		if (boost::filesystem::is_regular_file(it->status()))
			if (!__deserializeFile(it->path().string()))
				throw std::exception("DeserializeFile failed! ");
		
	return true;
}

//****************************************************************************
//FUNCTION:
bool CScene::__initRange(unsigned int vBeginTileNum, unsigned int vEndTileNum)
{
	_ASSERT(vBeginTileNum <= vEndTileNum);

	for (unsigned int i = vBeginTileNum; i <= vEndTileNum; ++i)
	{
		std::string TileName = "Tile_" + std::to_string(i);
		std::string FilePath = m_SerializedPath + TileName + SERIALIZED_SUFFIX;
		if (boost::filesystem::exists(FilePath))
			if (!__deserializeFile(FilePath))
				throw std::exception("DeserializeFile failed! ");
	}
	return true;
}

//****************************************************************************
//FUNCTION:
bool CScene::__deserializeFile(const std::string& vTilePath)
{
	_ASSERT(!vTilePath.empty());

	auto TileRoot = __parseRecord(__loadTreeRecordFromFile(vTilePath));
	__processBrother(TileRoot->fetchChildPointerAt(0));
	
	unsigned int TileNum = UID_TILE_NUM_MASK & TileRoot->getUID();
	TileNum >>= OFFSET_BIT;
	m_TileSet[TileNum] = TileRoot;
	return true;
}

//****************************************************************************
//FUNCTION:
std::vector<NodeRecord> CScene::__loadTreeRecordFromFile(const std::string& vFilename)
{
	_ASSERT(!vFilename.empty());

	if (vFilename.substr(vFilename.find_last_of('.')) != SERIALIZED_SUFFIX) throw std::exception("Wrong file type!");
	std::ifstream LODTreeFileInput(vFilename, std::ios::in);
	if (!LODTreeFileInput) throw std::exception("Can't open this file!");

	std::vector<NodeRecord> TreeRecordSet;
	int TotalRecordNum;
	LODTreeFileInput >> TotalRecordNum;
	for (int i = 0; i < TotalRecordNum; ++i)
		TreeRecordSet.emplace_back(__loadNodeRecordFromFile(LODTreeFileInput));

	LODTreeFileInput.close();
	return TreeRecordSet;
}

//****************************************************************************
//FUNCTION:
NodeRecord CScene::__loadNodeRecordFromFile(std::istream& vIs)
{
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
	std::vector<unsigned int> AncestorIDSet;
	for (auto& i : AncestorSet)
		AncestorIDSet.emplace_back(static_cast<unsigned int>(std::stoul(i)));

	std::vector<std::string> BrotherSet;
	boost::split(BrotherSet, BrotherString, boost::is_any_of(SPLIT_MARK), boost::token_compress_on);
	std::vector<unsigned int> BrotherUIDSet;
	for (auto& i : BrotherSet)
		BrotherUIDSet.emplace_back(static_cast<unsigned int>(std::stoul(i)));

	unsigned int TileNum = UID_TILE_NUM_MASK & UID;
	TileNum >>= OFFSET_BIT;

	auto Pointer = std::make_shared<CTileNode>();
	Pointer->setUID(UID);
	Pointer->setBoundingSphere(CBoundingSphere(X, Y, Z, R));
	Pointer->setGeometryFileName(m_BinFolderPath + GeometryFileName);
	Pointer->setTextureFileName(m_BinFolderPath + TextureFileName);
	Pointer->setGeometryFileSize(GeometryFileSize);
	Pointer->setTextureFileSize(TextureFileSize);
	Pointer->setTriangleCount(TriangleCount);
	Pointer->setNodeDeep(NodeDeep);
	Pointer->setLODLevel(LODLevel);
	Pointer->setAncestorUIDSet(AncestorIDSet);
	Pointer->setBrotherUIDSet(BrotherUIDSet);
	
	if (m_LoadCostMap.size() != 0)
	{
		m_LoadCostMap[TileNum][UID] = STileNodeLoadCost(GeometryFileSize - GEOMETRY_HEADER_LENGTH, TextureFileSize - TEXTURE_HEADER_LENGTH, TriangleCount);
		m_TileNodeAncestorMap[TileNum][UID] = AncestorIDSet;
		m_TileNodeBrotherMap[TileNum][UID] = BrotherUIDSet;

		m_TileNodeMap[TileNum].insert(std::make_pair(UID, Pointer));
	}
	return std::make_pair(Pointer,Father);
}

//****************************************************************************
//FUNCTION:
std::shared_ptr<CTileNode> CScene::__parseRecord(const std::vector<NodeRecord>& vTreeRecord)
{
	if (vTreeRecord.empty()) return nullptr;
	std::vector<std::shared_ptr<CTileNode>> Result;
	for (auto& Record : vTreeRecord)
	{
		Result.emplace_back(Record.first);
	}
	auto n = Result.size();
	unsigned int TileNum = UID_TILE_NUM_MASK & vTreeRecord[0].first->getUID();
	TileNum >>= OFFSET_BIT;
	
	for (unsigned int i = 1; i < n; ++i)
	{
		if (Result[i]->getGeometryFileSize() == FILE_NOT_FOUND_SIZE && Result[i]->getTextureFileSize() == FILE_NOT_FOUND_SIZE)
			continue;
		Result[i]->setParent(Result[vTreeRecord[i].second]);
		Result[vTreeRecord[i].second]->addChild(Result[i]);
	}
	return Result[0];
}

void CScene::__processBrother(std::shared_ptr<CTileNode>& vTileNode)
{
	if (!vTileNode) return;

	auto Parent = vTileNode->getParent().lock();
	for (unsigned int i = 0; i < Parent->getNumChildren(); ++i)
	{
		auto& temp = Parent->getChildPointerAt(i);
		if (temp->getUID() != vTileNode->getUID())
			vTileNode->addBrother(temp);
	}
	for (unsigned int i = 0; i < vTileNode->getNumChildren(); ++i)
		__processBrother(vTileNode->fetchChildPointerAt(i));
}