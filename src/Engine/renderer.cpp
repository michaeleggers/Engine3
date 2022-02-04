#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


#include <SDL.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

#include "platform.h"
#include "renderer.h"
#include "player.h"

AnimatedModel Renderer::RegisterModel(std::string model)
{
	AnimatedModel animModel = {};

	// figure out extension (.gpmesh is json, later: binary format)

	// CPP Streams, urgh. TODO: replace later
	std::ifstream modelFile;
	std::stringstream ss;
	modelFile.open(model, std::ofstream::in);
	ss << modelFile.rdbuf();
	std::string data = ss.str();
	modelFile.close();

	// Parse the json
	rapidjson::Document doc;
	doc.Parse(data.c_str());
	
	// Textures
	rapidjson::Value& textures = doc["textures"];
	for (
		rapidjson::Value::ConstValueIterator itr = textures.Begin(); 
		itr != textures.End(); 
		++itr) 
	{
		SDL_Log("%s\n", itr->GetString());
		//RegisterTexture(itr->GetString()); // TODO: Texture loading
	}

	// Vertices
	rapidjson::Value& vertexArray = doc["vertices"];
	assert(vertexArray.IsArray());
	rapidjson::SizeType vertexCount = vertexArray.Size();
	std::vector<VertexFormatAnimatedModel> vertices(vertexCount); // allocate and zero init
	for (rapidjson::SizeType i = 0; i < vertexCount; i++) {
		rapidjson::Value& currentVertex = vertexArray[i];		
		VertexFormatAnimatedModel* v = &vertices[i];
		for (rapidjson::SizeType j = 0; j < currentVertex.Size(); j++) {
			v->pos.x = currentVertex[0].GetFloat();
			v->pos.y = currentVertex[1].GetFloat();
			v->pos.z = currentVertex[2].GetFloat();
			
			v->normal.x = currentVertex[3].GetFloat();
			v->normal.y = currentVertex[4].GetFloat();
			v->normal.z = currentVertex[5].GetFloat();

			v->boneIdx0 = currentVertex[6].GetInt();
			v->boneIdx1 = currentVertex[7].GetInt();
			v->boneIdx2 = currentVertex[8].GetInt();
			v->boneIdx3 = currentVertex[9].GetInt();

			v->boneWeight0 = currentVertex[10].GetInt();
			v->boneWeight1 = currentVertex[11].GetInt();
			v->boneWeight2 = currentVertex[12].GetInt();
			v->boneWeight3 = currentVertex[13].GetInt();

			v->uv.x = currentVertex[14].GetFloat();
			v->uv.y = currentVertex[15].GetFloat();
		}		
	}

	return animModel;
}

