//#ifndef UNICODE
//#define UNICODE
//#endif

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>

#include <SDL.h>

#include "cengine_service.h"
#include "interface.h"
#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main(int argc, char** argv)
{
    SDL_Window* window = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("Hello SDL Window :)",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              800,
                              600,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    if ( !window )
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "FUCK!", "SDL: Could not create Vulkan Window!", NULL);
        exit(-1); // TODO: Log this?
    }

    // Window Icon
    char* basePath = SDL_GetBasePath();
    printf("%s\n", basePath);
    std::string    iconPath       = std::string(basePath) + "icon.png";
    int            iconWidth      = 0;
    int            iconHeight     = 0;
    int            iconComponents = 0;
    unsigned char* iconPixels     = stbi_load(iconPath.c_str(), &iconWidth, &iconHeight, &iconComponents, 4);
    SDL_Surface*   iconSurface    = SDL_CreateRGBSurfaceFrom(iconPixels,
                                                        iconWidth,
                                                        iconHeight,
                                                        4 * 8, // RGBA = 32bit
                                                        4 * iconWidth,
                                                        0x000000FF,
                                                        0x0000FF00,
                                                        0x00FF0000,
                                                        0xFF000000);
    SDL_SetWindowIcon(window, iconSurface);
    SDL_FreeSurface(iconSurface);
    stbi_image_free(iconPixels);

#if defined(WIN32)
    void* gameDLL = SDL_LoadObject("game.dll");
    if ( !gameDLL )
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "FUCK!", "SDL: Could not find game.dll!", NULL);
        exit(-1); // TODO: Log this?
    }
#elif defined(__linux__)
    void* gameDLL = SDL_LoadObject("./libGame.so");
    if ( !gameDLL )
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "FUCK!", "SDL: Could not find libGame.so!", NULL);
        exit(-1); // TODO: Log this?
    }
#endif

    // Load function from game.dll that gets the GameClient
    PFN_GET_GAME_CLIENT GetGameClient = (PFN_GET_GAME_CLIENT)SDL_LoadFunction(gameDLL, "GetGameClient");
    if ( !GetGameClient )
    {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR, "FUCK!", "SDL: Could not load function 'GetGameClient' from game-lib", NULL);
        exit(-1); // TODO: Log this?
    }

    // Init Engine system(s)
    Renderer* renderer = new Renderer("/data/"); // TODO: Set base-path properly via command-line or default to exe-dir.
    renderer->Init(window);

    // TODO: Pipelinecreation somewhere else and more 'generic'?
    renderer->CreateAnimatedModelPipeline("shaders/animatedModel_vert.spv", "shaders/animatedModel_frag.spv");

    IEngineService* engineService = new CEngineService("../data/", renderer);
    IGameClient*    gameClient    = GetGameClient(engineService);

    // Tell client that engine is ready.
    gameClient->OnEngineInitialized();

    // Loop
    e3Input input     = { { false }, { false } };
    bool    running   = true;
    Uint64  startTime = 0;
    Uint64  endTime   = 0;
    float   frameTime = 0.0f;
    while ( running )
    {

        startTime = SDL_GetTicks64();

        SDL_Event event;
        SDL_PollEvent(&event);
        if ( event.type == SDL_QUIT )
        {
            running = false;
        }

        if ( event.key.type == SDL_KEYDOWN )
        {
            input.scancodes[ event.key.keysym.scancode ] = true;
        }
        else if ( event.key.type == SDL_KEYUP )
        {
            input.scancodes[ event.key.keysym.scancode ] = false;
        }
        else if ( event.type == SDL_MOUSEBUTTONDOWN )
        {
            input.mouseButtonID[ event.button.button ] = true;
        }
        else if ( event.type == SDL_MOUSEBUTTONUP )
        {
            input.mouseButtonID[ event.button.button ] = false;
        }

        switch ( event.key.keysym.scancode )
        {
        case (SDL_SCANCODE_ESCAPE):
        {
            running = false;
        }
        break;
        }

        gameClient->Update(frameTime, input);

        engineService->RenderFrame();

        Uint64 endTime = SDL_GetTicks64();

        frameTime = float(endTime - startTime);
    }

    delete renderer;

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// Thanks for watching. See you next time.
