#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


#include <SDL.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

#include <vkal.h>

#include "platform.h"
#include "renderer.h"
#include "player.h"

void Renderer::Init(SDL_Window * window)
{
	char* device_extensions[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_MAINTENANCE3_EXTENSION_NAME
	};
	uint32_t device_extension_count = sizeof(device_extensions) / sizeof(*device_extensions);

	char* instance_extensions[] = {
	VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
#ifdef _DEBUG
	,VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
	};
	uint32_t instance_extension_count = sizeof(instance_extensions) / sizeof(*instance_extensions);

	char* instance_layers[] = {
		"VK_LAYER_KHRONOS_validation",
		"VK_LAYER_LUNARG_monitor"
	};
	uint32_t instance_layer_count = 0;
#ifdef _DEBUG
	instance_layer_count = sizeof(instance_layers) / sizeof(*instance_layers);
#endif

	vkal_create_instance_sdl(window, instance_extensions, instance_extension_count, instance_layers, instance_layer_count);

	VkalPhysicalDevice* devices = 0;
	uint32_t device_count;
	vkal_find_suitable_devices(device_extensions, device_extension_count, &devices, &device_count);
	assert(device_count > 0);
	printf("Suitable Devices:\n");
	for (uint32_t i = 0; i < device_count; ++i) {
		printf("    Phyiscal Device %d: %s\n", i, devices[i].property.deviceName);
	}
	vkal_select_physical_device(&devices[0]);
	m_VkalInfo = vkal_init(device_extensions, device_extension_count);
}

void Renderer::CreateAnimatedModelPipeline(void)
{

}

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

	// Indices
	rapidjson::Value& faceArray = doc["indices"];
	assert(faceArray.IsArray());
	rapidjson::SizeType faceCount = faceArray.Size();
	rapidjson::SizeType indexCount = faceCount * 3; // Assume each face is a triangle!
	std::vector<uint16_t> indices;
	for (rapidjson::SizeType i = 0; i < faceCount; i++) {
		rapidjson::Value& currentFace = faceArray[i];
		for (rapidjson::SizeType j = 0; j < 3; j++) {
			indices.push_back(currentFace[j].GetInt());
		}
	}

	uint64_t vertexOffset = vkal_vertex_buffer_add(&vertices[0], sizeof(VertexFormatAnimatedModel), vertexCount);
	uint64_t indexOffset = vkal_index_buffer_add(&indices[0], indexCount); // VKAL's default index buffer expects uint16_t!

	return animModel;
}
