#include "camera.h"

#include <glm/gtx/quaternion.hpp>

void Camera::Move(float distance)
{
	glm::vec3 forward = glm::normalize(m_Center - m_Pos);
	m_Pos += distance*forward;
	m_Center = m_Pos + forward;
}

void Camera::MoveSide(float distance)
{
	glm::vec3 forward = glm::normalize(m_Center - m_Pos);
	glm::vec3 side = glm::normalize(glm::cross(m_Up, forward));
	m_Pos += distance * side;
	m_Center = m_Pos + forward;
}

void Camera::RotateAroundUp(float angle)
{
	glm::quat q = glm::angleAxis(angle, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec3 forward = glm::normalize(m_Center - m_Pos);
	
	forward = glm::rotate(q, forward);
	m_Up = glm::rotate(q, m_Up);
	m_Center = m_Pos + forward;
}

void Camera::RotateAroundSide(float angle)
{
	glm::vec3 forward = glm::normalize(m_Center - m_Pos);
	glm::vec3 side = glm::normalize(glm::cross(m_Up, forward));
	glm::quat q = glm::angleAxis(angle, side);
	m_Up = glm::rotate(q, m_Up);
	forward = glm::rotate(q, forward);
	m_Center = m_Pos + forward;
}

glm::mat4 Camera::ViewMat()
{
	return glm::lookAt(m_Pos, m_Center, m_Up);
}
