#include <stdio.h>

#include "interface.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <string>

#include "player.h"

/* 
 What do I need ?

 - Pickups: Player walks over it and gets it (health, weapon, etc...)
 - Triggers: (invisible) geometry that triggers an action (open a door, move the elevator).
 - Player:  Controllable player
            Do not think about what part of the engine really is in control and how to
            'architect' the code. I can sort this out once I have a running thing going!
            Just let both game-code and engine see the player struct. DO NOT ABSTRACT! JUST BUILD WHAT I NEED!
 - Enemies: Run around, shoot, follow the player, ...
 - World: Static geometry (walls). Areas that hurt the player (lava) but are static.
 - World: Special geometry (doors, elevators)
 - Collisions: Collide Player <-> Enemies <-> World <-> Pickups <-> Triggers
               First: do it the naive way (no BSP, Octree, ...).
 - Camera:   FPS camera: that 'follows' the player.
             Debug camera: Fly around in the world.
 - Renderer: let the engine do the rendering. Game code just specifies all the entities (pickups, world, player, enemies, ...)
             and renderer sorts out what has to be done.
 - Load world, enemies, player-start-pos, etc. from file. For now create them manually.

 - Engine should keep track of all the stuff in the game.


 - CONCERNS:
    - Raw Pointers: not quite sure if I should return a pointer to the game for e.g. the Player.
      what if the pointer gets invalidated? A handle would solve this problem: I could first check
      if the pointer behind the handle actually is valid and only then return it. I'll see if it bites me :)

    - STL-Strings: sstream and string and ifstream etc. seem all very stupid. For now they work.
      Replace them later when actually things are running. Actually C-Libs fread seems to be ok(?), like I did it in VKAL's platform layer.

    - Pipeline creation: At the moment I create the pipeline within the renderer. But maybe it should be made more flexible later
      by just providing the function to create the pipeline. The function's input would just be the data, e.g. shader, descriptors, etc.


 - TODOs/Questions:
    - How to compile shaders during comp-time?


 - Some conclusions:
   -> It is more important to get things up and running quickly. Code is garbage anyway!
      Abstract away stuff like 'ReadFromFile' and then replace its implementation with something sane and not STL.

*/


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

    Player * player = engineService->CreatePlayer(glm::vec3(0), "models/policeman.gpmesh");
    
}

void MyGame::Update(void)
{
    // handle input

    
}

// TODO: Functions for setting render specific stuff such as resolution, bit-depth, and so on. (check lithtech/irrlicht).

void MyGame::Render(void)
{
    //renderer->drawFrame(camera, other things);

}

EXPORT_GAME_CLIENT(MyGame, gameClient, engineService);

