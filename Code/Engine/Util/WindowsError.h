#ifndef __WINDOWS_ERROR_HANDLING_H
#define __WINDOWS_ERROR_HANDLING_H

#include "Precompiled.h"

namespace Engine
{
	std::string GetFormattedWindowsError(const DWORD i_errorCode)
	{
		std::string errorMessage;
		{
			const DWORD formattingOptions =
				// Get the error message from Windows
				FORMAT_MESSAGE_FROM_SYSTEM
				// The space for the error message should be allocated by Windows
				| FORMAT_MESSAGE_ALLOCATE_BUFFER
				// Any potentially deferred inserts should be ignored
				// (i.e. the error message should be in its final form)
				| FORMAT_MESSAGE_IGNORE_INSERTS;
			const void* messageIsFromWindows = NULL;
			const DWORD useTheDefaultLanguage = 0;
			char* messageBuffer = NULL;
			const DWORD minimumCharacterCountToAllocate = 1;
			va_list* insertsAreIgnored = NULL;
			const DWORD storedCharacterCount = FormatMessage(formattingOptions, messageIsFromWindows, i_errorCode,
				useTheDefaultLanguage, reinterpret_cast<LPSTR>(&messageBuffer), minimumCharacterCountToAllocate, insertsAreIgnored);
			if (storedCharacterCount != 0)
			{
				errorMessage = messageBuffer;
			}
			else
			{
				// If there's an error GetLastError() can be called again,
				// but that is too complicated for this program :)
				errorMessage = "Unknown Windows Error";
			}
			// Try to free the memory regardless of whether formatting worked or not,
			// and ignore any error messages
			LocalFree(messageBuffer);
		}
		return errorMessage;
	}

	std::string GetLastWindowsError(DWORD* o_optionalErrorCode = NULL)
	{
		// Windows stores the error as a code
		const DWORD errorCode = GetLastError();
		if (o_optionalErrorCode)
		{
			*o_optionalErrorCode = errorCode;
		}
		return GetFormattedWindowsError(errorCode);
	}
}
#endif //__WINDOWS_ERROR_HANDLING_H