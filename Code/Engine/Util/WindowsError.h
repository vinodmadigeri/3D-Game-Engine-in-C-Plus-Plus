#ifndef __WINDOWS_ERROR_HANDLING_H
#define __WINDOWS_ERROR_HANDLING_H

#include "Precompiled.h"

namespace Engine
{
	std::string GetFormattedWindowsError(const DWORD i_errorCode);

	std::string GetLastWindowsError(DWORD* o_optionalErrorCode = NULL);
}

#endif //__WINDOWS_ERROR_HANDLING_H