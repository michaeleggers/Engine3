#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <SDL.h>

#include "platform.h"
#include "renderer.h"
#include "player.h"

AnimatedModel Renderer::RegisterModel(std::string model)
{
	AnimatedModel animModel = {};

	// figure out extension (.gpmesh is json, later: binary format)

	// CPP Streams, urgh. Pretty stupid! TODO: replace later
	std::ifstream modelFile;
	std::stringstream ss;
	modelFile.open(model, std::ofstream::in);
	ss << modelFile.rdbuf();
	std::string data = ss.str();
	SDL_Log("%s\n", data.c_str());
	modelFile.close();

	// Parse the json

	return animModel;


}
