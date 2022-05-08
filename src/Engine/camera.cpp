#include "camera.h"

#include <glm/gtx/quaternion.hpp>

void Camera::RotateAroundUp(float angle)
{
	m_Rotation = glm::angleAxis(angle, m_Up);
	glm::vec3 forward = glm::normalize(m_Center - m_Pos);
	forward = glm::rotate(m_Rotation, forward);
	m_Center = m_Pos + forward;
}

glm::mat4 Camera::ViewMat()
{
	return glm::lookAt(m_Pos, m_Center, m_Up);
}
