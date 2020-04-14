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

		//user should not call tick() tock() in main,
		//they should be called in function like CTileLoader::work(),
		//user enable timer by calling CTimer::registerFunction() in main

		void registerFunction(const std::vector<std::string>& vFunctionNameSet);
		double getCostTimeByName(const std::string& vFuncName);
		void openTimer(bool vOn) { m_TimerOn = vOn; }
		void outputTime();
		bool needOutput() { return m_OutputEachFrameFlag; }
		void setOutputEachFrameFlag(bool vFlag) { m_OutputEachFrameFlag = vFlag; }
		bool on() { return m_TimerOn; }
		void tick(const std::string& vFuncName);
		void tock(const std::string& vFuncName);
		bool isRegistered(const std::string& vFuncName) { return m_FuncCostTimeMap.find(vFuncName) != m_FuncCostTimeMap.end(); }
	private:
		LARGE_INTEGER m_BeginTime;
		LARGE_INTEGER m_EndTime;
		LARGE_INTEGER m_Freq;
		std::map<std::string, double> m_FuncCostTimeMap;
		bool m_TimerOn = false;
		bool m_OutputEachFrameFlag = false;
		CTimer() { QueryPerformanceFrequency(&m_Freq); }
		friend class hiveOO::CSingleton<CTimer>;
	};
}