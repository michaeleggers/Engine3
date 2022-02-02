//#ifndef UNICODE
//#define UNICODE
//#endif



#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <wchar.h>

#include <SDL.h>

#include "interface.h"
#include "cengine_service.h"



int main(int argc, char** argv)
{
    SDL_Window* window = NULL;
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow(
        "Hello SDL Window :)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_VULKAN
    );

    if (!window) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
            "FUCK!",
            "SDL: Could not create Vulkan Window!",
            NULL);
        exit(-1); // TODO: Log this?
    }

    // Load Game DLL/.so on nix. TODO: Check if Linux or Win
    void * gameDLL = SDL_LoadObject("game.dll"); // Linux "game.so"
    if (!gameDLL) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
            "FUCK!",
            "SDL: Could not find game.dll!",
            NULL);
        exit(-1); // TODO: Log this?
    }

    // Loop
    bool running = true;
    while (running) {

        SDL_Event event;
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            running = false;
        }
        
        switch (event.key.keysym.scancode) 
        {
            case (SDL_SCANCODE_ESCAPE): { running = false; } break;
        }

    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//
//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
//{
//	HANDLE hProcess = GetCurrentProcess();
//	if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS) == 0) {
//		printf("WARNING: Could not set this process to HIGH_PRIORITY_CLASS\n");
//	}
//	
//
//    // Register the window class.
//    const wchar_t CLASS_NAME[] = L"AzTech Engine Window";
//
//    WNDCLASS wc = {};
//
//    wc.lpfnWndProc		= WindowProc;
//    wc.hInstance		= hInstance;
//    wc.lpszClassName	= CLASS_NAME;
//	wc.style			= CS_HREDRAW | CS_VREDRAW;
//
//	
//
//    RegisterClass(&wc);
//
//    // Create the window.
//
//    HWND hwnd = CreateWindowEx(
//		0,                              // Optional window styles.
//		CLASS_NAME,                     // Window class
//		L"AzTech Engine 0.0.1",			// Window text
//		CS_OWNDC,			            // Window style
//
//		// Size and position
//		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
//
//		NULL,       // Parent window    
//		NULL,       // Menu
//		hInstance,  // Instance handle
//		NULL        // Additional application data
//	);
//
//    if (hwnd == NULL)
//    {
//		return 0;
//    }
//
//    ShowWindow(hwnd, nCmdShow);
//
//    // init console
//    AllocConsole();
//    FILE* pCin;
//    FILE* pCout;
//    FILE* pCerr;
//    freopen_s(&pCin, "conin$", "r", stdin);
//    freopen_s(&pCout, "conout$", "w", stdout);
//    freopen_s(&pCerr, "conout$", "w", stderr);
//    
//
//    /* Create engine services so the GameDLL can call us. 
//       this class needs the other engine subsystems so it can actually
//       do useful things...
//    */
//    IEngineService * engineService = new CEngineService("../game/");
//   
//    /* Load the GameDLL */
//    //HMODULE GameDLL = LoadLibrary(L"game.dll");
//	void * GameDLL = SDL_LoadObject("game.dll");
//	if (!GameDLL) 
//	{
//		MessageBoxA(hwnd, "Failed to load game.dll!", "OH NO!!!", MB_OK | MB_ICONEXCLAMATION);
//		exit(-1);
//		// TODO: Do a proper shutdown of all the submodules
//	}
// 
//    //PFN_GET_GAME_CLIENT GetGameClient = (PFN_GET_GAME_CLIENT)GetProcAddress(GameDLL, "GetGameClient");
//	PFN_GET_GAME_CLIENT GetGameClient = (PFN_GET_GAME_CLIENT)SDL_LoadFunction(GameDLL, "GetGameClient");
//    assert(GetGameClient != NULL && "Failed to load GetGameClient func!");
//
//    /* Get GameClient Implementation from GameDLL */
//    IGameClient * gameClient = GetGameClient(engineService);
//    assert(gameClient != NULL);
//
//    /* GO */
//    gameClient->OnEngineInitialized();
//
//
//    int  running = 1;
//    while (running) 
//	{
//		MSG msg = {};
//		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//		{
//			if (msg.message == WM_QUIT) {
//				running = 0;
//			}
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//
//		gameClient->Update();
//    }
//
//    // Close Console
//    fclose(pCin);
//    fclose(pCout);
//    fclose(pCerr);
//    FreeConsole();
//    
//    return 0;
//}
//
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    switch (uMsg)
//    {
//    case WM_CLOSE:
//	{
//		PostQuitMessage(0);
//		return 0;
//	}
//	break;
//
//	default: 
//		return DefWindowProc(hwnd, uMsg, wParam, lParam);
//    }
//
//	return 0;
//}
