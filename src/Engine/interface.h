#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <stdint.h>
#include <string>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "camera.h"
#include "player.h"

#if defined(_WIN32) || defined(_WIN64)
#ifdef MYLIB_EXPORTS
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif
#else
#define GAME_API __attribute__((visibility("default")))
#endif

struct e3Input
{
    bool scancodes[ SDL_NUM_SCANCODES ];
    bool mouseButtonID[ 256 ];
};

class IEngineService
{
  public:
    virtual void    DebugOut(wchar_t const* str)                        = 0;
    virtual Player* CreatePlayer(glm::vec3 startPos, std::string model) = 0;
    virtual Camera* CreateCamera(glm::vec3 pos)                         = 0;
    virtual void    RenderFrame()                                       = 0;
};

// GameDLL must implement this, so the engine can call into
// the game when it needs to. eg: tell the game when the subsystems are
// ready and the game can start loading a level or whatever...
class IGameClient
{
  public:
    virtual void OnEngineInitialized(void)       = 0;
    virtual void Update(float dt, e3Input input) = 0;
};

typedef IGameClient* (*PFN_GET_GAME_CLIENT)(IEngineService* pEngineService);

#define EXPORT_GAME_CLIENT(GameClientImpl, gameClient, engineService)                                                  \
    extern "C"                                                                                                         \
    {                                                                                                                  \
        GAME_API IGameClient* GetGameClient(IEngineService* engineServiceFromEXE)                                      \
        {                                                                                                              \
            engineService = engineServiceFromEXE;                                                                      \
            gameClient    = new GameClientImpl();                                                                      \
            return gameClient;                                                                                         \
        }                                                                                                              \
    }

#endif
