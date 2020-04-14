#include "Camera.h"
#include "DeviceInputHandler.h"

using namespace OpenGL_LIB;

//****************************************************************************
//FUNCTION:
void CDeviceInputHandler::doMovement()
{
	if (mKeys[GLFW_KEY_W])
		CCamera::getInstance().processKeyBoard(Constants::ECameraMovement::FORWARD, mDeltaTime);
	if (mKeys[GLFW_KEY_S])
		CCamera::getInstance().processKeyBoard(Constants::ECameraMovement::BACKWARD, mDeltaTime);
	if (mKeys[GLFW_KEY_A])
		CCamera::getInstance().processKeyBoard(Constants::ECameraMovement::LEFT, mDeltaTime);
	if (mKeys[GLFW_KEY_D])
		CCamera::getInstance().processKeyBoard(Constants::ECameraMovement::RIGHT, mDeltaTime);
	if (mKeys[GLFW_KEY_Q])
		CCamera::getInstance().processKeyBoard(Constants::ECameraMovement::UP, mDeltaTime);
	if (mKeys[GLFW_KEY_E])
		CCamera::getInstance().processKeyBoard(Constants::ECameraMovement::DOWN, mDeltaTime);
}

//****************************************************************************
//FUNCTION:
void CDeviceInputHandler::keyCallback(GLFWwindow* vWindow, int vKey, int vAction)
{
	if (vKey == GLFW_KEY_ESCAPE && vAction == GLFW_PRESS)
		glfwSetWindowShouldClose(vWindow, GL_TRUE);

	if (vAction == GLFW_PRESS)
		mKeys[vKey] = true;
	else if (vAction == GLFW_RELEASE)
		mKeys[vKey] = false;
}

//****************************************************************************
//FUNCTION:
void CDeviceInputHandler::mouseCallback(double vXPos, double vYPos)
{
	if (mKeys[GLFW_KEY_LEFT_CONTROL]) {
		glfwSetInputMode(CWindow::getInstance().fetchWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		lastX = static_cast<float>(vXPos);
		lastY = static_cast<float>(vYPos);
		return;
	}
	else {
		glfwSetInputMode(CWindow::getInstance().fetchWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	if (mFirstMouse)
	{
		lastX = static_cast<float>(vXPos);
		lastY = static_cast<float>(vYPos);
		mFirstMouse = false;
	}

	float xoffset = static_cast<float>(vXPos - lastX);
	float yoffset = static_cast<float>(lastY - vYPos);

	lastX = static_cast<float>(vXPos);
	lastY = static_cast<float>(vYPos);

	CCamera::getInstance().processMouseMovement(xoffset, yoffset);
}

//****************************************************************************
//FUNCTION:
void CDeviceInputHandler::scrollCallback(double vYOffset)
{
	CCamera::getInstance().processMouseScroll(static_cast<float>(vYOffset));
}