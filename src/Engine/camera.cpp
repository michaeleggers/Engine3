#include "camera.h"

#include <glm/gtx/quaternion.hpp>

void Camera::RotateAroundUp(float angle)
{
	glm::rotate(m_Rotation, angle, m_Up);
}

glm::mat4 Camera::ViewMat()
{
	glm::mat4 rotationMat = glm::toMat4(m_Rotation);
	glm::vec3 forward = glm::normalize(m_Center - m_Pos);
	forward = rotationMat * glm::vec4(forward, 1.0f);
	m_Center = m_Pos + forward;

	return glm::lookAt(m_Pos, m_Center, m_Up);
}
