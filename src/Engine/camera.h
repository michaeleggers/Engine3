#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(glm::vec3 pos) : 
		m_Pos(pos),
		m_Center(glm::vec3(0)),
		m_Up(glm::vec3(0, 1, 0))
	{}

	glm::vec3 m_Pos;
	glm::vec3 m_Center;
	glm::vec3 m_Up;
};

#endif