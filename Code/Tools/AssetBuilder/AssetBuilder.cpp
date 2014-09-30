// Header Files
//=============
#include "PreCompiled.h"
#include "AssetBuilder.h"

lua_State* AssetBuilder::mluaState = NULL;

AssetBuilder::AssetBuilder()
{
	if (!GetAssetBuilderEnvironmentVariable("AuthoredAssetDir", mAuthoredAssetDir))
	{
		assert(false);
	}
	if (!GetAssetBuilderEnvironmentVariable("BuiltAssetDir", mBuiltAssetDir))
	{
		assert(false);
	}

	std::string scriptDir;
	if (!GetAssetBuilderEnvironmentVariable("ScriptDir", scriptDir))
	{
		assert(false);
	}

	if (!InitializeLua(scriptDir, "BuildAssets.lua"))
	{
		assert(false);
	}
}

AssetBuilder::AssetBuilder(const std::string & i_AuthoredAssetDir, 
							const std::string & i_BuiltAssetDir, 
							const std::string & i_ScriptDir,
							const std::string & i_RelativeScriptPath) :
	mAuthoredAssetDir(i_AuthoredAssetDir), 
	mBuiltAssetDir(i_BuiltAssetDir)
{
	if (!InitializeLua(i_ScriptDir, i_RelativeScriptPath))
	{
		assert(false);
	}
}

AssetBuilder::~AssetBuilder()
{
	ShutDownLua();
}

AssetBuilder * AssetBuilder::Create(const std::string & i_RelativeScriptPath)
{
	std::string AuthoredAssetDir;
	std::string BuiltAssetDir;
	std::string scriptDir;

	if (!GetAssetBuilderEnvironmentVariable("AuthoredAssetDir", AuthoredAssetDir))
	{
		return NULL;
	}
	if (!GetAssetBuilderEnvironmentVariable("BuiltAssetDir", BuiltAssetDir))
	{
		return NULL;
	}

	if (!GetAssetBuilderEnvironmentVariable("ScriptDir", scriptDir))
	{
		return NULL;
	}

	return new AssetBuilder(AuthoredAssetDir, BuiltAssetDir, scriptDir, i_RelativeScriptPath);
}


// Function Definitions
//=====================

bool AssetBuilder::BuildAsset(const char* i_relativePath)
{
	// The only thing that this C/C++ function does
	// is call the corresponding Lua BuildAsset() function

	// To call a function it must be pushed onto the stack
	lua_getglobal(mluaState, "BuildAsset");
	// This function has a single argument
	const int argumentCount = 1;
	{
		lua_pushstring(mluaState, i_relativePath);
	}
	// This function can potentially return two values:
	//	* A boolean indicating success or failure
	//	* An optional error message if building the asset failed
	const int returnValueCount = 2;
	const int noMessageHandler = 0;
	int result = lua_pcall(mluaState, argumentCount, returnValueCount, noMessageHandler);
	if (result == LUA_OK)
	{
		result = lua_toboolean(mluaState, -2);
		if (!result)
		{
			OutputErrorMessage(lua_tostring(mluaState, -1), i_relativePath);
		}
		lua_pop(mluaState, returnValueCount);
		return result != 0;
	}
	else
	{
		const char* errorMessage = lua_tostring(mluaState, -1);
		std::cerr << errorMessage << "\n";
		lua_pop(mluaState, 1);

		return false;
	}

	return true;
}

int AssetBuilder::OutputErrorMessage(lua_State* io_luaState)
{
	// Argument #1: The error message
	const char* i_errorMessage;
	if (lua_isstring(io_luaState, 1))
	{
		i_errorMessage = lua_tostring(io_luaState, 1);
	}
	else
	{
		return luaL_error(io_luaState,
			"Argument #1 must be a string (instead of a %s)",
			luaL_typename(io_luaState, 1));
	}
	// Argument #2: An optional file name
	const char* i_optionalFileName = NULL;
	if (lua_isstring(io_luaState, 2))
	{
		i_optionalFileName = lua_tostring(io_luaState, 2);
	}

	// Output the error message
	OutputErrorMessage(i_errorMessage, i_optionalFileName);

	return 0;
}

void AssetBuilder::OutputErrorMessage(const char* i_errorMessage, const char* i_optionalFileName)
{
	std::cerr << ( i_optionalFileName ? i_optionalFileName : "Asset Build" ) << ": error: " <<
		i_errorMessage << "\n";
}

// Windows Functions
//------------------

int AssetBuilder::CopyAssetFile(lua_State* io_luaState)
{
	// Argument #1: The source path
	const char* i_path_source;
	{
		if (lua_isstring(io_luaState, 1))
		{
			i_path_source = lua_tostring(io_luaState, 1);
		}
		else
		{
			return luaL_error(io_luaState, "Argument#1 should of string type, (instead of a %s)", luaL_typename(io_luaState, 1));
		}
	}

	// Argument #2: The target path
	const char* i_path_target;
	{
		if (lua_isstring(io_luaState, 2))
		{
			i_path_target = lua_tostring(io_luaState, 2);
		}
		else
		{
			return luaL_error(io_luaState, "Argument#2 should of string type, (instead of a %s)", luaL_typename(io_luaState, 2));
		}
	}

	// Copy the file
	const BOOL noErrorIfTargetFileAlreadyExists = FALSE;
	if (CopyFile(i_path_source, i_path_target, noErrorIfTargetFileAlreadyExists) != FALSE)
	{
		lua_pushboolean(io_luaState, true); // If successful, return a boolean "true"
		int returnValueCount = 1;

		return returnValueCount;
	}
	else
	{
		const std::string errorMessage = GetLastWindowsError();
		return luaL_error(io_luaState, errorMessage.c_str());	// On failure, return a boolean "false" _and_ the error message

	}

	int returnValueCount = 0;
	return returnValueCount;
}

int AssetBuilder::CreateDirectoryIfNecessary(lua_State* io_luaState)
{
	// Argument #1: The path
	std::string i_path;
	{
		if (lua_isstring(io_luaState, 1))
		{
			i_path = lua_tostring(io_luaState, 1);
		}
		else
		{
			return luaL_error(io_luaState, "Argument#1 should of string type, (instead of a %s)", luaL_typename(io_luaState, 1));
		}
	}

	// If the path is to a file (likely), remove it so that only the directory remains
	std::string directory;
	{
		size_t pos_slash = i_path.find_last_of("\\/");
		if (pos_slash != std::string::npos)
		{
			directory = i_path.substr(0, pos_slash);
		}
		else
		{
			directory = i_path;
		}
	}

	// Get the path in a form Windows likes (without any ".."s).
	// Windows requires a character buffer
	// to copy the path variable into.
	// An arbitrary value is chosen that "should" be "big enough":
	const DWORD maxCharacterCount = MAX_PATH;
	char buffer[maxCharacterCount];
	{
		char** pathIsDirectory = NULL;
		DWORD characterCount = GetFullPathName(directory.c_str(), maxCharacterCount, buffer, pathIsDirectory);
		if (characterCount > 0)
		{
			if (characterCount <= maxCharacterCount)
			{
				// Create the directory
				int result;
				{
					HWND noWindowIsDisplayed = NULL;
					const SECURITY_ATTRIBUTES* useDefaultSecurityAttributes = NULL;
					result = SHCreateDirectoryEx(noWindowIsDisplayed, buffer, useDefaultSecurityAttributes);
				}
				if (result == ERROR_SUCCESS)
				{
					std::cout << "Created directory " << buffer << "\n";
					int returnValueCount = 0;
					return returnValueCount;
				}
				else if ((result == ERROR_FILE_EXISTS) || (result == ERROR_ALREADY_EXISTS))
				{
					// If the file already exists that's ok,
					// still don't return anything but don't print a message
					int returnValueCount = 0;
					return returnValueCount;
				}
				else
				{
					const std::string errorMessage = GetFormattedWindowsError(result);
					return luaL_error(io_luaState, errorMessage.c_str());	// Throw an error
				}
			}
			else
			{
				// If you're seeing this error you will need to increase maxCharacterCount
				std::stringstream errorMessage;
				errorMessage << "The full path of \"" << directory << "\" requires " << characterCount <<
					" characters and the provided buffer only has room for " << maxCharacterCount;
				return luaL_error(io_luaState, errorMessage.str().c_str());	// Throw an error
			}
		}
		else
		{
			const std::string errorMessage = GetLastWindowsError();
			return luaL_error(io_luaState, errorMessage.c_str());	// Throw an error
		}
	}
}

int AssetBuilder::DoesFileExist(lua_State* io_luaState)
{
	const char* i_FilePath;
	{
		if (lua_isstring(io_luaState, 1))
		{
			i_FilePath = lua_tostring(io_luaState, 1);
		}
		else
		{
			return luaL_error(io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename(io_luaState, 1));
		}
	}

	// Try to get information about the file
	WIN32_FIND_DATA fileData;
	HANDLE file = FindFirstFile(i_FilePath, &fileData);
	if (file != INVALID_HANDLE_VALUE)
	{
		if (FindClose(file) != FALSE)
		{
			lua_pushboolean(io_luaState, true);
			int returnValueCount = 1;
			return returnValueCount;
		}
		else
		{
			std::stringstream errorMessage; 
			errorMessage << GetLastWindowsError().c_str() << i_FilePath;
			return luaL_error(io_luaState, errorMessage.str().c_str());
		}
	}
	else
	{
		DWORD errorCode;
		std::string errorMessage = GetLastWindowsError(&errorCode);
		if ((errorCode == ERROR_FILE_NOT_FOUND) || (errorCode == ERROR_PATH_NOT_FOUND))
		{
			int returnValueCount = 1;
			lua_pushboolean(io_luaState, false);
			return returnValueCount;
		}
		else
		{
			std::stringstream errorMessage;
			errorMessage << GetLastWindowsError().c_str() << i_FilePath;
			return luaL_error(io_luaState, errorMessage.str().c_str());
		}
	}
}


bool AssetBuilder::GetAssetBuilderEnvironmentVariable(const char* i_key, std::string& o_value)
{
	// Windows requires a character buffer
	// to copy the environment variable into.
	// An arbitrary value is chosen that "should" be "big enough":
	const DWORD maxCharacterCount = 512;
	char buffer[maxCharacterCount];
	// Ask Windows for the environment variable
	const DWORD characterCount = GetEnvironmentVariable( i_key, buffer, maxCharacterCount );
	if ( characterCount > 0 )
	{
		if ( characterCount <= maxCharacterCount )
		{
			o_value = buffer;
			return true;
		}
		else
		{
			// If you're seeing this error you will need to increase maxCharacterCount
			std::stringstream errorMessage;
			errorMessage << "The environment variable \"" << i_key << "\" requires " << characterCount <<
				" characters and the provided buffer only has room for " << maxCharacterCount;
			OutputErrorMessage( errorMessage.str().c_str() );
			return false;
		}
	}
	else
	{
		DWORD errorCode;
		std::string errorString = GetLastWindowsError( &errorCode );
		if ( errorCode == ERROR_ENVVAR_NOT_FOUND )
		{
			// If you're seeing this error and the environment variable is spelled correctly
			// it _probably_ means that you are debugging and haven't set up the environment.
			//	* Right click the project name and choose "Properties"
			//	* In the tree view to the left select "Configuration Properties->Debugging"
			//	* In the "Environment" field make sure that you have this environment variable set like:
			//		* someKey=$(someKey)
			std::string errorMessage = std::string( "The environment variable \"" ) +
				i_key + "\" doesn't exist";
			OutputErrorMessage( errorMessage.c_str() );
		}
		else
		{
			std::string errorMessage = std::string( "Windows failed to get the environment variable \"" ) +
				i_key + "\": " + errorString;
			OutputErrorMessage( errorMessage.c_str() );
		}
		return false;
	}
}

std::string AssetBuilder::GetFormattedWindowsError(const DWORD i_errorCode)
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
		const DWORD storedCharacterCount = FormatMessage( formattingOptions, messageIsFromWindows, i_errorCode,
			useTheDefaultLanguage, reinterpret_cast<LPSTR>( &messageBuffer ), minimumCharacterCountToAllocate, insertsAreIgnored );
		if ( storedCharacterCount != 0 )
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
		LocalFree( messageBuffer );
	}
	return errorMessage;
}

std::string AssetBuilder::GetLastWindowsError(DWORD* o_optionalErrorCode)
{
	// Windows stores the error as a code
	const DWORD errorCode = GetLastError();
	if ( o_optionalErrorCode )
	{
		*o_optionalErrorCode = errorCode;
	}
	return GetFormattedWindowsError( errorCode );
}

int AssetBuilder::GetLastWriteTime(lua_State* io_luaState)
{
	//Argument#1: The path
	const char * i_path;
	{
		if (lua_isstring(io_luaState, 1))
		{
			i_path = lua_tostring(io_luaState, 1);
		}
		else
		{
			return luaL_error(io_luaState, "Argument#1 should be of type string (instead of %s)", 
								luaL_typename(io_luaState, 1));
		}
	}

	// Get the last time that the file was written to
	ULARGE_INTEGER lastWriteTime_int;
	{
		WIN32_FIND_DATA fileData;
		{
			HANDLE file = FindFirstFile( i_path, &fileData );
			if ( file != INVALID_HANDLE_VALUE )
			{
				if ( FindClose( file ) == FALSE )
				{
					std::stringstream errorMessage;
					errorMessage << GetLastWindowsError() << i_path;
					return luaL_error(io_luaState, errorMessage.str().c_str());
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << GetLastWindowsError() << i_path;
				return luaL_error(io_luaState, errorMessage.str().c_str());
			}
		}
		FILETIME fileTime = fileData.ftLastWriteTime;
		lastWriteTime_int.HighPart = fileTime.dwHighDateTime;
		lastWriteTime_int.LowPart = fileTime.dwLowDateTime;
	}

	const lua_Number lastWriteTime = static_cast<lua_Number>(lastWriteTime_int.QuadPart);
	lua_pushnumber(io_luaState, lastWriteTime);

	int returnValueCount = 1;
	return returnValueCount;
}

bool AssetBuilder::InitializeLua(const std::string & i_ScriptDir, const std::string & i_RelativeScriptPath)
{
	// Create a new Lua state
	{
		mluaState = luaL_newstate();
		if (!mluaState)
		{
			return false;
		}
	}
	// Open the standard libraries
	luaL_openlibs(mluaState);
	// Register custom functions
	{
		lua_register(mluaState, "CopyAssetFile", CopyAssetFile);
		lua_register(mluaState, "CreateDirectoryIfNecessary", CreateDirectoryIfNecessary);
		lua_register(mluaState, "DoesFileExist", DoesFileExist);
		lua_register(mluaState, "GetLastWriteTime", GetLastWriteTime);
		lua_register(mluaState, "OutputErrorMessage", OutputErrorMessage);
	}

	// Load and execute the build script
	{
		std::string path;
		{
			path = i_ScriptDir + i_RelativeScriptPath;
		}
		const int result = luaL_dofile(mluaState, path.c_str());
		if (result != LUA_OK)
		{
			const char* errorMessage = lua_tostring(mluaState, -1);
			std::cerr << errorMessage << "\n";
			lua_pop(mluaState, 1);

			return false;
		}
	}

	return true;
}

bool AssetBuilder::ShutDownLua()
{
	if (mluaState)
	{
		lua_close(mluaState);
		mluaState = NULL;
	}

	return true;
}