#pragma once
#include "UISystem.h"
#include "../../../PagedLOD/PagedLOD/FrameState.h"
#include "../../../PagedLOD/PagedLOD/RenderingTileNodeGenerator.h"

struct SFrameState : hivePagedLOD::IFrameState
{
	bool ClearBuffer = false;
	int BufferLimitSizeMB = 1024;
	float CameraSpeed = 50.0f;

	hivePagedLOD::ELoadStrategy LoadStrategy = hivePagedLOD::ELoadStrategy::DO_NOTHING;
};

class CMyUISystem : public OpenGL_LIB::CUISystem
{
public:
	CMyUISystem() = default;
	~CMyUISystem() = default;

	void buildWidget() override;

	bool getClearBufferStatus() const { return m_ClearBuffer; }
	int getLimitSizeMB() const { return m_LimitSize; }
	float getCameraSpeed() const { return m_CameraSpeed; }
	hivePagedLOD::ELoadStrategy getLoadStrategyStatus() const { return m_LoadStrategy; }
private:
	bool m_ClearBuffer = false;
	int m_LimitSize = 1024;
	float m_CameraSpeed = 50.0f;

	hivePagedLOD::ELoadStrategy m_LoadStrategy = hivePagedLOD::ELoadStrategy::DO_NOTHING;
};