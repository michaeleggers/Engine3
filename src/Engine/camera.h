#ifndef _CAMERA_H_
#define _CAMERA_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera
{
public:
	Camera(glm::vec3 pos) :
		m_Pos(pos),
		m_Center(glm::vec3(0)),
		m_Up(glm::vec3(0, 0, 1)),
		m_Rotation(glm::quat())
	{}

	void		RotateAroundUp(float angle);
	glm::mat4	ViewMat();

	glm::vec3 m_Pos;
	glm::vec3 m_Center;
	glm::vec3 m_Up;
	glm::quat m_Rotation;
};

#endif