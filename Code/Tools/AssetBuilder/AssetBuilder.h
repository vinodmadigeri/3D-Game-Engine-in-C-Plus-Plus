/*
	This example program has the helper funtions in a separate file
	to make it easier for you to keep track of program flow
*/

#ifndef __EAE2014_ASSETBUILDER_H
#define __EAE2014_ASSETBUILDER_H

// Header Files
//=============
#include "PreCompiled.h"
#include "../../External/Lua/Includes.h"

	class AssetBuilder
	{
	private:
		std::string mAuthoredAssetDir;
		std::string mBuiltAssetDir;
		static lua_State* mluaState;
		AssetBuilder();
		AssetBuilder(const std::string & i_AuthoredAssetDir, const std::string & i_BuiltAssetDir, const std::string & i_ScriptDir);

		// Errors can be formatted a specific way so that they show up
		// in Visual Studio's "Error List" tab
		static void OutputErrorMessage(const char* i_errorMessage, const char* i_optionalFileName = NULL);

		// Windows Functions
		//------------------
		static bool GetAssetBuilderEnvironmentVariable(const char* i_key, std::string& o_value);
		static int CopyAssetFile(lua_State* io_luaState);
		static int CreateDirectoryIfNecessary(lua_State* io_luaState);
		static int DoesFileExist(lua_State* io_luaState);
		static std::string GetFormattedWindowsError(const DWORD i_errorCode);
		static std::string GetLastWindowsError(DWORD* o_optionalErrorCode = NULL);
		static int GetLastWriteTime(lua_State* io_luaState);

		//Lua Functions
		//-------------
		static bool InitializeLua(const std::string & i_ScriptDir);

		static bool ShutDownLua();


	public:
		~AssetBuilder();
		// Function Declarations
		//======================
		static AssetBuilder * Create(void);
		bool BuildAsset(const char* i_relativePath);
	};


#endif	// __EAE2014_ASSETBUILDER_H
