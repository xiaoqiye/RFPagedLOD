#pragma once
#include "DeviceInputHandler.h"
#include <string>

namespace OpenGL_LIB {

	class CWindow;

	namespace Utils
	{
		void getSourceFormatAndTypeFromInternalFormat(unsigned int vInternalFormat, unsigned int& voSourceFormat, unsigned int& voSourceType);
		bool isFileExisted(const std::string& vFilePath);

		void initProgram(int vWindowWidth, int vWindowHeight);
		bool checkGLError();

		void applyDefaultCallbackFunctionToCurrentWindow();
		static void keyCallback(GLFWwindow* vWindow, int vKey, int vScancode, int vAction, int vMods);
		static void mouseCallback(GLFWwindow* vWindow, double vXPos, double vYPos);
		static void scrollCallback(GLFWwindow* vWindow, double vXOffset, double vYOffset);
		float lerp(float a, float b, float f);
	}
}