#include "GLHeader.h"
#include "OGLWindow.h"
#include "../../OpenGL_LIB/src/Utils.h"
#include "Camera.h"

#include "Common.h"
#include "GPUBufferCommon.h"

#include "OGLGPUOperator.h"
#include "PagedLODSystem.h"
#include "MyUISystem.h"
#include "SceneVisitor.h"

#include <windows.h>
#include <FreeImage.h>
#include <iostream>

using namespace hivePagedLOD;

//****************************************************************************
//FUNCTION:
bool COGLGPUOperator::destroyGeometryBufferForOSGV(const SGPUGeometryBufferHandle& vHandle)
{
	unsigned int VAO = static_cast<unsigned int>(vHandle.VAO);
	unsigned int VBO = static_cast<unsigned int>(vHandle.VBO);
	unsigned int IBO = static_cast<unsigned int>(vHandle.IBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	return true;
}

//****************************************************************************
//FUNCTION:
bool COGLGPUOperator::destroyTextureBufferForOSGV(TTextureID vTextureID)
{
	unsigned int TextureID = static_cast<unsigned int>(vTextureID);
	glDeleteTextures(1, &TextureID);
	return true;
}

//****************************************************************************
//FUNCTION:
TTextureID COGLGPUOperator::generateTextureBufferForOSGV(const std::shared_ptr<const STexture>& vTexture)
{
	unsigned int TextureID;
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	//notice:default config for osg texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//notice:temp osg texture internal format
	unsigned int InternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
	glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, InternalFormat, vTexture->TexInfo.Width, vTexture->TexInfo.Height, 0, vTexture->TexInfo.Size, vTexture->pTextureData.get());
	glBindTexture(GL_TEXTURE_2D, 0);

	return static_cast<TTextureID>(TextureID);
}

//****************************************************************************
//FUNCTION:
std::pair<SGPUGeometryBufferHandle, unsigned int> COGLGPUOperator::generateGeometryBufferForOSGV(const std::shared_ptr<const SGeometry>& vGeometry)
{
	unsigned int VAO, VBO, IBO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vGeometry->VertexCount * sizeof(OSG_VERTEX_SIZE), vGeometry->pVertexData.get(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vGeometry->IndexCount * sizeof(unsigned int), vGeometry->pIndexData.get(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return std::make_pair<>(SGPUGeometryBufferHandle{ static_cast<TVertexArrayHandle>(VAO),static_cast<TBufferHandle>(VBO),static_cast<TBufferHandle>(IBO) }, vGeometry->IndexCount);
}

//****************************************************************************
//FUNCTION:
void COGLGPUOperator::initV(unsigned int vWidth, unsigned int vHeight)
{
	OpenGL_LIB::Utils::initProgram(vWidth, vHeight);
	
	m_WindowWidth = vWidth;
	m_WindowHeight = vHeight;
	
	m_pShader = std::make_shared<OpenGL_LIB::CShader>();
	m_pShader->addShader("modelShader_VS.glsl", GL_VERTEX_SHADER);
	m_pShader->addShader("modelShader_FS.glsl", GL_FRAGMENT_SHADER);
	m_pShader->linkShader();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//camera init
	{
		const float CAMERA_FAR_PLANE = 5000.0f;
		const float CAMERA_NEAR_PLANE = 0.1f;

		const glm::vec3 CAMERA_DEFAULT_POSITION = hivePagedLOD::MODEL_MATRIX * glm::vec4(m_CameraDefualtPosition, 1.0f);

		OpenGL_LIB::CCamera::getInstance().setFarPlane(CAMERA_FAR_PLANE);
		OpenGL_LIB::CCamera::getInstance().setNearPlane(CAMERA_NEAR_PLANE);
		OpenGL_LIB::CCamera::getInstance().setPosition(CAMERA_DEFAULT_POSITION);
	}
}

//****************************************************************************
//FUNCTION:
void COGLGPUOperator::renderV(const std::vector<std::shared_ptr<SGPUMeshBuffer>>& vMeshBufferSet)
{
	++m_FrameID;
	glm::mat4 projectionMatrix = OpenGL_LIB::CCamera::getInstance().getProjectionMatrix();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_pShader->useShader();
	m_pShader->setMat4("uModelMatrix", MODEL_MATRIX);
	m_pShader->setMat4("uProjectionMatrix", projectionMatrix);
	m_pShader->setInt("uTextureDiffuse", 0);
	m_pShader->setMat4("uViewMatrix", m_PreviousViewMatrix);
	m_PreviousViewMatrix = OpenGL_LIB::CCamera::getInstance().getViewMatrix();

	for (unsigned int i = 0; i < vMeshBufferSet.size(); ++i)
	{
		if (vMeshBufferSet[i]->isValidV())
		{
			m_pShader->setInt("LODLevel", vMeshBufferSet[i]->LODLevel);
			__draw(dynamic_cast<SGPUMeshBuffer*>(vMeshBufferSet[i].get()));
		}
	}

	if (m_SaveCapture)
		saveCapture2FileV("./Capture/" + std::to_string(m_FrameID) + ".png", m_WindowWidth, m_WindowHeight);
	
	m_pUISystem->renderUISystem();
	glfwSwapBuffers(OpenGL_LIB::CWindow::getInstance().fetchWindowHandle());
	//std::cout << "draw" << std::endl;
}

//****************************************************************************
//FUNCTION:
void COGLGPUOperator::saveCapture2FileV(const std::string& vPath, unsigned int vWidth, unsigned int vHeight)
{
	unsigned char* data = new unsigned char[vWidth * vHeight * 4]();
	glReadPixels(0, 0, vWidth, vHeight, GL_BGRA, GL_UNSIGNED_BYTE, data);
	FIBITMAP* pImage = FreeImage_ConvertFromRawBits(data, vWidth, vHeight, vWidth * 4, 32, FI_RGBA_BLUE_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_RED_MASK, false);
	_ASSERTE(pImage);
	FreeImage_Save(FIF_PNG, pImage, vPath.c_str(), 0);
	FreeImage_Unload(pImage);
	delete[] data;
}

//****************************************************************************
//FUNCTION:
void COGLGPUOperator::prepareGUIV()
{
	m_pUISystem->newFrame();
	m_pUISystem->buildWidget();
}

//****************************************************************************
//FUNCTION:
void COGLGPUOperator::initFirstFrameStateV(const std::shared_ptr<IFrameState>& vFrameState)
{
	m_pUISystem = std::make_shared<CMyUISystem>();
	m_pUISystem->initUISystem();
	
	auto pFrameState = dynamic_cast<SFrameState*>(vFrameState.get());
	_ASSERTE(pFrameState);
	m_pUISystem->setCameraSpeed(pFrameState->CameraSpeed);
	m_pUISystem->setBufferLimitSize(pFrameState->BufferLimitSize);
	m_pUISystem->setLoadLimitSizePerFrame(pFrameState->LoadLimitSizePerFrame);
	m_pUISystem->setLoadParentMaxLevel(pFrameState->LoadParentMaxLevel);
	m_pUISystem->setLoadStrategy(pFrameState->LoadStrategy);
	m_pUISystem->initProxy();
}

//****************************************************************************
//FUNCTION:
std::shared_ptr<IFrameState> COGLGPUOperator::updateFrameStateV()
{
	OpenGL_LIB::CDeviceInputHandler::getInstance().recordTime();
	glfwPollEvents();
	OpenGL_LIB::CDeviceInputHandler::getInstance().doMovement();
	
	CSceneVisitor::getInstance()->visit(static_cast<unsigned int>(m_FrameID));
	if (OpenGL_LIB::CCamera::getInstance().getSaveRecordFlag())
		OpenGL_LIB::CCamera::getInstance().recordCameraInfo();

	OpenGL_LIB::CCamera::getInstance().setSpeed(m_pUISystem->getCameraSpeed());
	auto pFrameState = std::make_shared<SFrameState>();
	if (windowShouldCloseV())
	{
		pFrameState->EndFlag = true;
		return pFrameState;
	}
	pFrameState->EndFlag						= false;
	pFrameState->CameraSpeed					= m_pUISystem->getCameraSpeed();
	pFrameState->BufferLimitSize				= m_pUISystem->getBufferLimitSize();
	pFrameState->LoadLimitSizePerFrame			= m_pUISystem->getLoadLimitSizePerFrame();
	pFrameState->LoadParentMaxLevel				= m_pUISystem->getLoadParentMaxLevel();
	pFrameState->LoadStrategy					= m_pUISystem->getLoadStrategy();
	
	__dumpViewInfo(&OpenGL_LIB::CWindow::getInstance(), &OpenGL_LIB::CCamera::getInstance(), pFrameState->ViewInfo);
	//OpenGL_LIB::CCamera::getInstance().outputCameraPosition();
	return pFrameState;
}

//****************************************************************************
//FUNCTION:
void COGLGPUOperator::__draw(const SGPUMeshBuffer* vMesh)
{
	glBindVertexArray(static_cast<unsigned int>(vMesh->pGPUGeometryBuffer->getVAO()));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, static_cast<unsigned int>(vMesh->pGPUTextureBuffer->getTextureID()));
	glDrawElements(GL_TRIANGLES, vMesh->pGPUGeometryBuffer->getIndexCount(), GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

//****************************************************************************
//FUNCTION:
void COGLGPUOperator::__draw(const SMemoryMeshBuffer* vMesh)
{
	unsigned int VAO, VBO, IBO, Texture;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glGenTextures(1, &Texture);

	auto pGeometry = vMesh->pGeometry;

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, pGeometry->VertexCount * OSG_VERTEX_SIZE, pGeometry->pVertexData.get(), GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, pGeometry->IndexCount * sizeof(unsigned int), pGeometry->pIndexData.get(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	const STextureInfo& TexInfo = vMesh->pTexture->TexInfo;

	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//notice:temp internal format
	unsigned int InternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
	glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, InternalFormat, TexInfo.Width, TexInfo.Height, 0, TexInfo.Size, vMesh->pTexture->pTextureData.get());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);

	glDrawElements(GL_TRIANGLES, pGeometry->IndexCount, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VAO);
}

//****************************************************************************
//FUNCTION:
void COGLGPUOperator::__dumpViewInfo(const OpenGL_LIB::CWindow* vWindow, const OpenGL_LIB::CCamera* vCamera, SViewInfo& voViewInfo)
{
	voViewInfo.ViewPortInfo.Height = vWindow->getHeight();
	voViewInfo.ViewPortInfo.Width = vWindow->getWidth();
	voViewInfo.CameraInfo.FOV = vCamera->getFOV();
	voViewInfo.CameraInfo.FarPlane = vCamera->getFarPlane();
	voViewInfo.CameraInfo.Front = vCamera->getFront();
	voViewInfo.CameraInfo.NearPlane = vCamera->getNearPlane();
	voViewInfo.CameraInfo.Position = vCamera->getPosition();
	voViewInfo.CameraInfo.Up = vCamera->getUp();
}

//****************************************************************************
//FUNCTION:
bool COGLGPUOperator::windowShouldCloseV()
{
	return glfwWindowShouldClose(OpenGL_LIB::CWindow::getInstance().fetchWindowHandle());
}

//****************************************************************************
//FUNCTION:
void COGLGPUOperator::endV()
{
	glfwTerminate();
	m_pUISystem->destoryUISystem();
	if (OpenGL_LIB::CCamera::getInstance().getSaveRecordFlag())
		OpenGL_LIB::CCamera::getInstance().outputCameraInfo(CSceneVisitor::getInstance()->getRecordFilePath());
}