#ifndef _CENGINE_SERVICE_H_
#define _CENGINE_SERVICE_H_

#include <wchar.h>
#include <string>
#include <vector>

#include <SDL.h>

#include "interface.h"
#include "platform.h"
#include "renderer.h"

class CEngineService : public IEngineService
{
public:
	CEngineService(std::string relAssetPath, Renderer * renderer) :
		m_relAssetPath(relAssetPath),
		m_Renderer(renderer)
	{
		//m_ExePath = atp_get_exe_path();
		m_ExePath = SDL_GetBasePath();
    }
    ~CEngineService() {}
    
    void						DebugOut(wchar_t const * str);
	Player *					CreatePlayer(glm::vec3 startPos, std::string model);
	void						RenderFrame(void);

	Renderer *					m_Renderer;
    std::string					m_ExePath;
	std::string					m_relAssetPath;
	std::vector<Player>         m_Players;
};

#endif
