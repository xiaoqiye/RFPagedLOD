#include "Camera.h"
#include "OGLWindow.h"
#include <GLM/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>

using namespace OpenGL_LIB;


CCamera::CCamera()
{
	__updateCameraVectors();
}

void CCamera::recordCameraInfo()
{
	mRecordSet.emplace_back(mPosition.x);
	mRecordSet.emplace_back(mPosition.y);
	mRecordSet.emplace_back(mPosition.z);
	mRecordSet.emplace_back(mFront.x);
	mRecordSet.emplace_back(mFront.y);
	mRecordSet.emplace_back(mFront.z);
	mRecordSet.emplace_back(mUp.x);
	mRecordSet.emplace_back(mUp.y);
	mRecordSet.emplace_back(mUp.z);
}

//****************************************************************************
//FUNCTION:
void CCamera::outputCameraPosition() const
{
	std::cout << "X=" << mPosition.x << "  Y=" << mPosition.y << "  Z=" << mPosition.z << std::endl;
}

//****************************************************************************
//FUNCTION:
void CCamera::outputCameraInfo(const std::string& vPath)
{

	//auto Matrix = OpenGL_LIB::CCamera::getInstance().getViewMatrix();
	//std::cout << Matrix[0].x << " " << Matrix[0].y << " " << Matrix[0].z << " " << Matrix[0].w << std::endl;
	//std::cout << Matrix[1].x << " " << Matrix[1].y << " " << Matrix[1].z << " " << Matrix[1].w << std::endl;
	//std::cout << Matrix[2].x << " " << Matrix[2].y << " " << Matrix[2].z << " " << Matrix[2].w << std::endl;
	//std::cout << Matrix[3].x << " " << Matrix[3].y << " " << Matrix[3].z << " " << Matrix[3].w << std::endl;
	//std::cout << std::endl;

	//std::cout << mPosition.x << std::endl << mPosition.y << std::endl << mPosition.z << std::endl;
	//std::cout << mFront.x << std::endl << mFront.y << std::endl << mFront.z << std::endl;
	//std::cout << mUp.x << std::endl << mUp.y << std::endl << mUp.z << std::endl;
	//std::cout << std::endl;
	//
	std::ofstream FileStream(vPath);
	for (auto& i : mRecordSet)
		FileStream << i << std::endl;
	FileStream.close();
}

//****************************************************************************
//FUNCTION:
void CCamera::__updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	front.y = sin(glm::radians(mPitch));
	front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	mFront = glm::normalize(front);
	mRight = glm::normalize(glm::cross(mFront, mWorldUp));
	mUp = glm::normalize(glm::cross(mRight, mFront));
}

//****************************************************************************
//FUNCTION:
glm::mat4 CCamera::getViewMatrix()
{
	return glm::lookAt(mPosition, mPosition + mFront, mUp);
}

//****************************************************************************
//FUNCTION:
glm::mat4 CCamera::getProjectionMatrix()
{
	return glm::perspective(glm::radians(mFOV), static_cast<float>(CWindow::getInstance().getWidth()) / CWindow::getInstance().getHeight(), mNearPlane, mFarPlane);
}

//****************************************************************************
//FUNCTION:
void CCamera::processKeyBoard(Constants::ECameraMovement vMovement, float vDeltaTime)
{
	float velocity = mMovementSpeed * vDeltaTime;
	if (vMovement == Constants::ECameraMovement::FORWARD)
		mPosition += mFront * velocity;
	if (vMovement == Constants::ECameraMovement::BACKWARD)
		mPosition -= mFront * velocity;
	if (vMovement == Constants::ECameraMovement::LEFT)
		mPosition -= mRight * velocity;
	if (vMovement == Constants::ECameraMovement::RIGHT)
		mPosition += mRight * velocity;
	if (vMovement == Constants::ECameraMovement::UP)
		mPosition += mUp * velocity;
	if (vMovement == Constants::ECameraMovement::DOWN)
		mPosition -= mUp * velocity;
}

//****************************************************************************
//FUNCTION:
void CCamera::processMouseMovement(float vXOffset, float vYOffset, bool vConstrainPitch)
{
	mMouseSensitivity = 0.02;
	vXOffset *= mMouseSensitivity;
	vYOffset *= mMouseSensitivity;

	mYaw += vXOffset;
	mPitch += vYOffset;

	if (vConstrainPitch)
	{
		if (mPitch > 89.0f)
			mPitch = 89.0f;
		if (mPitch < -89.0f)
			mPitch = -89.0f;
	}

	__updateCameraVectors();
}

//****************************************************************************
//FUNCTION:
void CCamera::processMouseScroll(float vYOffset)
{
	if (mFOV >= 1.0f && mFOV <= 45.0f)
		mFOV -= vYOffset;
	if (mFOV <= 1.0f)
		mFOV = 1.0f;
	if (mFOV >= 45.0f)
		mFOV = 45.0f;
}