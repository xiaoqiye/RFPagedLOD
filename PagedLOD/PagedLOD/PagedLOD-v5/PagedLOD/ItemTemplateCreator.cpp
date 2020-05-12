#include "ItemTemplateCreator.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <set>
#include <string>

using namespace hivePagedLOD;

CItemTemplateCreator::~CItemTemplateCreator()
{
	//FIXME:是否需要自己释放
	m_AllTileBaseNSet.clear();
	m_AllTileItemTemplateSet.clear();
}

//****************************************************************************
//FUNCTION:
void CItemTemplateCreator::generateAllTileItemTemplate(const std::string& vPath, unsigned int vTotalTileNum)
{
	_ASSERT(vPath && vTotalTileNum >= 0);

	loadBaseNSetByRecords(vPath, vTotalTileNum);
	m_AllTileItemTemplateSet.resize(vTotalTileNum);
	unsigned int BaseNSetSize = m_AllTileBaseNSet.size();
	for (unsigned int i = 0; i < BaseNSetSize; ++i)
	{
		//FIXME:magic number
		unsigned int TileNum = UID_TILE_NUM_MASK & m_AllTileBaseNSet[i][0][0]->getUID();
		TileNum >>= OFFSET_BIT;

		unsigned int BaseNSetISize = m_AllTileBaseNSet[i].size();
		for (unsigned int k = 0; k < BaseNSetISize; ++k)
			m_AllTileItemTemplateSet[TileNum].emplace_back(m_AllTileBaseNSet[i][k], 0, 0);
	}
}

//****************************************************************************
//FUNCTION:
void CItemTemplateCreator::loadBaseNSetByRecords(const std::string& vPath, unsigned int vTotalTileNum)
{
	_ASSERT(vPath && vTotalTileNum >= 0);

	std::vector<std::string> BaseNRecordsSet;
	boost::filesystem::directory_iterator EndItr;
	unsigned int TempCounter = 0;
	for (boost::filesystem::directory_iterator Itr(vPath); Itr != EndItr && TempCounter < vTotalTileNum; ++Itr, ++TempCounter)
		BaseNRecordsSet.push_back(Itr->path().string());
	
	//FIXME:每个Tile一个BaseN文件，需要多次打开关闭文件，效率？？
	_ASSERTE(m_AllTileBaseNSet.empty());
	for (auto& BaseNRecord : BaseNRecordsSet)
	{
		std::ifstream FileStream(BaseNRecord);
		std::string CurrentLine;
		std::vector<std::vector<std::shared_ptr<CTileNode>>> OneTileBaseNSet;
		std::getline(FileStream, CurrentLine);
		const auto Size = static_cast<unsigned int>(std::stoul(CurrentLine));
		OneTileBaseNSet.resize(Size);
		int Index = -1;
		while (std::getline(FileStream, CurrentLine))
		{
			std::string::const_iterator OneLineIterator = CurrentLine.cbegin();
			if (*OneLineIterator == '#')
				++Index;
			else
				OneTileBaseNSet[Index].emplace_back(CScene::getInstance()->getTileNodePointerByUID(static_cast<unsigned int>(std::stoul(CurrentLine))));
		}
		FileStream.close();
		m_AllTileBaseNSet.emplace_back(OneTileBaseNSet);
	}
}