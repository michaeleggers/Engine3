#include <stdio.h>

#include "interface.h"

#include <SDL.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <unordered_map>

#include "player.h"
#include "camera.h"
#include "input_handler.h"

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
    MyGame(void):
    m_Camera(NULL)
    {}
    ~MyGame() {}

	// Overrides from Interface
    void OnEngineInitialized(void);
    void Update(bool * scancodes);

    Camera* m_Camera;
};


static MyGame         * gameClient;
static IEngineService * engineService;


void MyGame::OnEngineInitialized(void)
{
    printf("FROM GAME DLL: Engine Initialized! Ready to GO!\n");
    engineService->DebugOut(L"From MyGame: Hellohoooo");

    Player * player = engineService->CreatePlayer(glm::vec3(0.0f, 0.0f, 0.0f), "models/policeman.gpmesh");
    //Player* player2 = engineService->CreatePlayer(glm::vec3(0.0f, 0.0f, 0.0f), "models/policeman.gpmesh");
    m_Camera = engineService->CreateCamera(glm::vec3(2.0f, -2.0f, 5.0f));

}

// TODO: extra file!
enum Action {
    ACTION_NONE = 0,
    MOVE_CAM_FORWARD,
    MOVE_CAM_BACKWARD,
    MOVE_CAM_LEFT,
    MOVE_CAM_RIGHT,
    ROTATE_CAM_SIDE_FORWARD,
    ROTATE_CAM_SIDE_BACKWARD,
    ROTATE_CAM_UP_LEFT,
    ROTATE_CAM_UP_RIGHT,
    RESET_CAM,
    MAX_ACTIONS
};
struct KeyToAction {
    uint32_t    scancode;
    Action      action;
};
static KeyToAction keyMappings[SDL_NUM_SCANCODES] = { };
void setupKeyMappings()
{
    // load from config
    keyMappings[SDL_SCANCODE_UP].action   = ROTATE_CAM_SIDE_FORWARD; 
    keyMappings[SDL_SCANCODE_DOWN].action = ROTATE_CAM_SIDE_BACKWARD;
    keyMappings[SDL_SCANCODE_LEFT].action = ROTATE_CAM_UP_LEFT;
    keyMappings[SDL_SCANCODE_RIGHT].action = ROTATE_CAM_UP_RIGHT;
    keyMappings[SDL_SCANCODE_W].action = MOVE_CAM_FORWARD;
    keyMappings[SDL_SCANCODE_S].action = MOVE_CAM_BACKWARD;
    keyMappings[SDL_SCANCODE_A].action = MOVE_CAM_LEFT;
    keyMappings[SDL_SCANCODE_D].action = MOVE_CAM_RIGHT;
    keyMappings[SDL_SCANCODE_R].action = RESET_CAM;
}

Action actionForKey(uint32_t scancode)
{
    return keyMappings[scancode].action;
}

void MyGame::Update(bool * scancodes)
{
    setupKeyMappings(); // TODO: setup once.
    glm::vec3 camForward = glm::normalize(m_Camera->m_Center - m_Camera->m_Pos);
    glm::vec3 camRight = glm::normalize(glm::cross(camForward, m_Camera->m_Up));

    for (uint32_t scancode = 0; scancode < SDL_NUM_SCANCODES; ++scancode) { // TODO: really need to iterate over all of these every frame?
        if (scancodes[scancode]) { 
            Action action = actionForKey(scancode);
            if (action != ACTION_NONE) { // TODO: switch
                if (action == ROTATE_CAM_SIDE_FORWARD)
                    m_Camera->RotateAroundSide(glm::radians(2.0f));
                if (action == ROTATE_CAM_SIDE_BACKWARD)
                    m_Camera->RotateAroundSide(glm::radians(-2.0f));
                if (action == ROTATE_CAM_UP_LEFT)
                    m_Camera->RotateAroundUp(glm::radians(2.0f));
                if (action == ROTATE_CAM_UP_RIGHT)
                    m_Camera->RotateAroundUp(glm::radians(-2.0f));

                if (action == MOVE_CAM_FORWARD)
                    m_Camera->Move(0.25f);
                if (action == MOVE_CAM_BACKWARD)
                    m_Camera->Move(-0.255f);
                if (action == MOVE_CAM_LEFT)
                    m_Camera->MoveSide(0.25f);
                if (action == MOVE_CAM_RIGHT)
                    m_Camera->MoveSide(-0.25);

                if (action == RESET_CAM)
                    m_Camera->ResetOrientation();
            }
        }
    }    
}



EXPORT_GAME_CLIENT(MyGame, gameClient, engineService);

