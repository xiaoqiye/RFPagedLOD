#include "Timer.h"
#include <iostream>

using namespace hivePagedLOD;

//****************************************************************************
//FUNCTION:
void CTimer::registerFunction(const std::vector<std::string>& vFunctionNameSet)
{
	_ASSERT(vFunctionNameSet.size() > 0);

	for (auto& Name : vFunctionNameSet)
		m_FuncCostTimeMap[Name] = 0;
}

//****************************************************************************
//FUNCTION:
double CTimer::getCostTimeByName(const std::string& vFuncName)
{
	_ASSERT(!vFuncName.empty());

	if (m_FuncCostTimeMap.find(vFuncName) != m_FuncCostTimeMap.end())
		return m_FuncCostTimeMap[vFuncName];
	return 0.0;
}

//****************************************************************************
//FUNCTION:
void CTimer::tick(const std::string& vFuncName)
{ 
	_ASSERT(!vFuncName.empty());

	if (m_FuncCostTimeMap.find(vFuncName) != m_FuncCostTimeMap.end())
		QueryPerformanceCounter(&m_BeginTime);
}

//****************************************************************************
//FUNCTION:
void CTimer::tock(const std::string& vFuncName)
{
	_ASSERT(!vFuncName.empty());

	if (m_FuncCostTimeMap.find(vFuncName) != m_FuncCostTimeMap.end())
	{
		QueryPerformanceCounter(&m_EndTime);
		//FIXME:magic number
		m_FuncCostTimeMap[vFuncName] = (m_EndTime.QuadPart - m_BeginTime.QuadPart) * 1000000.0 / m_Freq.QuadPart;
	}
}

//****************************************************************************
//FUNCTION:
void CTimer::outputTime()
{
	_ASSERT(!m_FuncCostTimeMap.empty());

	for (auto& i : m_FuncCostTimeMap)
		std::cout << i.first << "  " << i.second << std::endl;
}

