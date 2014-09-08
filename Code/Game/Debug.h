#ifndef __ENGINE_DEBUG_H
#define __ENGINE_DEBUG_H

#include "PreCompiled.h"

const int MAX_DEBUG_STRING = 256;

// __BASE_FILENAME__ will contain only the file name.
#define __BASE_FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

namespace Engine
{
void DebugPrint(const char *pFormat, ...);

#if defined (_ENABLE_DEBUG_LOGS)
#define CONSOLE_PRINT Engine::DebugPrint("\n[%s:%d]",__BASE_FILENAME__, __LINE__);Engine::DebugPrint
#else
#define CONSOLE_PRINT(fmt, ...) void(0)

#endif
}


#endif //__ENGINE_DEBUG_H