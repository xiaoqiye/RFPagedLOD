#pragma once
#include "Common.h"
#include <vector>

class CGeometryData
{
public:
	CGeometryData(std::vector<hivePagedLOD::SOSGVertex>& voVertexSet, std::vector<unsigned int>& voIndexSet);
	~CGeometryData() = default;
	unsigned int getVertexSetSize() const { return static_cast<unsigned int>(m_VertexSet.size()); }
	unsigned int getIndexSetSize() const { return static_cast<unsigned int>(m_IndexSet.size()); }
	const void* getVertexData() const { return m_VertexSet.data(); }
	const void* getIndexData() const { return m_IndexSet.data(); }
private:
	std::vector<hivePagedLOD::SOSGVertex> m_VertexSet;
	std::vector<unsigned int> m_IndexSet;
};