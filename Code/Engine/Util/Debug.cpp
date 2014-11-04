
#include "PreCompiled.h"

#include "Debug.h"


namespace Engine
{
	/******************************************************************************
	 Function     : DebugPrint
	 Description  : Print the debug information to console of Visual studio.
	 Input        : const char *pFormat, ...
	 Output       : 
	 Return Value : void
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	void DebugPrint(const char *pFormat, ...)
	{
#ifdef _ENABLE_DEBUG_LOGS
		va_list Args;
		char aDebugString[MAX_DEBUG_STRING + 2] = {'\0'};
		unsigned long ulOutLength;

		if (pFormat == NULL)
		{
			printf("\nNull Pointer Received\n");
			return;
		}

		va_start(Args, pFormat);
	
		//ulInputLength = strlen(pFormat);
		//ulMaxLength = ulInputLength > MAX_DEBUG_STRING ? MAX_DEBUG_STRING : ulInputLength;

		ulOutLength = vsprintf_s(aDebugString, MAX_DEBUG_STRING, pFormat, Args);
		va_end(Args);

		//End with a new line and set null character
		aDebugString[ulOutLength++] = '\n';
		aDebugString[ulOutLength] = '\0';

		OutputDebugStringA(aDebugString);
		return;
#endif
		return;
	}
}