
#ifndef __BUILDER_UTILITY_FUNCTIONS_H
#define __BUILDER_UTILITY_FUNCTIONS_H

/*
	This file contains useful utility functions for other build processes
*/

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
	#include <Windows.h>
#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN

#include <cstdint>
#include <string>

namespace BuilderHelper
{
	// Interface
	//==========

	bool CopyAssetFile( const char* i_path_source, const char* i_path_target,
		const bool i_shouldFunctionFailIfTargetAlreadyExists = false, std::string* o_errorMessage = NULL );
	bool CreateDirectoryIfNecessary( const std::string& i_path, std::string* o_errorMessage = NULL );
	bool DoesFileExist( const char* i_path, std::string* o_errorMessage = NULL, DWORD* o_errorCode = NULL );
	bool GetEnvironmentVariable( const char* i_key, std::string& o_value, std::string* o_errorMessage = NULL );
	std::string GetFormattedWindowsError( const DWORD i_errorCode );
	std::string GetLastWindowsError( DWORD* o_optionalErrorCode = NULL );
	bool GetLastWriteTime( const char* i_path, uint64_t& o_lastWriteTime, std::string* o_errorMessage = NULL );
	void OutputErrorMessage( const char* i_errorMessage, const char* i_optionalFileName = NULL );
	bool ExecuteCommand(const char* i_command, DWORD* o_exitCode, std::string* o_errorMessage = NULL);
	bool ExecuteCommand(const char* i_command, const char* i_optionalArguments, DWORD* o_exitCode, std::string* o_errorMessage = NULL);
}


#endif //__BUILDER_UTILITY_FUNCTIONS_H