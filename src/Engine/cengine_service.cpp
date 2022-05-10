#include <wchar.h>
#include <stdio.h>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "platform.h"
#include "cengine_service.h"
#include "interface.h"
#include "player.h"

void CEngineService::DebugOut(wchar_t const * str)
{
    wprintf(L"ENGINE DBG OUT: %s\n", str);
}

Player * CEngineService::CreatePlayer(glm::vec3 startPos, std::string model)
{
    Player player{};

    // Register Model. This will upload vertex/index-data to GPU.
    AnimatedModel modelData = m_Renderer->RegisterModel(model);
    
    player.animModel = modelData;
    player.modelName = model;
    player.pos = startPos;
    m_Players.push_back(player);

    return &m_Players.back();
}

Camera* CEngineService::CreateCamera(glm::vec3 pos)
{
    Camera* camera = new Camera(pos);
    m_ActiveCamera = camera;

    return m_ActiveCamera;
}

void CEngineService::RenderFrame()
{
    m_Renderer->RenderFrame(m_Players, m_ActiveCamera);
}
