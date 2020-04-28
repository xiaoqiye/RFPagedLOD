#include "ItemTemplateCreator.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <set>
#include <string>

using namespace hivePagedLOD;

CItemTemplateCreator::~CItemTemplateCreator()
{
	m_AllTileBaseNSet.clear();
	m_AllTileItemTemplateSet.clear();
}

//****************************************************************************
//FUNCTION:
void CItemTemplateCreator::generateAllTileItemTemplate(const std::vector<std::shared_ptr<CTileNode>>& vRootSet, const std::string& vPath, unsigned int vTotalTileNum)
{
	loadBaseNSetByRecords(vPath, vTotalTileNum);
	m_AllTileItemTemplateSet.resize(vTotalTileNum);
	for (unsigned int i = 0; i < m_AllTileBaseNSet.size(); ++i)
	{
		uintmax_t LoadCost = 0, TriangleCount = 0;
		unsigned int TileNum = UID_TILE_NUM_MASK & m_AllTileBaseNSet[i][0][0]->getUID();
		TileNum >>= OFFSET_BIT;
		for (unsigned int k = 0; k < m_AllTileBaseNSet[i].size(); ++k)
		{
			m_AllTileItemTemplateSet[TileNum].emplace_back(m_AllTileBaseNSet[i][k], 0, 0);
		}
	}
}

//****************************************************************************
//FUNCTION:
void CItemTemplateCreator::loadBaseNSetByRecords(const std::string& vPath, unsigned int vTotalTileNum)
{
	std::vector<std::string> Result;
	boost::filesystem::directory_iterator EndItr;
	unsigned int i = 0;
	for (boost::filesystem::directory_iterator Itr(vPath); Itr != EndItr && i < vTotalTileNum; ++Itr)
	{
		auto TileName = Itr->path().string();
		Result.push_back(TileName);
		++i;
	}

	_ASSERTE(m_AllTileBaseNSet.empty());
	for (auto& i : Result)
	{
		std::ifstream FileStream(i);
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
			{
				auto UID = static_cast<unsigned int>(std::stoul(CurrentLine));
				OneTileBaseNSet[Index].emplace_back(CScene::getInstance()->getTileNodePointerByUID(UID));
			}
		}
		FileStream.close();
		m_AllTileBaseNSet.emplace_back(OneTileBaseNSet);
	}
}