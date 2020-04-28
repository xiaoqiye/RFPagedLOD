#pragma once
#include "Shader.h"
#include "Camera.h"
#include "OGLWindow.h"

#include "GPUBufferInterface.h"
#include "Common.h"
#include "GPUBufferCommon.h"
#include "FrameState.h"

#include "MyUISystem.h"

#define TEST_RENDER_PERFORMANCE 0

namespace hivePagedLOD
{
	class COGLGPUOperator : public IGPUBufferInterface
	{
	public:
		TTextureID generateTextureBufferForOSGV(const std::shared_ptr<const STexture>& vTexture) override;
		std::pair<SGPUGeometryBufferHandle, unsigned int> generateGeometryBufferForOSGV(const std::shared_ptr<const SGeometry>& vGeometry) override;
		bool destroyGeometryBufferForOSGV(const SGPUGeometryBufferHandle& vHandle) override;
		bool destroyTextureBufferForOSGV(TTextureID vTextureID) override;
		void initV(unsigned int vWidth, unsigned int vHeight) override;
		void prepareGUIV() override;
		void renderV(const std::vector<std::shared_ptr<SGPUMeshBuffer>>& vMeshBufferSet) override;
		void initFirstFrameStateV(const std::shared_ptr<IFrameState>& vFrameState) override;

		std::shared_ptr<IFrameState> updateFrameStateV() override;
		bool windowShouldCloseV() override;
		void endV() override;

		void setCameraDefaultPositionV(float vX, float vY, float vZ) override { m_CameraDefualtPosition = glm::vec3(vX, vY, vZ); }
		void saveCapture2FileV(const std::string& vPath, unsigned int vWidth, unsigned int vHeight) override;
		void saveCaptureFlagV(bool vFlag) override { m_SaveCapture = vFlag; }
	private:
		void __draw(const SGPUMeshBuffer* vMesh);
		void __draw(const SMemoryMeshBuffer* vMesh);
		void __dumpViewInfo(const OpenGL_LIB::CWindow* vWindow, const OpenGL_LIB::CCamera* vCamera, SViewInfo& voViewInfo);

		std::shared_ptr<OpenGL_LIB::CShader> m_pShader;
		std::shared_ptr<CMyUISystem> m_pUISystem;
		glm::mat4 m_PreviousViewMatrix;
		glm::vec3 m_CameraDefualtPosition;
		unsigned int m_WindowWidth = 0;
		unsigned int m_WindowHeight = 0;
		uintmax_t m_FrameID = 0;
		bool m_SaveCapture = false;
	};
}