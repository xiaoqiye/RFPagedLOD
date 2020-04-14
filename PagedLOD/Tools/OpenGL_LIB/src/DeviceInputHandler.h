#pragma once
#include "OGLWindow.h"
#include "Singleton.h"

namespace OpenGL_LIB {
	class CCamera;

	class CDeviceInputHandler : public CSingleton<CDeviceInputHandler>
	{
	public:
		CDeviceInputHandler(const CDeviceInputHandler&) = delete;
		CDeviceInputHandler& operator= (const CDeviceInputHandler&) = delete;
		~CDeviceInputHandler() = default;
		void keyCallback(GLFWwindow* vWindow, int vKey, int vAction);
		void scrollCallback(double vYOffset);
		void mouseCallback(double vXPos, double vYPos);
		void doMovement();
		void recordTime() { float currentTime = static_cast<float>(glfwGetTime()); mDeltaTime = currentTime - mLastFrame; mLastFrame = currentTime; }
		float getFrameDeltaTime() const { return mDeltaTime; }
	private:
		CDeviceInputHandler() = default;

		bool mKeys[1024] = {false};
		bool mFirstMouse = true;
		float mDeltaTime = 0.0f;
		float mLastFrame = 0.0f;
		//cursor is locked , so below 2 member var no sense
		float lastX = 400, lastY = 300;

		friend class CSingleton<CDeviceInputHandler>;
	};
}