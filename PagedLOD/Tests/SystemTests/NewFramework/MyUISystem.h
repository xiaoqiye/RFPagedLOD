#pragma once
#include "UISystem.h"
#include "../../../PagedLOD/PagedLOD/FrameState.h"
#include "../../../PagedLOD/PagedLOD/RenderingTileNodeGenerator.h"

struct SFrameState : hivePagedLOD::IFrameState
{
	float CameraSpeed = 0.0f;
	int BufferLimitSize = 0;
	int LoadLimitSizePerFrame = 0;
	int LoadParentMaxLevel = 0;
	hivePagedLOD::ELoadStrategy LoadStrategy = hivePagedLOD::ELoadStrategy::UNDEFINED;
	SFrameState() = default;
	SFrameState(float vSpeed, int vBufferSize, int vLoadSize, int vLoadLevel, const hivePagedLOD::ELoadStrategy& vStrategy) :
	CameraSpeed(vSpeed), BufferLimitSize(vBufferSize), LoadLimitSizePerFrame(vLoadSize), LoadParentMaxLevel(vLoadLevel), LoadStrategy(vStrategy) {}
};

class CMyUISystem : public OpenGL_LIB::CUISystem
{
public:
	CMyUISystem() = default;
	~CMyUISystem() = default;

	void buildWidget() override;

	float getCameraSpeed() const { return m_CameraSpeed; }
	int getBufferLimitSize() const { return m_BufferLimitSize; }
	int getLoadLimitSizePerFrame() const { return m_LoadLimitSizePerFrame; }
	int getLoadParentMaxLevel() const { return m_LoadParentMaxLevel; }
	hivePagedLOD::ELoadStrategy getLoadStrategy() const { return  m_LoadStrategy; }

	void setCameraSpeed(float vSpeed)  {  m_CameraSpeed = vSpeed; }
	void setBufferLimitSize(int vSize)  {  m_BufferLimitSize = vSize; }
	void setLoadLimitSizePerFrame(int vSize)  {  m_LoadLimitSizePerFrame = vSize; }
	void setLoadParentMaxLevel(int vLevel)  {  m_LoadParentMaxLevel = vLevel; }
	void setLoadStrategy(const hivePagedLOD::ELoadStrategy& vStrategy)  {   m_LoadStrategy = vStrategy; }
	void initProxy() { m_BufferLimitSizeProxy = m_BufferLimitSize / hivePagedLOD::MEGABYTE; m_LoadLimitSizePerFrameProxy = m_LoadLimitSizePerFrame / hivePagedLOD::KILOBYTE; }
private:

	float m_CameraSpeed = 0.0f;
	int m_BufferLimitSize = 0;
	int m_BufferLimitSizeProxy = 0;
	int m_LoadLimitSizePerFrame = 0;
	int m_LoadLimitSizePerFrameProxy = 0;
	int m_LoadParentMaxLevel = 0;
	hivePagedLOD::ELoadStrategy m_LoadStrategy = hivePagedLOD::ELoadStrategy::UNDEFINED;

};