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
    

    // Register Model. Model also contains texture info
    //AnimatedModel animModel = renderer->RegisterModel(model);
    
    m_Players.push_back(player);

    return &m_Players.back();
}