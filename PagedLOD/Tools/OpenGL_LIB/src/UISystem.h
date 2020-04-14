#pragma once
#include "../ThirdParty/IMGUI/imgui.h"

namespace OpenGL_LIB {

	class CUISystem
	{
	public:
		CUISystem();
		virtual ~CUISystem() = default;

		void initUISystem();
		void destoryUISystem();
		void newFrame();
		void renderUISystem();
		virtual void buildWidget() = 0;
	private:
		void __setupUIStyle();
		void __renderDrawData(ImDrawData* vDrawData);
		void __implementGLFWKeyMap();
		void __createFontsTexture();
		void __createDeviceObject();
		void __invalidateDeviceObjects();
		ImGuiContext* m_pImGuiContext;
		//gui variable
		unsigned int mFontTextureID = 0;
		unsigned int mShaderProgramID = 0;
		unsigned int mVertShaderID = 0;
		unsigned int mFragShaderID = 0;
		char         mGLSLVersion[32] = "#version 450\n";
		int mTextureLocation = 0;
		int mProjectionMatrixLocation = 0;
		int mPositionLocation = 0;
		int mUVLocation = 0;
		int mColorLocation = 0;
		unsigned int mVBO = 0;
		unsigned int mEBO = 0;
		bool         mMousePressedTag[3] = { false, false, false };
	};
}