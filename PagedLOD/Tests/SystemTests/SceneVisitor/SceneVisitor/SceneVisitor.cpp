#include "SceneVisitor.h"

#include "Common.h"
#include "Camera.h"

#include <fstream>
#include <iostream>

const float BOUNDARYZ = 180.0f;
const float BOUNDARYX = 80.0f;
const float RESET_POSITION_FACTOR = 90.0f;
const float CHANGE_DIRECTION_FACTOR = 1.0f;
const unsigned int WAIT_FRAME_COUNT = 200;
const unsigned int VISIT_FRAME_COUNT = 500;

//****************************************************************************
//FUNCTION:
void CSceneVisitor::initSceneVisitor(unsigned int vWindowWidth, unsigned int vWindowHeight, EVisitStrategy vStrategy)
{
	m_WindowWidth = vWindowWidth;
	m_WindowHeight = vWindowHeight;
	m_strategy = vStrategy;

	std::string LoadFilePath = "";
	float CameraSpeed = 0.0f;
	std::ifstream File("./VisitorConfig");
	if (File.is_open())
	{
		std::string CurrentLine;
		std::getline(File, CurrentLine);
		CameraSpeed = std::stof(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		OpenGL_LIB::CCamera::getInstance().setSpeed(CameraSpeed);
		std::getline(File, CurrentLine);
		m_CameraFarPlane = std::stof(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		m_CameraNearPlane = std::stof(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		auto x = std::stof(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		auto y = std::stof(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		auto z = std::stof(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		m_CameraDefaultPosition = glm::vec3(x, y, z);
		std::getline(File, CurrentLine);
		m_VisitStopPositionZ = std::stof(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		m_VisitSpeed = std::stof(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		m_VisitEndFrameID = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		m_ChangeDirectionFrameID = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		m_ResetCameraFrameID = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		m_Seed = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		m_AreaLength = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		m_AreaWidth = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		m_LogPath = CurrentLine.substr(CurrentLine.find_last_of('=') + 1);
		std::getline(File, CurrentLine);
		LoadFilePath = CurrentLine.substr(CurrentLine.find_last_of('=') + 1);
		std::getline(File, CurrentLine);
		m_RecordFilePath = CurrentLine.substr(CurrentLine.find_last_of('=') + 1);
		std::getline(File, CurrentLine);

		bool SaveRecordFlag = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		if (SaveRecordFlag)
			OpenGL_LIB::CCamera::getInstance().setSaveRecordFlag(true);
	}
	else
	{
		throw std::exception("cannot open VisitorConfig file.");
	}
	File.close();

	m_Distribution = std::uniform_real_distribution<float>(-1.0f, 1.0f);
	m_RandomEngine = std::default_random_engine(m_Seed);
	m_RandomFloat = m_Distribution(m_RandomEngine);

	std::vector<glm::vec3> TempPosVec;
	if (m_strategy == EVisitStrategy::BY_RECORD)
	{
		std::ifstream LoadFileStream(LoadFilePath);
		if (LoadFileStream.is_open())
		{
			std::string CurrentLine;
			while (std::getline(LoadFileStream, CurrentLine))
			{
				auto PositionX = std::stof(CurrentLine);
				std::getline(LoadFileStream, CurrentLine);
				auto PositionY = std::stof(CurrentLine);
				std::getline(LoadFileStream, CurrentLine);
				auto PositionZ = std::stof(CurrentLine);
				std::getline(LoadFileStream, CurrentLine);
				auto FrontX = std::stof(CurrentLine);
				std::getline(LoadFileStream, CurrentLine);
				auto FrontY = std::stof(CurrentLine);
				std::getline(LoadFileStream, CurrentLine);
				auto FrontZ = std::stof(CurrentLine);
				std::getline(LoadFileStream, CurrentLine);
				auto UpX = std::stof(CurrentLine);
				std::getline(LoadFileStream, CurrentLine);
				auto UpY = std::stof(CurrentLine);
				std::getline(LoadFileStream, CurrentLine);
				auto UpZ = std::stof(CurrentLine);

				std::vector<glm::vec3> t;
				t.emplace_back(glm::vec3(PositionX, PositionY, PositionZ));
				t.emplace_back(glm::vec3(FrontX, FrontY, FrontZ));
				t.emplace_back(glm::vec3(UpX, UpY, UpZ));
				m_RecordSet.emplace_back(t);
				TempPosVec.emplace_back(glm::vec3(PositionX, PositionY, PositionZ));
			}
		}
	}
	_ASSERT(m_RecordSet.size() == TempPosVec.size());
	std::cout << "Record ViewPort num:" << m_RecordSet.size() << std::endl;

	/*int MaxX = 0;
	int MaxZ = 0;
	int MinX = 1000;
	int MinZ = 1000;
	for (int i = 0; i < TempPosVec.size(); ++i)
	{
		if (TempPosVec[i].x > MaxX)
			MaxX = TempPosVec[i].x;
		if (TempPosVec[i].x < MinX)
			MinX = TempPosVec[i].x;
		if (TempPosVec[i].z > MaxZ)
			MaxZ = TempPosVec[i].z;
		if (TempPosVec[i].z < MinZ)
			MinZ = TempPosVec[i].z;
	}
	std::cout << MaxX << std::endl;*/
}

//****************************************************************************
//FUNCTION:
void CSceneVisitor::visit(unsigned int vFrameID)
{
	switch (m_strategy)
	{
	case EVisitStrategy::GO_STRAIGHT:
		__visitStraightly();
		break;
	case EVisitStrategy::RANDOM:
		__visitRandomly(vFrameID);
		break;
	case EVisitStrategy::BY_RECORD:
		__visitByRecord(vFrameID);
		break;
	case EVisitStrategy::NO_STRATEGY:
		break;
	default:
		_ASSERTE(false);
	}
}

//****************************************************************************
//FUNCTION:
void CSceneVisitor::__visitRandomly(unsigned int vFrameID)
{
	/*if (m_WaitFrameCount < WAIT_FRAME_COUNT)
	{
		++m_WaitFrameCount;
		return;
	}*/
	__changeDirection(vFrameID);
	__moveFront(true);
	/*++m_WaitFrameCount;
	if (m_WaitFrameCount == VISIT_FRAME_COUNT)
		m_WaitFrameCount = 0;*/
}

//****************************************************************************
//FUNCTION:
void CSceneVisitor::__visitStraightly()
{
	if (OpenGL_LIB::CCamera::getInstance().getPosition().z > m_VisitStopPositionZ)
		__moveFront(false);
}

//****************************************************************************
//FUNCTION:
void CSceneVisitor::__visitByRecord(unsigned int vFrameID)
{
	static int WaitFrameNum = 0;
	++WaitFrameNum;
	//FIXME
	if (WaitFrameNum >= WAIT_FRAME_NUM)
	{
		if (vFrameID / WAIT_FRAME_NUM < m_RecordSet.size())
		{
			OpenGL_LIB::CCamera::getInstance().setPosition(m_RecordSet[vFrameID][0]);
			OpenGL_LIB::CCamera::getInstance().setFront(m_RecordSet[vFrameID][1]);
			OpenGL_LIB::CCamera::getInstance().setUp(m_RecordSet[vFrameID][2]);
			WaitFrameNum = 0;
		}
		else
		{
			OpenGL_LIB::CCamera::getInstance().setPosition(m_RecordSet[0][0]);
			OpenGL_LIB::CCamera::getInstance().setFront(m_RecordSet[0][1]);
			OpenGL_LIB::CCamera::getInstance().setUp(m_RecordSet[0][2]);
		}

	}
}

//****************************************************************************
//FUNCTION:
void CSceneVisitor::__changeDirection(unsigned int vFrameID)
{
	if (vFrameID % m_ChangeDirectionFrameID == 0)
		m_RandomFloat = m_Distribution(m_RandomEngine);
	OpenGL_LIB::CCamera::getInstance().processMouseMovement(m_RandomFloat * CHANGE_DIRECTION_FACTOR, 0.0f, true);
	//OpenGL_LIB::CCamera::getInstance().processMouseMovement(0.0f, 0.0f, false);
}

//****************************************************************************
//FUNCTION:
void CSceneVisitor::__moveFront(bool vBoundary)
{
	glm::vec3 Pos = OpenGL_LIB::CCamera::getInstance().getPosition();
	Pos = Pos + OpenGL_LIB::CCamera::getInstance().getFront() * m_VisitSpeed;
	////保证y不变
	Pos.y = m_CameraDefaultPosition.z;
	OpenGL_LIB::CCamera::getInstance().setPosition(Pos);
	if (vBoundary && (Pos.x > BOUNDARYX || Pos.x < -BOUNDARYX || Pos.z > BOUNDARYZ || Pos.z < -BOUNDARYZ))
		__resetPosition();
}

//****************************************************************************
//FUNCTION:
void CSceneVisitor::__resetPosition()
{
	float X = m_Distribution(m_RandomEngine) * m_AreaWidth;
	float Z = m_Distribution(m_RandomEngine) * m_AreaLength;
	std::vector<float> YamVec = { -180.0f,90.0f,0.0f ,-90.0f};
	static int i = 0;
	++i;
	//int Gap = 50;
	//static int i = 1;
	OpenGL_LIB::CCamera::getInstance().setPosition(hivePagedLOD::MODEL_MATRIX * glm::vec4(X, Z, m_CameraDefaultPosition.z, 1.0f));
	OpenGL_LIB::CCamera::getInstance().setYaw(YamVec[i]);
	if (i == 3)
		i = 0;
	//OpenGL_LIB::CCamera::getInstance().setPosition(hivePagedLOD::MODEL_MATRIX * glm::vec4(m_CameraDefaultPosition.x, m_CameraDefaultPosition.y + i * Gap, m_CameraDefaultPosition.z, 1.0f));
	//++i;
}
