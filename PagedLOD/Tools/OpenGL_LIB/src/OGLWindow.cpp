#include "OGLWindow.h"
#include <crtdbg.h>


using namespace OpenGL_LIB;

//****************************************************************************
//FUNCTION:
void CWindow::init(unsigned int vWidth, unsigned int vHeight)
{
	_ASSERTE(vHeight && vWidth);
	mWidth = vWidth;
	mHeight = vHeight;
	__createWindow();
}

//****************************************************************************
//FUNCTION:
void CWindow::__createWindow()
{
	_ASSERTE(!mWindow);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	mWindow = glfwCreateWindow(mWidth, mHeight, "PagedLOD", nullptr, nullptr);
	_ASSERTE(mWindow);
	glfwMakeContextCurrent(mWindow);
}