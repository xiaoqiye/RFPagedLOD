#pragma once
#include <glm/glm.hpp>

namespace hivePagedLOD
{
	class CBoundingSphere
	{
	public:
		CBoundingSphere() :
			CBoundingSphere(0.0f, 0.0f, 0.0f, 0.0f) {}

		CBoundingSphere(float vCenterX, float vCenterY, float vCenterZ, float vRadius) :
			CBoundingSphere(glm::vec3(vCenterX, vCenterY, vCenterZ), vRadius) {}

		CBoundingSphere(const glm::vec3 &vCenter, float vRadius) :
			m_Center(vCenter),
			m_Radius(vRadius) {}

		~CBoundingSphere() = default;

		const glm::vec3 &getCenter() const { return m_Center; }
		glm::vec4 getCenterVec4() const { return glm::vec4(m_Center, 1.0f); }
		float getRadius() const { return m_Radius; }

		void setCenter(const glm::vec3& vCenter) { m_Center = vCenter;}
		void setCenter(float vX, float vY, float vZ) { m_Center = glm::vec3(vX, vY, vZ);}
		void setRadius(float vRadius) { m_Radius = vRadius; }

		bool isValid() const { return m_Radius > 0.0f; }

	private:
		glm::vec3 m_Center;
		float m_Radius;
	};
}
