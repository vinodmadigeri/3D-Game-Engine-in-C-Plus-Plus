#ifndef __LUA_HELPER
#define __LUA_HELPER
#include <cassert>
#include "../../External/Lua/Includes.h"

namespace LuaHelper
{
	void UnLoad_LuaTable(lua_State& luaState);

	bool Load_LuaFileAndTopTable(const char* i_Filepath, lua_State*& o_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
	);

	bool UnLoad_LuaFile(lua_State*& luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		);

	bool Load_LuaTable(lua_State& io_luaState, const char* TableName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		);


	bool GetStringValueFromKey(lua_State& io_luaState, const char* Key, std::string & o_Value
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		);

	//Template Functions
	//--------------
	template <typename T>
	bool GetEachNumberDataValuesInCurrentTable(lua_State& io_luaState, T * o_DataVariable, const unsigned int i_DataCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(o_DataVariable);
		//Iterating through every value table
		const int DataCount = luaL_len(&io_luaState, -1);

		if (DataCount != i_DataCount)
		{
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "Data count:" << DataCount << " while loading float values is not equal to expected data count:" << i_DataCount << "\n";
				*o_errorMessage = errorMessage.str();
			}

			return false;
		}

		for (int i = 1; i <= DataCount; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			const int currentIndexOfConstantDataTable = -2;
			lua_gettable(&io_luaState, currentIndexOfConstantDataTable);

			if (lua_type(&io_luaState, -1) != LUA_TNUMBER)
			{
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "value must be a number (instead of a " <<
						luaL_typename(&io_luaState, -1) << ")\n";
					*o_errorMessage = errorMessage.str();
				}

				//Pop the invalid data value from stack and return false on error
				lua_pop(&io_luaState, 1);

				return false;
			}

			o_DataVariable[i - 1] = static_cast<T>(lua_tonumber(&io_luaState, -1));

			//Pop the value from the stack since it is stored
			lua_pop(&io_luaState, 1);
		}
		//At this point all the values are stored in o_DataVariable

		return true;
	}


	template <typename T>
	bool GetNumberValueFromKey(lua_State& io_luaState, const char* Key, T & o_Value
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(Key != NULL);
		// Get the value of "name"
		{
			// Right now, the stack only has _one_ object,
			// which is the table from the asset file, at index -1.
			// We want to look up the value associated with the key "name":
			lua_pushstring(&io_luaState, Key);

			// Now, the key is at index -1
			// and the table is at index -2.

			// We can now call lua_gettable() to get the value associated
			// with the key at index -1:
			{
				const int currentIndexOfTable = -2;
				lua_gettable(&io_luaState, currentIndexOfTable);
			}

			// Now, the value that was at t[k] is at index -1
			// and the table is still at index -2.

			// First, then, we will make sure that a value (_any_ value) existed for the key:
			if (lua_isnil(&io_luaState, -1))
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "No value for \"" << Key << "\" was found in the asset table\n";

					*o_errorMessage = errorMessage.str().c_str();
				}
#endif
				lua_pop(&io_luaState, 1);
				// Now the only thing on the stack is the asset table at -1,
				// and the calling function will deal with this
				// (regardless of whether this function succeeds or fails).
				return false;
			}

			// If we reach this code then we know that a value exists,
			//Check if the type is a number
			if (lua_type(&io_luaState, -1) != LUA_TNUMBER)
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "The value for \"" << Key << "\" must be a number value "
						"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";

					*o_errorMessage = errorMessage.str().c_str();
				}
#endif
				// Pop the value
				lua_pop(&io_luaState, 1);
				// (The asset table is now at -1)
				return false;
			}

			// Now we know that the value stored in the table at the key is valid:
			{
				o_Value = static_cast<T>(lua_tonumber(&io_luaState, -1));

				// Pop the value
				lua_pop(&io_luaState, 1);
				// (The asset table is now at -1)
			}
		}

		return true;
	}


}

#endif //__LUA_HELPER