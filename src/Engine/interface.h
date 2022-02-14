#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <stdint.h>
#include <string>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "player.h"
#include "camera.h"

class IEngineService
{
public:
    virtual void		DebugOut(wchar_t const * str)    = 0;
    virtual Player*     CreatePlayer(glm::vec3 startPos, std::string model) = 0;
    virtual Camera*     CreateCamera(glm::vec3 pos) = 0;
     virtual void       RenderFrame() = 0;
};

// GameDLL must implement this, so the engine can call into
// the game when it needs to. eg: tell the game when the subsystems are
// ready and the game can start loading a level or whatever...
class IGameClient
{
public:
    virtual void		OnEngineInitialized(void) = 0;
    virtual void		Update(bool * scancodes) = 0;
};



typedef IGameClient * (*PFN_GET_GAME_CLIENT)(IEngineService * pEngineService);

#ifdef __cplusplus
extern "C" {
#endif

#define EXPORT_GAME_CLIENT(GameClientImpl, gameClient, engineService)\
    IGameClient * GetGameClient(IEngineService * engineServiceFromEXE) {\
    engineService = engineServiceFromEXE;\
    gameClient = new GameClientImpl();\
    return gameClient;\
    }\

#ifdef __cplusplus
}
#endif



#endif
