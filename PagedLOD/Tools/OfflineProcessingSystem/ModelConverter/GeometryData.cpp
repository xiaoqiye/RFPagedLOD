#include "GeometryData.h"

CGeometryData::CGeometryData(std::vector<hivePagedLOD::SOSGVertex>& voVertexSet, std::vector<unsigned>& voIndexSet)
{
	m_VertexSet = std::move(voVertexSet);
	m_IndexSet = std::move(voIndexSet);
}