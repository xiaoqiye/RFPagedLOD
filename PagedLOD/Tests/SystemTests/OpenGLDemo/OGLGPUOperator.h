#pragma once
#include "Shader.h"
#include "Camera.h"
#include "OGLWindow.h"

#include "../../../PagedLOD/PagedLOD/GPUBufferInterface.h"
#include "../../../PagedLOD/PagedLOD/Common.h"
#include "../../../PagedLOD/PagedLOD/GPUBufferCommon.h"
#include "../../../PagedLOD/PagedLOD/FrameState.h"

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
		void renderV(const std::vector<std::shared_ptr<SGPUMeshBuffer>>& vMeshBufferSet) override;
		void renderV(const std::vector<std::shared_ptr<SMemoryMeshBuffer>>& vMeshBufferSet) override;
		std::pair<float, float> getWindowPositionV() override;

		std::shared_ptr<IFrameState> updateFrameStateV() override;
		bool windowShouldCloseV() override;
		void endV() override;

		SViewInfo handleCameraV(float vX, float vY) override;

	private:
		void __draw(const SGPUMeshBuffer* vMesh);
		void __draw(const SMemoryMeshBuffer* vMesh);
		void __dumpViewInfo(const OpenGL_LIB::CWindow* vWindow, const OpenGL_LIB::CCamera* vCamera, SViewInfo& voViewInfo);

		std::shared_ptr<OpenGL_LIB::CShader> m_pShader;
		std::shared_ptr<CMyUISystem> m_pUISystem;
		glm::mat4 m_PreviousViewMatrix;
		POINT m_PreviousMousePosition;
		bool m_FirstMouse = true;
		float m_MouseMovementX;
		float m_MouseMovementY;
		float m_W = 0.0f;
		float m_S = 0.0f;
		float m_A = 0.0f;
		float m_D = 0.0f;
		float m_Q = 0.0f;
		float m_E = 0.0f;

	};
}