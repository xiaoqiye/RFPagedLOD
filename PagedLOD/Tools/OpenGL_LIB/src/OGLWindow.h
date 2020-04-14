#pragma once
#include "Singleton.h"
#include "GLHeader.h"

namespace OpenGL_LIB {

	class CWindow : public CSingleton<CWindow>
	{
	public:
		CWindow(const CWindow&) = delete;
		CWindow& operator= (const CWindow&) = delete;
		~CWindow() = default;
		GLFWwindow* fetchWindowHandle() { return mWindow; }
		unsigned int getWidth() const { return mWidth; }
		unsigned int getHeight() const { return mHeight; }
		void init(unsigned int vWidth, unsigned int vHeight);

	private:
		CWindow() = default;
		void __createWindow();
		unsigned int mWidth = 1080;
		unsigned int mHeight = 720;
		GLFWwindow* mWindow = nullptr;

		friend class CSingleton<CWindow>;
	};
}