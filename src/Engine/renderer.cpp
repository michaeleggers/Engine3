#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <stdint.h>

#include <SDL.h>
//#include <SDL_vulkan.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vkal.h>

#include "platform.h"
#include "renderer.h"
#include "player.h"
#include "camera.h"

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
	SDL_Log("Suitable Devices:\n");
	for (uint32_t i = 0; i < device_count; ++i) {
		SDL_Log("    Phyiscal Device %d: %s\n", i, devices[i].property.deviceName);
	}
	vkal_select_physical_device(&devices[0]);
	m_VkalInfo = vkal_init(device_extensions, device_extension_count);

	m_Window = window;
}

static std::vector<uint8_t> loadBinaryFile(std::string file)
{
	// TODO: Use SDL_RWFromFile instead of stdio's FILE and fopen?
	FILE* hFile = fopen(file.c_str(), "rb");
	SDL_assert_always(hFile != NULL);
	fseek(hFile, 0L, SEEK_END);
	size_t vertShaderSize = ftell(hFile);
	fseek(hFile, 0L, SEEK_SET);
	std::vector<uint8_t> data(vertShaderSize);
	fread(&data[0], sizeof(char), vertShaderSize, hFile);
	fclose(hFile);

	return data;
}

void Renderer::CreateAnimatedModelPipeline(std::string vertShaderFile, std::string fragShaderFile)
{
	/* Load Shader code */
	std::vector<uint8_t> vertShader = loadBinaryFile(m_ExePath + m_relAssetPath + vertShaderFile);
	std::vector<uint8_t> fragShader = loadBinaryFile(m_ExePath + m_relAssetPath + fragShaderFile);
	ShaderStageSetup shaderStageSetup = vkal_create_shaders(&vertShader[0], vertShader.size(), &fragShader[0], fragShader.size());

	// CPP streams are nuts?
	//std::ifstream vertShaderStream;
	//vertShaderStream.open(vertShaderFile, std::ifstream::in | std::ifstream::binary);
	//uint8_t data = vertShaderStream.rdbuf();
	//vertShaderStream.close();

	/* Setup Vertex Layout */
	VkVertexInputBindingDescription vertex_input_bindings[] =
	{
		{ 
			0, 
			2*sizeof(glm::vec3)			// Position, Normal
			+ 2*4*sizeof(uint8_t)		// Bone Indices, Bone Weights, 4 uint8 each
			+ sizeof(glm::vec2),        // UV
			VK_VERTEX_INPUT_RATE_VERTEX 
		}
	};

	VkVertexInputAttributeDescription vertex_attributes[] =
	{
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },										// Position
		{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(glm::vec3) },						// Normal
		{ 2, 0, VK_FORMAT_R8G8B8A8_UINT, 2*sizeof(glm::vec3) },							// Bone Idx
		{ 3, 0, VK_FORMAT_R8G8B8A8_UNORM, 2*sizeof(glm::vec3) + 4*sizeof(uint8_t)},		// Bone Weight, Setting the format to VK_FORMAT_R8G8B8A8_UNORM converts from 8bit integer to float [0.0 - 1.0]
		{ 4, 0, VK_FORMAT_R32G32_SFLOAT, 2*sizeof(glm::vec3) + 2*4*sizeof(uint8_t)}		// UV 
	};
	uint32_t vertex_attribute_count = sizeof(vertex_attributes) / sizeof(*vertex_attributes);

	/* Descriptor Sets TODO: define those outside this function? */
	VkDescriptorSetLayoutBinding set_layout[] =
	{
		{
			0,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			1,
			VK_SHADER_STAGE_VERTEX_BIT,
			0
		}
	};
	VkDescriptorSetLayout descriptor_set_layout = vkal_create_descriptor_set_layout(set_layout, 1);

	VkDescriptorSetLayout layouts[] = {
		descriptor_set_layout
	};
	uint32_t descriptor_set_layout_count = sizeof(layouts) / sizeof(*layouts);
	VkDescriptorSet* descriptor_sets = (VkDescriptorSet*)malloc(descriptor_set_layout_count * sizeof(VkDescriptorSet));
	vkal_allocate_descriptor_sets(m_VkalInfo->default_descriptor_pool, layouts, descriptor_set_layout_count, &descriptor_sets);
	for (size_t i = 0; i < descriptor_set_layout_count; ++i) {
		m_DescriptorSets.push_back(descriptor_sets[i]);
	}
	
	/* Pipeline */
	VkPipelineLayout pipeline_layout = vkal_create_pipeline_layout(layouts, 1, NULL, 0);
	VkPipeline graphics_pipeline = vkal_create_graphics_pipeline(
		vertex_input_bindings, 1,
		vertex_attributes, vertex_attribute_count,
		shaderStageSetup, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL, VK_CULL_MODE_BACK_BIT, VK_POLYGON_MODE_FILL,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_FRONT_FACE_CLOCKWISE,
		m_VkalInfo->render_pass, pipeline_layout);


	m_animatedModelLayout   = pipeline_layout;
	m_animatedModelPipeline = graphics_pipeline;

	/* Update Descriptor Sets */
	m_ViewProjUniform = vkal_create_uniform_buffer(sizeof(ViewProj), 1, 0);
	vkal_update_descriptor_set_uniform(m_DescriptorSets[0], m_ViewProjUniform, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
}

static std::string loadTextFile(std::string file)
{
	std::ifstream iFileStream;
	std::stringstream ss;
	iFileStream.open(file, std::ifstream::in);
	ss << iFileStream.rdbuf();
	std::string data = ss.str();
	iFileStream.close();

	return data;
}

AnimatedModel Renderer::RegisterModel(std::string model) // TODO: check if model already loaded
{
	std::unordered_map<std::string, AnimatedModel>::iterator got = m_AnimatedModels.find(model);
	if (got != m_AnimatedModels.end()) {
		return got->second;
	}

	AnimatedModel animModel = {};

	// figure out extension (.gpmesh is json, later: binary format)

	// CPP Streams, urgh. TODO: replace later
	std::string modelFilePath = m_ExePath + m_relAssetPath + model;
	std::string data = loadTextFile(modelFilePath);

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

	animModel.vertexOffset = vkal_vertex_buffer_add(&vertices[0], sizeof(VertexFormatAnimatedModel), vertexCount);
	animModel.indexOffset  = vkal_index_buffer_add(&indices[0], indexCount); // VKAL's default index buffer expects uint16_t!
	animModel.indexCount   = indexCount;
	animModel.pipeline	   = m_animatedModelPipeline;
	animModel.pipelineLayout = m_animatedModelLayout;

	m_AnimatedModels.insert({ model, animModel });

	return animModel;
}

// TODO: Renderer gets a refresh definition with all the stuff that needs to be done.
//       For now just the player.
void Renderer::RenderFrame(std::vector<Player> players, Camera * camera)
{
	int width, height;
	SDL_GetWindowSize(m_Window, &width, &height);

	// update view-proj matrices
	m_ViewProj.viewMat = glm::lookAt(camera->m_Pos, camera->m_Center, camera->m_Up);
	m_ViewProj.projMat = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.01f, 1000.0f);
	//vkal_update_descriptor_set_uniform(m_DescriptorSets[0], m_ViewProjUniform, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	vkal_update_uniform(&m_ViewProjUniform, &m_ViewProj);

	{
		//vkDeviceWaitIdle(vkal_info->device);
		uint32_t image_id = vkal_get_image();

		VkCommandBuffer currentCmdBuffer = m_VkalInfo->default_command_buffers[image_id];

		vkal_set_clear_color({ 0.2f, 0.2f, 0.2f, 1.0f });

		vkal_begin_command_buffer(image_id);
		vkal_begin_render_pass(image_id, m_VkalInfo->render_pass);
		vkal_viewport(currentCmdBuffer,
			0, 0,
			(float)width, (float)height);
		vkal_scissor(currentCmdBuffer,
			0, 0,
			(float)width, (float)height);

		vkal_bind_descriptor_set(image_id, &m_DescriptorSets[0], m_animatedModelLayout);
		for (int i = 0; i < players.size(); ++i) {
			Player player = players[i];
			vkal_draw_indexed(image_id, m_animatedModelPipeline,
				player.animModel.indexOffset, player.animModel.indexCount,
				player.animModel.vertexOffset);
		}
		
		vkal_end_renderpass(image_id);
		vkal_end_command_buffer(image_id);

		vkal_queue_submit(&currentCmdBuffer, 1);

		vkal_present(image_id);
	}
}
