#include "ConsoleLog.h"

#include "../../PagedLOD-v5/PagedLOD/Timer.h"

#include "OGLGPUOperator.h"
#include "PagedLODSystem.h"
#include "SceneVisitor.h"

#include "boost/algorithm/string/predicate.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iomanip>

using namespace hivePagedLOD;
static void initFromConfigureFile(SInitValue& voConfig, const std::string& vConfigFile);

int main()
{
	_MEMORY_LEAK_DETECTOR;

	SInitValue InitValue;
	std::string ConfigFile = "Config.json";

	initFromConfigureFile(InitValue, ConfigFile);

	CSceneVisitor::getInstance()->initSceneVisitor(InitValue);

	CPagedLODSystem::getInstance()->init(InitValue);
	CPagedLODSystem::getInstance()->getGPUThread()->doFirstFrame(InitValue.WindowWidth, InitValue.WindowHeight, InitValue.MultiFrameCount);
	CPagedLODSystem::getInstance()->getTileNodeLoader()->doFirstFrame();
	CPagedLODSystem::getInstance()->getMemoryBufferManager()->doFirstFrame();
	CPagedLODSystem::getInstance()->run();

	if (CTimer::getInstance()->on())
		CTimer::getInstance()->outputTime();

	return 0;
}

//****************************************************************************
//FUNCTION:
static void initFromConfigureFile(SInitValue& voConfig, const std::string& vConfigFile)
{
	_ASSERT(boost::filesystem::exists(vConfigFile));
	std::ifstream File(vConfigFile);
	nlohmann::json j;
	try {
		File >> j;
		File.close();
		voConfig.WindowHeight = j["WindowHeight"];
		voConfig.WindowWidth = j["WindowWidth"];
		voConfig.SerializedPath = j["DataPath"]["SerializedPath"];
		voConfig.BinPath = j["DataPath"]["BinPath"];
		voConfig.BaseNPath = j["DataPath"]["BaseNPath"];
		voConfig.LoadTileCount = j["Tile"]["LoadTileCount"];
		voConfig.LegalTileName = j["Tile"]["LegalTileName"];
		voConfig.BeginTileNumber = j["Tile"]["BeginTileNumber"];
		voConfig.EndTileNumber = j["Tile"]["EndTileNumber"];
		voConfig.CameraPositionX = j["Camera"]["PositionX"];
		voConfig.CameraPositionY = j["Camera"]["PositionY"];
		voConfig.CameraPositionZ = j["Camera"]["PositionZ"];
		voConfig.CameraSpeed = j["Camera"]["Speed"];
		voConfig.BufferLimitSize = j["BufferLimitSize"];
		voConfig.MultiFrameCount = j["MultiFrameCount"];
		voConfig.LoadStrategy = j["Load"]["Strategy"];
		voConfig.LoadLimitPerFrame = j["Load"]["LimitPerFrame"];
		voConfig.LoadLimitPerSolve = j["Load"]["Limit"];
		voConfig.LoadParentMaxLevel = j["Load"]["LoadParentMaxLevel"];
		voConfig.SaveCapture = j["SaveCapture"];
		voConfig.FinishLoadFrameID = j["Load"]["FinishLoadFrameID"];
		voConfig.OutputRenderingTileNodeGeneratorInfo = j["OutputInfo"];
		voConfig.TraverseMaxDeep = j["TraverseMaxDeep"];
		voConfig.GPUInterface = std::make_shared<COGLGPUOperator>();

		auto Timer = CTimer::getInstance();
		Timer->openTimer(j["RecordTime"]["Open"]);
		Timer->setOutputEachFrameFlag(j["RecordTime"]["Output"]);
		std::vector<std::string> FunctionNames;
		const auto& RecordFunctionNames = j.at("RecordTime").at("RecordFunctionNames");
		for (const auto& item : RecordFunctionNames.items()) {
			auto Value = item.value();
			FunctionNames.emplace_back(item.value());
		}
		Timer->registerFunction(FunctionNames);

		const auto& VisitorConfig = j.at("VisitorConfig");
		voConfig.VisitInit.AreaHeight = VisitorConfig["AreaHeight"];
		voConfig.VisitInit.AreaWidth = VisitorConfig["AreaWidth"];
		voConfig.VisitInit.EndFrameID = VisitorConfig["EndFrameID"];
		voConfig.VisitInit.ChangeDirectionFrameID = VisitorConfig["ChangeDirectionFrameID"];
		voConfig.VisitInit.LogPath = VisitorConfig["LogPath"];
		voConfig.VisitInit.ResetCameraPositionFrameID = VisitorConfig["ResetCameraPositionFrameID"];
		voConfig.VisitInit.SaveRecordSignal = VisitorConfig["SaveRecord"];
		voConfig.VisitInit.SaveVisitFilePath = VisitorConfig["SaveVisitFilePath"];
		voConfig.VisitInit.Seed = VisitorConfig["Seed"];
		voConfig.VisitInit.VisitFilePath = VisitorConfig["LoadVisitFilePath"];
		voConfig.VisitInit.StopPositionZ = VisitorConfig["StopPositionZ"];
		const auto& VisitStrategy = VisitorConfig["VisitStrategy"];
		for (const auto& item : VisitStrategy.items())
			voConfig.VisitInit.VisitStrategy = item.value();
		voConfig.VisitInit.WaitFrameNum = VisitorConfig["WaitFrameNum"];

		const auto& VisitorCameraConfig = VisitorConfig.at("Camera");
		voConfig.CameraInit.Far = VisitorCameraConfig["Far"];
		voConfig.CameraInit.Near = VisitorCameraConfig["Near"];
		voConfig.CameraInit.Speed = VisitorCameraConfig["Speed"];
		voConfig.CameraInit.PositionX = VisitorCameraConfig["PositionX"];
		voConfig.CameraInit.PositionY = VisitorCameraConfig["PositionY"];
		voConfig.CameraInit.PositionZ = VisitorCameraConfig["PositionZ"];
	}
	catch (std::exception& e) {
		std::cout << "fail to parse config file: " << e.what() << std::endl;
	}
}