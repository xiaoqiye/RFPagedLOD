#pragma once

#include "Common.h"
#include "MemoryBufferManager.h"

#include <list>
#include <functional>
#include <set>

namespace hivePagedLOD
{
	struct SMemoryElement
	{
		unsigned int MeshBufferUID;
		std::uintmax_t GeometrySize;
		std::uintmax_t TextureSize;
		SMemoryElement(unsigned int vMeshBufferUID, std::uintmax_t vGeometrySize, std::uintmax_t vTextureSize) : MeshBufferUID(vMeshBufferUID), GeometrySize(vGeometrySize), TextureSize(vTextureSize) {};
	};

	class CMemoryBufferManager;
	class CMemoryMeshLRUList
	{
	public:
		CMemoryMeshLRUList() = delete;
		CMemoryMeshLRUList(CMemoryBufferManager* vMemoryBufferManager);
		virtual ~CMemoryMeshLRUList() = default;

		void clearLRUList() { m_List.clear(); }
		bool setLimit(std::uintmax_t vlimit) { return vlimit <= 0 ? false : (m_Limit = vlimit, true); }
		std::uintmax_t getLimit() const { return m_Limit; }
		void setOverflowCallback(std::function<void(unsigned int)> vCallback) { m_Callback = vCallback; }

		bool isValid() const { return m_Limit > 0; }

		void deleteExpiredTileNode();
		void addNewMemoryElement(unsigned int, std::uintmax_t vGeoSize, std::uintmax_t vTexSize);
		void updateList(const std::set<unsigned int>& vElementUIDSet);

		const std::list<std::shared_ptr<SMemoryElement>>& getList() const { return m_List; }
	private:
		std::uintmax_t m_Limit = DEFAULT_MEMORY_LIMIT;
		std::list<std::shared_ptr<SMemoryElement>> m_List;
		std::function<void(unsigned int)> m_Callback;
		CMemoryBufferManager* m_pHostMemoryBufferManager;
	};
}