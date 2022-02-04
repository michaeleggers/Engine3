#ifndef _PLAYER_H_
#define _PLAYER_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct AnimatedModel {

};

struct AABB {
	glm::vec3 minXYZ;
	glm::vec3 maxXYZ;
};

struct Player {
	glm::vec3 pos;
	glm::quat orientation;
	AABB aabb;
	AnimatedModel aniModel;
};

#endif
