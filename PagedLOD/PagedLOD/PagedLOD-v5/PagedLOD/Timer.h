#pragma once
#include <common/Singleton.h>
#include <Windows.h>
#include <vector>
#include <map>
#include <string>

namespace hivePagedLOD
{
	class CTimer : public hiveOO::CSingleton<CTimer>
	{
	public:
		~CTimer() = default;

		void registerFunction(const std::vector<std::string>& vFunctionNameSet);
		double getCostTimeByName(const std::string& vFuncName);
		void tick(const std::string& vFuncName);
		void tock(const std::string& vFuncName);
		void outputTime();
		bool isRegistered(const std::string& vFuncName) { _ASSERT(!vFuncName.empty()); return m_FuncCostTimeMap.find(vFuncName) != m_FuncCostTimeMap.end(); }
		bool needOutput() { return m_OutputEachFrameFlag; }
		bool on() { return m_TimerOn; }
		void openTimer(bool vOn) { m_TimerOn = vOn; }
		void setOutputEachFrameFlag(bool vFlag) { m_OutputEachFrameFlag = vFlag; }

	private:
		CTimer() { QueryPerformanceFrequency(&m_Freq); }

		LARGE_INTEGER m_BeginTime;
		LARGE_INTEGER m_EndTime;
		LARGE_INTEGER m_Freq;
		bool m_TimerOn = false;
		bool m_OutputEachFrameFlag = false;
		std::map<std::string, double> m_FuncCostTimeMap;

		friend class hiveOO::CSingleton<CTimer>;
	};
}