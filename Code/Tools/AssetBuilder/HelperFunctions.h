/*
	This example program has the helper funtions in a separate file
	to make it easier for you to keep track of program flow
*/

#ifndef EAE6320_HELPERFUNCTIONS_H
#define EAE6320_HELPERFUNCTIONS_H

// Header Files
//=============

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
	#include <Windows.h>
#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN

#include <cstdint>
#include <string>

// Function Declarations
//======================

bool BuildAsset( const char* i_relativePath );

bool InitializeAssetBuild();
// Errors can be formatted a specific way so that they show up
// in Visual Studio's "Error List" tab
void OutputErrorMessage( const char* i_errorMessage, const char* i_optionalFileName = NULL );

// Windows Functions
//------------------

bool CopyFile( const char* i_path_source, const char* i_path_target );
bool CreateDirectoryIfNecessary( const std::string& i_path );
bool DoesFileExist( const char* i_path, bool& o_doesFileExist );
bool GetEnvironmentVariable( const char* i_key, std::string& o_value );
std::string GetFormattedWindowsError( const DWORD i_errorCode );
std::string GetLastWindowsError( DWORD* o_optionalErrorCode = NULL );
bool GetLastWriteTime( const char* i_path, uint64_t& o_lastWriteTime );

#endif	// EAE6320_HELPERFUNCTIONS_H
