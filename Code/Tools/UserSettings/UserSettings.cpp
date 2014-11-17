// Header Files
//=============

#include "UserSettings.h"

#include <cmath>
#include <sstream>
#include <string>
#include "../../External/Lua/Includes.h"
#include "../BuilderHelper/UtilityFunctions.h"

// Static Data Initialization
//===========================

namespace
{
	unsigned int s_height = 480;
	bool s_isFullScreenModeEnabled = false;
	unsigned int s_width = 640;

	const char* s_userSettingsfileName = "settings.ini";
}

// Helper Function Declarations
//=============================

namespace UserSettings
{
	bool IsThisWidthSupported(int iWidth);
	bool InitializeIfNecessary();
	bool IsNumberAnInteger( const lua_Number i_number );
	bool LoadUserSettingsIntoLuaTable( lua_State& io_luaState, bool& o_doesUserSettingsFileExist );
	bool PopulateUserSettingsFromLuaTable( lua_State& io_luaState );
}

// Interface
//==========

unsigned int UserSettings::GetHeight()
{
	InitializeIfNecessary();
	return s_height;
}

unsigned int UserSettings::GetWidth()
{
	InitializeIfNecessary();
	return s_width;
}

bool UserSettings::IsFullScreenModeEnabled()
{
	InitializeIfNecessary();
	return s_isFullScreenModeEnabled;
}

// Helper Function Definitions
//============================

namespace UserSettings
{
	bool IsThisWidthSupported(int iWidth)
	{
		DEVMODE dm = { 0 };
		dm.dmSize = sizeof(dm);
		
		for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0; iModeNum++) 
		{
			if (dm.dmPelsWidth == iWidth)
				return true;
		}

		return false;
	}

	bool IsThisHeightSupported(int iHeight)
	{
		DEVMODE dm = { 0 };
		dm.dmSize = sizeof(dm);

		for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0; iModeNum++)
		{
			if (dm.dmPelsHeight == iHeight)
				return true;
		}

		return false;
	}

	bool InitializeIfNecessary()
	{
		static bool isInitialized = false;
		if ( isInitialized )
		{
			return true;
		}

		bool wereThereErrors = false;

		// Create a new Lua state
		lua_State* luaState = NULL;
		bool wasUserSettingsEnvironmentCreated = false;
		{
			luaState = luaL_newstate();
			if ( luaState == NULL )
			{
				MessageBox( NULL, "Failed to create a new Lua state", "No User Settings", MB_OK | MB_ICONERROR );
				wereThereErrors = true;
				goto CleanUp;
			}
		}
		// Create an empty table to be used as the Lua environment for the user settings
		{
			lua_newtable( luaState );
			wasUserSettingsEnvironmentCreated = true;
		}
		// Load the user settings
		bool doesUserSettingsFileExist;
		if ( LoadUserSettingsIntoLuaTable( *luaState, doesUserSettingsFileExist ) )
		{
			if ( doesUserSettingsFileExist )
			{
				// Populate the user settings in C from the user settings in the Lua environment
				if ( !PopulateUserSettingsFromLuaTable( *luaState ) )
				{
					wereThereErrors = true;
					goto CleanUp;
				}
			}
		}
		else
		{
			wereThereErrors = true;
			goto CleanUp;
		}

		// Free the Lua environment

	CleanUp:

		if ( luaState )
		{
			if ( wasUserSettingsEnvironmentCreated )
			{
				lua_pop( luaState, 1 );
			}
			// Sanity Check
			{
				int stackItemCount = lua_gettop( luaState );
				if ( stackItemCount != 0 )
				{
					std::ostringstream errorMessage;
					errorMessage << "There are still " << stackItemCount
						<< " items in the Lua stack when the Lua state is being closed";
					MessageBox( NULL, errorMessage.str().c_str(), "No User Settings", MB_OK | MB_ICONERROR );
					wereThereErrors = true;
				}
			}
			lua_close( luaState );
		}

		isInitialized = !wereThereErrors;
		return isInitialized;
	}

	bool IsNumberAnInteger( const lua_Number i_number )
	{
		lua_Number integralPart;
		lua_Number fractionalPart = modf( i_number, &integralPart );
		return integralPart == i_number;
	}

	bool LoadUserSettingsIntoLuaTable( lua_State& io_luaState, bool& o_doesUserSettingsFileExist )
	{
		// Load the file into a Lua function
		int result = luaL_loadfile( &io_luaState, s_userSettingsfileName );
		if ( result == LUA_OK )
		{
			o_doesUserSettingsFileExist = true;

			// Set the Lua function's environment
			{
				lua_pushvalue( &io_luaState, -2 );
				const char* upValueName = lua_setupvalue( &io_luaState, -2, 1 );
				if ( upValueName == NULL )
				{
					std::stringstream errorMessage;
					errorMessage << "Internal error setting the Lua environment for " << s_userSettingsfileName << "... This should never happen";
					MessageBox( NULL, errorMessage.str().c_str(), "No User Settings", MB_OK | MB_ICONERROR );
					lua_pop( &io_luaState, 2 );
					return false;
				}
			}
			// Call the Lua function
			{
				int noArguments = 0;
				int noReturnValues = 0;
				int noErrorMessageHandler = 0;
				result = lua_pcall( &io_luaState, noArguments, noReturnValues, noErrorMessageHandler );
				if ( result == LUA_OK )
				{
					return true;
				}
				else
				{
					std::string luaErrorMessage( lua_tostring( &io_luaState, -1 ) );
					lua_pop( &io_luaState, 1 );

					if ( result == LUA_ERRRUN )
					{
						std::stringstream errorMessage;
						errorMessage << "Error in " << s_userSettingsfileName << ": " << luaErrorMessage;
						MessageBox( NULL, errorMessage.str().c_str(), "No User Settings", MB_OK | MB_ICONERROR );
					}
					else
					{
						std::stringstream errorMessage;
						errorMessage << "Error processing user settings file: " << luaErrorMessage;
						MessageBox( NULL, errorMessage.str().c_str(), "No User Settings", MB_OK | MB_ICONERROR );
					}

					return false;
				}
			}
		}
		else
		{
			o_doesUserSettingsFileExist = false;

			std::string luaErrorMessage( lua_tostring( &io_luaState, -1 ) );
			lua_pop( &io_luaState, 1 );

			if ( result == LUA_ERRFILE )
			{
				// If loading the file failed because the file doesn't exist it's ok
				if ( GetFileAttributes( s_userSettingsfileName ) == INVALID_FILE_ATTRIBUTES )
				{
					return true;
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "User settings file " << s_userSettingsfileName <<
						" exists but couldn't be opened or read: " << luaErrorMessage;
					MessageBox( NULL, errorMessage.str().c_str(), "No User Settings", MB_OK | MB_ICONERROR );
				}
			}
			else if ( result == LUA_ERRSYNTAX )
			{
				std::stringstream errorMessage;
				errorMessage << "Syntax error in " << s_userSettingsfileName << ": " << luaErrorMessage;
				MessageBox( NULL, errorMessage.str().c_str(), "No User Settings", MB_OK | MB_ICONERROR );
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "Error loading user settings file: " << luaErrorMessage;
				MessageBox( NULL, errorMessage.str().c_str(), "No User Settings", MB_OK | MB_ICONERROR );
			}

			return false;
		}
	}

	bool PopulateUserSettingsFromLuaTable( lua_State& io_luaState )
	{
		//Iterating through the constant key value pairs
		lua_pushnil(&io_luaState);
		int CurrentIndexOfConstantTable = -2;
		while (lua_next(&io_luaState, CurrentIndexOfConstantTable))
		{
			//Current Table is at -3 inside the while loop
			int IndexOfKey = -2; int IndexOfValue = -1;
			if (lua_type(&io_luaState, IndexOfKey) != LUA_TSTRING)
			{
				// Pop the returned key value pair on error
				lua_pop(&io_luaState, 2);
				goto OnExit;
			}

			//Store the valid key in a variable
			const char * OptionName = lua_tostring(&io_luaState, IndexOfKey);

			if (strcmp(OptionName, "width") == 0)
			{
				if (lua_type(&io_luaState, IndexOfValue) != LUA_TNUMBER)
				{
					std::stringstream errorMessage;
					errorMessage << "Ignoring the invalid entry for Screen Width: " << s_userSettingsfileName;
					MessageBox(NULL, errorMessage.str().c_str(), "No User Settings", MB_OK | MB_ICONERROR);

					// Pop the returned key value pair on error
					lua_pop(&io_luaState, 2);
					goto OnExit;
				}

				int tempWidth = lua_tointeger(&io_luaState, IndexOfValue);

				if (IsThisWidthSupported(tempWidth))
				{
					s_width = tempWidth;
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "Screen Width not supported :" << tempWidth;
					MessageBox(NULL, errorMessage.str().c_str(), "No User Settings", MB_OK | MB_ICONERROR);
				}
			}

			if (strcmp(OptionName, "height") == 0)
			{
				if (lua_type(&io_luaState, IndexOfValue) != LUA_TNUMBER)
				{
					std::stringstream errorMessage;
					errorMessage << "Ignoring the invalid entry for Screen Height: " << s_userSettingsfileName;
					MessageBox(NULL, errorMessage.str().c_str(), "No User Settings", MB_OK | MB_ICONERROR);

					// Pop the returned key value pair on error
					lua_pop(&io_luaState, 2);
					goto OnExit;
				}

				int tempHeight = lua_tointeger(&io_luaState, IndexOfValue);

				if (IsThisHeightSupported(tempHeight))
				{
					s_height = tempHeight;
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "Screen Height not supported: " << tempHeight;
					MessageBox(NULL, errorMessage.str().c_str(), "No User Settings", MB_OK | MB_ICONERROR);
				}
			}

			if (strcmp(OptionName, "fullscreen") == 0)
			{
				if (lua_type(&io_luaState, IndexOfValue) != LUA_TBOOLEAN)
				{
					std::stringstream errorMessage;
					errorMessage << "Ignoring the invalid entry for fullscreen: " << s_userSettingsfileName;
					MessageBox(NULL, errorMessage.str().c_str(), "No User Settings", MB_OK | MB_ICONERROR);

					// Pop the returned key value pair on error
					lua_pop(&io_luaState, 2);
					goto OnExit;
				}

				s_isFullScreenModeEnabled = lua_toboolean(&io_luaState, IndexOfValue) == 1 ? true : false;
			}

			//Pop the value, but leave the key
			lua_pop(&io_luaState, 1);
		}

	OnExit:

		return true;
	}
}
