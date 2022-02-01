#include <stdio.h>
#include <string.h>

#include "interface.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <string>



class MyGame : public IGameClient
{
public:
    MyGame(void) {}
    ~MyGame() {}

	// Overrides from Interface
    void OnEngineInitialized(void);
    void Update(void);

	// Game Specific
	void Render(void);

};


static MyGame         * gameClient;
static IEngineService * engineService;


void MyGame::OnEngineInitialized(void)
{
    printf("FROM GAME DLL: Engine Initialized! Ready to GO!\n");
    engineService->DebugOut(L"From MyGame: Hellohoooo");
}

void MyGame::Update(void)
{

}

// TODO: Functions for setting render specific stuff such as resolution, bit-depth, and so on. (check lithtech/irrlicht).

void MyGame::Render(void)
{

}

EXPORT_GAME_CLIENT(MyGame, gameClient, engineService);

