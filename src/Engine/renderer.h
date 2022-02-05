#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <string>
#include <stdint.h>

#include <SDL.h>
#include <glm/glm.hpp>
#include <vkal.h>

#include "player.h"

struct VertexFormatAnimatedModel 
{
	glm::vec3 pos;
	glm::vec3 normal;
	
	// Bone indices
	uint8_t   boneIdx0;
	uint8_t   boneIdx1;
	uint8_t   boneIdx2;
	uint8_t   boneIdx3;
	// Matching bone weights
	uint8_t   boneWeight0;
	uint8_t   boneWeight1;
	uint8_t   boneWeight2;
	uint8_t   boneWeight3;

	glm::vec2 uv;
};

class Renderer 
{
public:
	Renderer() {}

	void			Init(SDL_Window* window);
	AnimatedModel	RegisterModel(std::string model);

	VkalInfo*		m_VkalInfo;
};

#endif
