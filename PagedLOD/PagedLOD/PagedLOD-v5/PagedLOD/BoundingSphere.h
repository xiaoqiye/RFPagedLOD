#pragma once
#include <GLM/glm.hpp>

namespace hivePagedLOD
{
	class CBoundingSphere
	{
	public:
		CBoundingSphere() : CBoundingSphere(0.0f, 0.0f, 0.0f, 0.0f) {}
		CBoundingSphere(const glm::vec3 &vCenter, float vRadius) : m_Center(vCenter), m_Radius(vRadius) {}
		CBoundingSphere(float vCenterX, float vCenterY, float vCenterZ, float vRadius) : CBoundingSphere(glm::vec3(vCenterX, vCenterY, vCenterZ), vRadius) {}
		
		~CBoundingSphere() = default;

		const glm::vec3 &getCenter() const { _ASSERT(m_Center.length() > 0); return m_Center; }
		glm::vec4 getCenterVec4() const { _ASSERT(m_Center.length() > 0); return glm::vec4(m_Center, 1.0f); }
		float getRadius() const { _ASSERT(m_Radius > 0.0f); return m_Radius; }

		void setCenter(const glm::vec3& vCenter) { _ASSERT(vCenter.length() > 0); m_Center = vCenter; }
		void setCenter(float vCenterX, float vCenterY, float vCenterZ) { m_Center = glm::vec3(vCenterX, vCenterY, vCenterZ);}
		void setRadius(float vRadius) { _ASSERT(vRadius > 0); m_Radius = vRadius; }

		bool isValid() const { return m_Radius > 0.0f; }
		bool operator==(const CBoundingSphere& vBoundingSphere) const { return vBoundingSphere.getCenter() == this->getCenter() && vBoundingSphere.getRadius() == this->getRadius(); }

	private:
		glm::vec3 m_Center;
		float m_Radius;
	};
}
