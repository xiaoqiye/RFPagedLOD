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
void CSceneVisitor::initSceneVisitor(const SInitValue& vInitValue)
{
	m_WindowHeight = vInitValue.WindowHeight;
	m_WindowWidth = vInitValue.WindowWidth;

	const SCameraInitValue& CameraInit = vInitValue.CameraInit;
	float CameraSpeed = CameraInit.Speed;
	m_CameraFarPlane = CameraInit.Far;
	m_CameraNearPlane = CameraInit.Near;
	m_CameraDefaultPosition = glm::vec3(CameraInit.PositionX, CameraInit.PositionY, CameraInit.PositionZ);


	const SVisitInitValue& VisitInit = vInitValue.VisitInit;
	m_Strategy = hivePagedLOD::EVisitStrategy(VisitInit.VisitStrategy);
	m_VisitStopPositionZ = VisitInit.StopPositionZ;
	m_VisitSpeed = VisitInit.Speed;
	m_VisitEndFrameID = VisitInit.EndFrameID;
	m_ChangeDirectionFrameID = VisitInit.ChangeDirectionFrameID;
	m_ResetCameraFrameID = VisitInit.ResetCameraPositionFrameID;
	m_Seed = VisitInit.Seed;
	m_AreaHeight = VisitInit.AreaHeight;
	m_AreaWidth = VisitInit.AreaWidth;
	m_LogPath = VisitInit.LogPath;
	m_RecordFilePath = VisitInit.SaveVisitFilePath;
	m_WaitFrameNum = VisitInit.WaitFrameNum;

	OpenGL_LIB::CCamera::getInstance().setSaveRecordFlag(VisitInit.SaveRecordSignal);
	std::string LoadVisitFilePath = VisitInit.VisitFilePath;
	_ASSERTE(boost::filesystem::exists(LoadVisitFilePath));
	if (m_Strategy == hivePagedLOD::EVisitStrategy::BY_RECORD)
	{
		std::ifstream LoadVisitStream(LoadVisitFilePath);
		if (LoadVisitStream.is_open())
		{
			std::string CurrentLine;
			while (std::getline(LoadVisitStream, CurrentLine))
			{
				auto PositionX = std::stof(CurrentLine);
				std::getline(LoadVisitStream, CurrentLine);
				auto PositionY = std::stof(CurrentLine);
				std::getline(LoadVisitStream, CurrentLine);
				auto PositionZ = std::stof(CurrentLine);
				std::getline(LoadVisitStream, CurrentLine);
				auto FrontX = std::stof(CurrentLine);
				std::getline(LoadVisitStream, CurrentLine);
				auto FrontY = std::stof(CurrentLine);
				std::getline(LoadVisitStream, CurrentLine);
				auto FrontZ = std::stof(CurrentLine);
				std::getline(LoadVisitStream, CurrentLine);
				auto UpX = std::stof(CurrentLine);
				std::getline(LoadVisitStream, CurrentLine);
				auto UpY = std::stof(CurrentLine);
				std::getline(LoadVisitStream, CurrentLine);
				auto UpZ = std::stof(CurrentLine);

				std::vector<glm::vec3> t;
				t.emplace_back(glm::vec3(PositionX, PositionY, PositionZ));
				t.emplace_back(glm::vec3(FrontX, FrontY, FrontZ));
				t.emplace_back(glm::vec3(UpX, UpY, UpZ));
				m_RecordSet.emplace_back(t);
			}
		}
	}
}
//****************************************************************************
//FUNCTION:
void CSceneVisitor::visit(unsigned int vFrameID)
{
	switch (m_Strategy)
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
	__changeDirection(vFrameID);
	__moveFront(true);
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
	std::cout << "Record ViewPort num:" << m_RecordSet.size() << std::endl;
	static unsigned int WaitFrameNum = 0;
	++WaitFrameNum;
	//FIXME
	if (WaitFrameNum >= m_WaitFrameNum)
	{
		if (vFrameID / m_WaitFrameNum < m_RecordSet.size())
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
	float Z = m_Distribution(m_RandomEngine) * m_AreaHeight;
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
