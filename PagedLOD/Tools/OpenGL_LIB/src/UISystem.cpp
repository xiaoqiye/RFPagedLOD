#include "UISystem.h"
#include "ConsoleLog.h"
#include "DeviceInputHandler.h"
#include "../ThirdParty/GLAD/include/glad/glad.h"
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "../ThirdParty/GLFW/include/GLFW/glfw3native.h"
#endif

using namespace OpenGL_LIB;

CUISystem::CUISystem() : m_pImGuiContext(nullptr)
{
}

//***********************************************************
//FUNCTION:
void CUISystem::initUISystem()
{
	m_pImGuiContext = ImGui::CreateContext();
	__setupUIStyle();
	__implementGLFWKeyMap();
}

//***********************************************************
//FUNCTION:
void CUISystem::__implementGLFWKeyMap()
{
	ImGuiIO& IO = ImGui::GetIO();
	IO.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	IO.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

	IO.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	IO.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	IO.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	IO.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	IO.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	IO.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	IO.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	IO.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	IO.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	IO.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
	IO.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	IO.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	IO.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
	IO.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	IO.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	IO.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	IO.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	IO.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	IO.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	IO.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	IO.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

#ifdef _WIN32
	IO.ImeWindowHandle = glfwGetWin32Window(CWindow::getInstance().fetchWindowHandle());
#endif
}
void CUISystem::newFrame()
{
	if (!mFontTextureID)
		__createDeviceObject();
	ImGuiIO& io = ImGui::GetIO();

	int windowWidth, windowHeight;
	int frameBufferWidth, frameBufferHeight;

	glfwGetWindowSize(CWindow::getInstance().fetchWindowHandle(), &windowWidth, &windowHeight);
	glfwGetFramebufferSize(CWindow::getInstance().fetchWindowHandle(), &frameBufferWidth, &frameBufferHeight);
	io.DisplaySize = ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight));
	io.DisplayFramebufferScale = ImVec2(windowWidth > 0 ? (static_cast<float>(frameBufferWidth) / windowWidth) : 0, windowHeight > 0 ? (static_cast<float>(frameBufferHeight) / windowHeight) : 0);

	io.DeltaTime = CDeviceInputHandler::getInstance().getFrameDeltaTime();

	if (glfwGetWindowAttrib(CWindow::getInstance().fetchWindowHandle(), GLFW_FOCUSED))
	{
		if (io.WantSetMousePos)
		{
			glfwSetCursorPos(CWindow::getInstance().fetchWindowHandle(), static_cast<double>(io.MousePos.x), static_cast<double>(io.MousePos.y));
		}
		else
		{
			double mouseX, mouseY;
			glfwGetCursorPos(CWindow::getInstance().fetchWindowHandle(), &mouseX, &mouseY);
			io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
		}
	}
	else
	{
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	}

	for (int i = 0; i < 3; i++)
	{
		io.MouseDown[i] = mMousePressedTag[i] || glfwGetMouseButton(CWindow::getInstance().fetchWindowHandle(), i) != 0;
		mMousePressedTag[i] = false;
	}

	ImGui::NewFrame();
}
void CUISystem::__createDeviceObject()
{
	GLint lastTexture, lastArrayBuffer, lastVertexArray;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &lastArrayBuffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastVertexArray);

	const GLchar* vertexShader =
		"uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec2 UV;\n"
		"in vec4 Color;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"	Frag_UV = UV;\n"
		"	Frag_Color = Color;\n"
		"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar* fragmentShader =
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
		"}\n";

	const GLchar* vertex_shader_with_version[2] = { mGLSLVersion, vertexShader };
	const GLchar* fragment_shader_with_version[2] = { mGLSLVersion, fragmentShader };

	mShaderProgramID = glCreateProgram();
	mVertShaderID = glCreateShader(GL_VERTEX_SHADER);
	mFragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(mVertShaderID, 2, vertex_shader_with_version, NULL);
	glShaderSource(mFragShaderID, 2, fragment_shader_with_version, NULL);
	glCompileShader(mVertShaderID);
	glCompileShader(mFragShaderID);
	glAttachShader(mShaderProgramID, mVertShaderID);
	glAttachShader(mShaderProgramID, mFragShaderID);
	glLinkProgram(mShaderProgramID);

	mTextureLocation = glGetUniformLocation(mShaderProgramID, "Texture");
	mProjectionMatrixLocation = glGetUniformLocation(mShaderProgramID, "ProjMtx");
	mPositionLocation = glGetAttribLocation(mShaderProgramID, "Position");
	mUVLocation = glGetAttribLocation(mShaderProgramID, "UV");
	mColorLocation = glGetAttribLocation(mShaderProgramID, "Color");

	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);

	__createFontsTexture();

	glBindTexture(GL_TEXTURE_2D, lastTexture);
	glBindBuffer(GL_ARRAY_BUFFER, lastArrayBuffer);
	glBindVertexArray(lastVertexArray);
}
void CUISystem::__createFontsTexture()
{
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	GLint lastTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
	glGenTextures(1, &mFontTextureID);
	glBindTexture(GL_TEXTURE_2D, mFontTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	io.Fonts->TexID = (void *)(intptr_t)mFontTextureID;

	glBindTexture(GL_TEXTURE_2D, lastTexture);
}

//***********************************************************
//FUNCTION:
void CUISystem::renderUISystem()
{
	ImGui::Render();
	__renderDrawData(ImGui::GetDrawData());
}

//***********************************************************
//FUNCTION:
void CUISystem::destoryUISystem()
{
	__invalidateDeviceObjects();
	ImGui::DestroyContext();
}

//***********************************************************
//FUNCTION:
void CUISystem::__invalidateDeviceObjects()
{
	if (mVBO) glDeleteBuffers(1, &mVBO);
	if (mEBO) glDeleteBuffers(1, &mEBO);
	mVBO = mEBO = 0;

	if (mShaderProgramID && mVertShaderID) glDetachShader(mShaderProgramID, mVertShaderID);
	if (mVertShaderID) glDeleteShader(mVertShaderID);
	mVertShaderID = 0;

	if (mShaderProgramID && mFragShaderID) glDetachShader(mShaderProgramID, mFragShaderID);
	if (mFragShaderID) glDeleteShader(mFragShaderID);
	mFragShaderID = 0;

	if (mShaderProgramID) glDeleteProgram(mShaderProgramID);
	mShaderProgramID = 0;

	if (mFontTextureID)
	{
		glDeleteTextures(1, &mFontTextureID);
		ImGui::GetIO().Fonts->TexID = 0;
		mFontTextureID = 0;
	}
}

//***********************************************************
//FUNCTION:
void CUISystem::__setupUIStyle()
{
	ImGui::StyleColorsDark();
	//TODO: setup Colors¡¢Fonts...
}

//***********************************************************
//FUNCTION:
void CUISystem::__renderDrawData(ImDrawData* vDrawData)
{
	ImGuiIO& io = ImGui::GetIO();
	int frameBufferWidth = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int frameBufferHeight = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (frameBufferWidth == 0 || frameBufferHeight == 0)
		return;
	vDrawData->ScaleClipRects(io.DisplayFramebufferScale);

	// Backup GL state
	GLenum lastActiveTexture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&lastActiveTexture);
	glActiveTexture(GL_TEXTURE0);
	GLint lastProgram; glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);
	GLint lastTexture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
	GLint lastSampler; glGetIntegerv(GL_SAMPLER_BINDING, &lastSampler);
	GLint lastArrayBuffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &lastArrayBuffer);
	GLint lastElementArrayBuffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &lastElementArrayBuffer);
	GLint lastVertexArray; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastVertexArray);
	GLint lastPolygonMode[2]; glGetIntegerv(GL_POLYGON_MODE, lastPolygonMode);
	GLint lastViewport[4]; glGetIntegerv(GL_VIEWPORT, lastViewport);
	GLint lastScissorBox[4]; glGetIntegerv(GL_SCISSOR_BOX, lastScissorBox);
	GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
	GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
	GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
	GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
	GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
	GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Setup viewport, orthographic projection matrix
	glViewport(0, 0, (GLsizei)frameBufferWidth, (GLsizei)frameBufferHeight);
	const float ortho_projection[4][4] =
	{
		{ 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
		{ 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
		{ 0.0f,                  0.0f,                  -1.0f, 0.0f },
		{-1.0f,                  1.0f,                   0.0f, 1.0f },
	};
	glUseProgram(mShaderProgramID);
	glUniform1i(mTextureLocation, 0);
	glUniformMatrix4fv(mProjectionMatrixLocation, 1, GL_FALSE, &ortho_projection[0][0]);
	if (glBindSampler) glBindSampler(0, 0); // We use combined texture/sampler state. Applications using GL 3.3 may set that otherwise.

	// Recreate the VAO every time 
	// (This is to easily allow multiple GL contexts. VAO are not shared among GL contexts, and we don't track creation/deletion of windows so we don't have an obvious key to use to cache them.)
	GLuint vao_handle = 0;
	glGenVertexArrays(1, &vao_handle);
	glBindVertexArray(vao_handle);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glEnableVertexAttribArray(mPositionLocation);
	glEnableVertexAttribArray(mUVLocation);
	glEnableVertexAttribArray(mColorLocation);
	glVertexAttribPointer(mPositionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(mUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(mColorLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, col));

	for (int n = 0; n < vDrawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = vDrawData->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(frameBufferHeight - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}
	glDeleteVertexArrays(1, &vao_handle);

	// Restore modified GL state
	glUseProgram(lastProgram);
	glBindTexture(GL_TEXTURE_2D, lastTexture);
	if (glBindSampler) glBindSampler(0, lastSampler);
	glActiveTexture(lastActiveTexture);
	glBindVertexArray(lastVertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, lastArrayBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lastElementArrayBuffer);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)lastPolygonMode[0]);
	glViewport(lastViewport[0], lastViewport[1], (GLsizei)lastViewport[2], (GLsizei)lastViewport[3]);
	glScissor(lastScissorBox[0], lastScissorBox[1], (GLsizei)lastScissorBox[2], (GLsizei)lastScissorBox[3]);
}