#include "ConsoleLog.h"

#include "../../PagedLOD-v5/PagedLOD/Timer.h"

#include "OGLGPUOperator.h"
#include "PagedLODSystem.h"

#include "boost/algorithm/string/predicate.hpp"

#include <fstream>

using namespace hivePagedLOD;

//****************************************************************************
//FUNCTION:
static void initFromConfigureFile(SInitValue& voOutValue)
{
	std::ifstream File("./InitValue");
	if (File.is_open())
	{
		std::string CurrentLine;
		std::getline(File, CurrentLine);
		voOutValue.WindowWidth = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.WindowHeight = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.SerializedPath = CurrentLine.substr(CurrentLine.find_last_of('=') + 1);
		std::getline(File, CurrentLine);
		voOutValue.BinPath = CurrentLine.substr(CurrentLine.find_last_of('=') + 1);
		std::getline(File, CurrentLine);
		voOutValue.BaseNPath = CurrentLine.substr(CurrentLine.find_last_of('=') + 1);
		std::getline(File, CurrentLine);
		voOutValue.LegalTileName = CurrentLine.substr(CurrentLine.find_last_of('=') + 1) == "1" ? true : false;
		std::getline(File, CurrentLine);
		voOutValue.BeginTileNumber = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.EndTileNumber = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.CameraPositionX = std::stof(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.CameraPositionY = std::stof(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.CameraPositionZ = std::stof(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.CameraSpeed = std::stof(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.BufferLimitSize = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.MultiFrameCount = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.LoadStrategy = CurrentLine.substr(CurrentLine.find_last_of('=') + 1);
		std::getline(File, CurrentLine);
		voOutValue.LoadLimitPerFrame = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.LoadLimitPerSolve = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.LoadParentMaxLevel = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.SaveCapture = CurrentLine.substr(CurrentLine.find_last_of('=') + 1) == "1" ? true : false;
		std::getline(File, CurrentLine);
		voOutValue.FinishLoadFrameID = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.OutputRenderingTileNodeGeneratorInfo = CurrentLine.substr(CurrentLine.find_last_of('=') + 1) == "1" ? true : false;
		std::getline(File, CurrentLine);
		voOutValue.TraverseMaxDeep = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
	}
	else
	{
		throw std::exception("cannot open InitValue file.");
	}
	File.close();

	voOutValue.GPUInterface = std::make_shared<COGLGPUOperator>();
}

//****************************************************************************
//FUNCTION:
static void registerTimerFunction()
{
	std::vector<std::string> FunctionNameSet;
	std::ifstream File("./FunctionName");
	if (File.is_open())
	{
		std::string CurrentLine;
		std::getline(File, CurrentLine);
		if (CurrentLine == "OFF")
		{
			CTimer::getInstance()->openTimer(false);
			return;
		}
		CTimer::getInstance()->openTimer(true);
		
		std::getline(File, CurrentLine);
		if (CurrentLine == "YES")
		{
			CTimer::getInstance()->setOutputEachFrameFlag(true);
		}
		
		while (std::getline(File, CurrentLine))
			FunctionNameSet.emplace_back(CurrentLine);

		CTimer::getInstance()->registerFunction(FunctionNameSet);
	}
	else
		throw std::exception("cannot open InitValue file.");
	File.close();
}

int main()
{
	_MEMORY_LEAK_DETECTOR;

	SInitValue InitValue;
	initFromConfigureFile(InitValue);
	
	registerTimerFunction();
	
	CPagedLODSystem::getInstance()->init(InitValue);
	CPagedLODSystem::getInstance()->getGPUThread()->doFirstFrame(InitValue.WindowWidth, InitValue.WindowHeight, InitValue.MultiFrameCount);
	CPagedLODSystem::getInstance()->getTileNodeLoader()->doFirstFrame();
	CPagedLODSystem::getInstance()->getMemoryBufferManager()->doFirstFrame();
	CPagedLODSystem::getInstance()->run();

	if (CTimer::getInstance()->on())
		CTimer::getInstance()->outputTime();

	system("PAUSE");
	return 0;
}