#pragma once

#include <GLM/vec3.hpp>
#include <GLM/vec4.hpp>
#include <GLM/mat4x4.hpp>
#include <common/Singleton.h>

#include <random>
#include <string>
#include <vector>
#include "PagedLODSystem.h"	
using namespace hivePagedLOD;


class CSceneVisitor : public hiveOO::CSingleton<CSceneVisitor>
{
public:
	void initSceneVisitor(const SInitValue& vInitValue);
	void visit(unsigned int vFrameID);
	
	glm::vec4 getCameraInitPostion() const { return glm::vec4(m_CameraDefaultPosition, 1.0f); }
	float getStopPositionZ() const { return m_VisitStopPositionZ; }
	std::string getRecordFilePath() const { return m_RecordFilePath; }
private:
	void __visitRandomly(unsigned int vFrameID);
	void __visitStraightly();
	void __visitByRecord(unsigned int vFrameID);

	void __changeDirection(unsigned int vFrameID);
	void __moveFront(bool vBoundary);
	void __resetPosition();

	EVisitStrategy m_Strategy;
	
	float m_RandomFloat;
	std::default_random_engine m_RandomEngine;
	std::uniform_real_distribution<float> m_Distribution;

	unsigned int m_PauseFrameCount;
	
	int m_WindowWidth;
	int m_WindowHeight;

	float m_CameraFarPlane;
	float m_CameraNearPlane;
	glm::vec3 m_CameraDefaultPosition;
	float m_VisitStopPositionZ;
	
	float m_VisitSpeed;
	unsigned int m_VisitEndFrameID;
	unsigned int m_ChangeDirectionFrameID;
	unsigned int m_ResetCameraFrameID;
	unsigned int m_Seed;
	unsigned int m_WaitFrameCount = 0;
	
	unsigned int m_AreaWidth;
	unsigned int m_AreaHeight;
	std::string m_LogPath;
	std::string m_RecordFilePath;

	//add
	unsigned int m_WaitFrameNum;
	
	std::vector<std::vector<glm::vec3>> m_RecordSet;

friend class hiveOO::CSingleton<CSceneVisitor>;
};