#include "Timer.h"
#include <iostream>

using namespace hivePagedLOD;

//****************************************************************************
//FUNCTION:
void CTimer::registerFunction(const std::vector<std::string>& vFunctionNameSet)
{
	for (auto& Name : vFunctionNameSet)
	{
		m_FuncCostTimeMap[Name] = 0;
	}
}

//****************************************************************************
//FUNCTION:
void CTimer::tick(const std::string& vFuncName)
{ 
	if (m_FuncCostTimeMap.find(vFuncName) != m_FuncCostTimeMap.end())
		QueryPerformanceCounter(&m_BeginTime);
}

//****************************************************************************
//FUNCTION:
void CTimer::tock(const std::string& vFuncName)
{
	if (m_FuncCostTimeMap.find(vFuncName) != m_FuncCostTimeMap.end())
	{
		QueryPerformanceCounter(&m_EndTime);
		m_FuncCostTimeMap[vFuncName] = (m_EndTime.QuadPart - m_BeginTime.QuadPart) * 1000000.0 / m_Freq.QuadPart;
	}
}

//****************************************************************************
//FUNCTION:
double CTimer::getCostTimeByName(const std::string& vFuncName)
{ 
	if (m_FuncCostTimeMap.find(vFuncName) != m_FuncCostTimeMap.end())
	{
		return m_FuncCostTimeMap[vFuncName];
	}
	return 0.0;
}

//****************************************************************************
//FUNCTION:
void CTimer::outputTime()
{
	for (auto& i : m_FuncCostTimeMap)
		std::cout << i.first << "  " << i.second << std::endl;
}

