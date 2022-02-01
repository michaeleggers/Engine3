#include <wchar.h>
#include <stdio.h>

#include "platform.h"
#include "cengine_service.h"
#include "interface.h"

void CEngineService::DebugOut(wchar_t const * str)
{
    wprintf(L"ENGINE DBG OUT: %s\n", str);
}