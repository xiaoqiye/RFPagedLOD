#pragma once
#include "Constants.h"
#include "Singleton.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace OpenGL_LIB {

	class CCamera : public CSingleton<CCamera>
	{
	public:
		~CCamera() = default;
		CCamera(const CCamera&) = delete;
		CCamera& operator= (const CCamera&) = delete;
		glm::mat4 getViewMatrix();
		glm::mat4 getProjectionMatrix();
		void setFarPlane(float vFarPlane) { mFarPlane = vFarPlane; }
		void setNearPlane(float vNearPlane) { mNearPlane = vNearPlane; }
		void setFOV(float vFOV) { mFOV = vFOV; }
		void setYaw(float vYaw) { mYaw = vYaw; __updateCameraVectors(); }
		float getYaw()const { return mYaw; }
		float getFOV() const { return mFOV; }
		float getFarPlane() const { return mFarPlane; }
		float getNearPlane() const { return mNearPlane; }
		float getCameraSpeed() const { return mMovementSpeed; }
		const glm::vec3& getPosition() const { return mPosition; }
		const glm::vec3& getFront() const { return mFront; }
		const glm::vec3& getUp() const { return mUp; }
		void processKeyBoard(Constants::ECameraMovement vMovement, float vDeltaTime);
		void processMouseMovement(float vXOffset, float vYOffset, bool vConstrainPitch = true);
		void processMouseScroll(float vYOffset);
		void setSpeed(float vSpeed) { mMovementSpeed = vSpeed; }
		void setPosition(const glm::vec3& vPosition) { mPosition = vPosition; }
		void setPosition(float vX, float vY, float vZ) { mPosition.x = vX; mPosition.y = vY; mPosition.z = vZ; }
		void setFront(const glm::vec3& vFront) { mFront = vFront; }
		void setFront(float vX, float vY, float vZ) { mFront.x = vX; mFront.y = vY; mFront.z = vZ; }
		void setUp(const glm::vec3& vUp) { mUp = vUp; }
		void setUp(float vX, float vY, float vZ) { mUp.x = vX; mUp.y = vY; mUp.z = vZ; }

		void recordCameraInfo();
		void outputCameraPosition() const;
		void outputCameraInfo(const std::string& vPath);
		bool getSaveRecordFlag() const { return mSaveRecordFlag; }
		void setSaveRecordFlag(bool vFlag) { mSaveRecordFlag = vFlag; }
	private:
		CCamera();
		void __updateCameraVectors();

		glm::vec3 mFront;
		glm::vec3 mUp;
		glm::vec3 mRight;
		glm::vec3 mPosition = glm::make_vec3(Constants::DEFAULT_CAMERA_POSITION);
		glm::vec3 mWorldUp = glm::make_vec3(Constants::WORLD_UP_VEC3);

		float mYaw = Constants::DEFAULT_YAW;
		float mPitch = Constants::DEFAULT_CAMERA_PITCH;

		float mMovementSpeed = Constants::DEFAULT_SPEED;
		float mMouseSensitivity = Constants::DEFAULT_SENSITIVITY;
		float mFOV = Constants::DEFAULT_ZOOM;

		float mNearPlane = Constants::DEFAULT_NEAR_PLANE;
		float mFarPlane = Constants::DEFAULT_FAR_PLANE;

		std::vector<float> mRecordSet;
		bool mSaveRecordFlag = false;
		friend class CSingleton<CCamera>;
	};
}
