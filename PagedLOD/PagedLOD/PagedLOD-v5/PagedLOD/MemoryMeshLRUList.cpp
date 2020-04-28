#include "MemoryMeshLRUList.h"
#include "Utils.h"

using namespace hivePagedLOD;

CMemoryMeshLRUList::CMemoryMeshLRUList(CMemoryBufferManager* vMemoryBufferManager)
{
	m_pHostMemoryBufferManager = vMemoryBufferManager;
}

//****************************************************************************
//FUNCTION:
void CMemoryMeshLRUList::deleteExpiredTileNode()
{
	auto NewMemoryCount = m_pHostMemoryBufferManager->getUsedMemoryCount();
	while (NewMemoryCount > m_Limit)
	{
		if (m_List.empty())
		{
			m_pHostMemoryBufferManager->setUsedMemoryCount(0);
			return;
		}
		const auto ErasedElement = m_List.back();
		m_List.pop_back();

		NewMemoryCount -= ErasedElement->GeometrySize;
		unsigned int ErasedUID = ErasedElement->MeshBufferUID;

		//generate delete buffer task
		m_Callback(ErasedUID);
		
		if (!m_pHostMemoryBufferManager->isTextureExistInMemory(ErasedUID))
			NewMemoryCount -= ErasedElement->TextureSize;
	}
	m_pHostMemoryBufferManager->setUsedMemoryCount(NewMemoryCount);
}

//*****************************************************************************************
//FUNCTION:
void CMemoryMeshLRUList::addNewMemoryElement(unsigned int vElementUID, std::uintmax_t vGeoSize, std::uintmax_t vTexSize)
{
	m_List.emplace_front(std::make_shared<SMemoryElement>(vElementUID, vGeoSize, vTexSize));
}

//****************************************************************************
//FUNCTION:
void CMemoryMeshLRUList::updateList(const std::set<unsigned int>& vElementUIDSet)
{
	if (vElementUIDSet.empty())
		return;

	auto itr = m_List.begin();
	auto headItr = m_List.begin();
	const unsigned int size = static_cast<unsigned int>(vElementUIDSet.size());
	unsigned int i = 0;
	while (itr != m_List.end() && i != size)
	{
		auto ElementNameListItr = vElementUIDSet.find((*itr)->MeshBufferUID);
		if (ElementNameListItr != vElementUIDSet.end())
		{
			std::swap(*headItr, *itr);
			++headItr;
			++i;
		}
		++itr;
	}
}