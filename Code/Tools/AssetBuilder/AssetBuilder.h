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
		AssetBuilder(const AssetBuilder &i_other);
		AssetBuilder& operator=(const AssetBuilder & i_rhs);

		AssetBuilder(const std::string & i_AuthoredAssetDir,
						const std::string & i_BuiltAssetDir,
						const std::string & i_ScriptDir,
						const std::string & i_RelativeScriptPath);

		// Errors can be formatted a specific way so that they show up
		// in Visual Studio's "Error List" tab
		static void OutputErrorMessage(const char* i_errorMessage, const char* i_optionalFileName = NULL);

		// Windows Functions
		//------------------
		static bool GetAssetBuilderEnvironmentVariable(const char* i_key, std::string& o_value);
		static std::string GetFormattedWindowsError(const DWORD i_errorCode);
		static std::string GetLastWindowsError(DWORD* o_optionalErrorCode = NULL);

		//Lua Init and Shutdown
		//-------------
		static bool InitializeLua(const std::string & i_ScriptDir, const std::string & i_RelativeScriptPath);
		static bool ShutDownLua();

		//Functions called from Lua script
		//--------------------------------
		static int GetLastWriteTime(lua_State* io_luaState);
		static int CopyAssetFile(lua_State* io_luaState);
		static int CreateDirectoryIfNecessary(lua_State* io_luaState);
		static int DoesFileExist(lua_State* io_luaState);

	public:
		~AssetBuilder();
		// Function Declarations
		//======================
		static AssetBuilder * Create(const std::string & i_RelativeScriptPath);
		bool BuildAsset(const char* i_relativePath);
	};


#endif	// __EAE2014_ASSETBUILDER_H
