#pragma once
#include "PagedLOD/Tile.h"
#include "PagedLOD/Common.h"
#include <GLM/glm.hpp>

#include <vector>

namespace MockData
{
	hivePagedLOD::CMeshLRUList createMeshLRUList()
	{
		hivePagedLOD::CMeshLRUList LRUList;
		LRUList.init();

		return LRUList;
	}
}