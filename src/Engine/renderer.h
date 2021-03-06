#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <string>
#include <stdint.h>
#include <unordered_map>

#include <SDL.h>
#include <glm/glm.hpp>
#include <vkal.h>

#include "player.h"
#include "camera.h"

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

struct AnimatedModel_UB
{
	glm::mat4 modelMat;
};

struct ViewProj
{
	glm::mat4 viewMat;
	glm::mat4 projMat;
};

class Renderer 
{
public:
	Renderer(std::string relAssetPath) 
		: m_relAssetPath(relAssetPath)
	{
		m_ExePath = SDL_GetBasePath();
	}

	void											Init(SDL_Window* window);
	void											CreateAnimatedModelPipeline(std::string vertShaderFile, std::string fragShaderFile);
	AnimatedModel									RegisterModel(std::string model);
	void											RenderFrame(std::vector<Player> players, Camera * camera);

	SDL_Window*										m_Window;

	VkalInfo*										m_VkalInfo;
	VkPipeline										m_animatedModelPipeline;
	VkPipelineLayout								m_animatedModelLayout;
	std::vector<VkDescriptorSet>					m_DescriptorSets;

	std::string										m_ExePath;
	std::string										m_relAssetPath;

	std::unordered_map<std::string, AnimatedModel>	m_AnimatedModels;
	UniformBuffer									m_AnimatedModelUB;

	ViewProj										m_ViewProj;
	UniformBuffer									m_ViewProjUniform; // TODO: type should be called VkalUniformBuffer
};

#endif
