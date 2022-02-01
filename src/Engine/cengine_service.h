#ifndef _CENGINE_SERVICE_H_
#define _CENGINE_SERVICE_H_

#include <wchar.h>
#include <string>

#include "interface.h"
#include "platform.h"


class CEngineService : public IEngineService
{
public:
	CEngineService(std::string relGamePath) :
		m_RelGamePath(relGamePath)
	{
		m_ExePath = atp_get_exe_path();
    }
    ~CEngineService() {}
    
    void						DebugOut(wchar_t const * str);

    std::string					m_ExePath;
	std::string					m_RelGamePath;
};

#endif
