#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <string>

#include "player.h"

class Renderer 
{
public:
	Renderer() {};
	AnimatedModel RegisterModel(std::string model);
};

#endif
