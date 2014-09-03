/*
	This example program has the helper funtions in a separate file
	to make it easier for you to keep track of program flow
*/

#ifndef __EAE2014_ASSETBUILDER_H
#define __EAE2014_ASSETBUILDER_H

// Header Files
//=============
#include "PreCompiled.h"


	class AssetBuilder
	{
	private:
		std::string mAuthoredAssetDir;
		std::string mBuiltAssetDir;
		
		AssetBuilder();
		AssetBuilder(std::string & i_AuthoredAssetDir, std::string & i_BuiltAssetDir);

		// Errors can be formatted a specific way so that they show up
		// in Visual Studio's "Error List" tab
		static void OutputErrorMessage(const char* i_errorMessage, const char* i_optionalFileName = NULL);

		// Windows Functions
		//------------------
		static bool GetAssetBuilderEnvironmentVariable(const char* i_key, std::string& o_value);
		static bool CopyAssetFile(const char* i_path_source, const char* i_path_target);
		static bool CreateDirectoryIfNecessary(const std::string& i_path);
		static bool DoesFileExist(const char* i_path, bool& o_doesFileExist);
		static std::string GetFormattedWindowsError(const DWORD i_errorCode);
		static std::string GetLastWindowsError(DWORD* o_optionalErrorCode = NULL);
		static bool GetLastWriteTime(const char* i_path, uint64_t& o_lastWriteTime);

	public:
		~AssetBuilder();
		// Function Declarations
		//======================
		static AssetBuilder * Create(void);
		bool BuildAsset(const char* i_relativePath);
	};





#endif	// __EAE2014_ASSETBUILDER_H
