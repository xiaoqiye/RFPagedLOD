#include "Utils.h"
#include "ConsoleLog.h"
#include <crtdbg.h>
#include <io.h>

using namespace OpenGL_LIB;

//****************************************************************************
//FUNCTION:
void Utils::getSourceFormatAndTypeFromInternalFormat(unsigned int vInternalFormat, unsigned int& voSourceFormat, unsigned int& voSourceType)
{
	if (vInternalFormat == GL_RGBA8 || vInternalFormat == GL_RGBA) { voSourceFormat = GL_RGBA; voSourceType = GL_UNSIGNED_BYTE; return; }
	if (vInternalFormat == GL_RGBA32F) { voSourceFormat = GL_RGBA; voSourceType = GL_FLOAT; return; }
	if (vInternalFormat == GL_RGB8 || vInternalFormat == GL_RGB) { voSourceFormat = GL_RGB; voSourceType = GL_UNSIGNED_BYTE; return; }
	if (vInternalFormat == GL_R8) { voSourceFormat = GL_RED; voSourceType = GL_UNSIGNED_BYTE; return; }
	if (vInternalFormat == GL_DEPTH_COMPONENT32F) { voSourceFormat = GL_DEPTH_COMPONENT; voSourceType = GL_FLOAT; return; }
	if (vInternalFormat == GL_RGB32F) { voSourceFormat = GL_RGB; voSourceType = GL_FLOAT; return; }
	if (vInternalFormat == GL_R32F) { voSourceFormat = GL_RED; voSourceType = GL_FLOAT; return; }
	_DEBUG_OUTPUT("CANNOT ANALYSIS INTERNAL FORMAT");
	_ASSERTE(false);
}

//****************************************************************************
//FUNCTION:
bool Utils::isFileExisted(const std::string& vFilePath)
{
	return _access(vFilePath.c_str(), 0) != -1;
}

//****************************************************************************
//FUNCTION:
void Utils::applyDefaultCallbackFunctionToCurrentWindow()
{
	glfwSetKeyCallback(CWindow::getInstance().fetchWindowHandle(), keyCallback);
	glfwSetCursorPosCallback(CWindow::getInstance().fetchWindowHandle(), mouseCallback);
	glfwSetScrollCallback(CWindow::getInstance().fetchWindowHandle(), scrollCallback);
}

//****************************************************************************
//FUNCTION:
void Utils::keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
	CDeviceInputHandler::getInstance().keyCallback(pWindow, key, action);
}

//****************************************************************************
//FUNCTION:
void Utils::mouseCallback(GLFWwindow * vWindow, double vXPos, double vYPos)
{
	CDeviceInputHandler::getInstance().mouseCallback(vXPos, vYPos);
}

//****************************************************************************
//FUNCTION:
void Utils::scrollCallback(GLFWwindow* vWindow, double vXOffset, double vYOffset)
{
	CDeviceInputHandler::getInstance().scrollCallback(vYOffset);
}

//****************************************************************************
//FUNCTION:
bool Utils::checkGLError()
{
	GLenum err(glGetError());
	if (err == GL_NO_ERROR) return false;
	while (err != GL_NO_ERROR) 
	{
		std::string error;

		switch (err) 
		{
		case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
		default: error = "SOME_ERROR";                                    break;
		}

		_DEBUG_OUTPUT(error);
		err = glGetError();
	}
	return true;
}

//****************************************************************************
//FUNCTION:
void Utils::initProgram(int vWindowWidth, int vWindowHeight)
{
	_MEMORY_LEAK_DETECTOR;
	glfwInit();
	CWindow::getInstance().init(vWindowWidth, vWindowHeight);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		_ASSERTE(false);

	applyDefaultCallbackFunctionToCurrentWindow();
}

//******************************************************************
//FUNCTION:
float Utils::lerp(float a, float b, float f) {
	return a + f * (b - a);
}