#include <sstream>
#include <string>

#include "LuaHelper.h"


namespace LuaHelper
{
	void UnLoad_LuaTable(lua_State& luaState)
	{
		// Pop the table
		lua_pop(&luaState, 1);
	}



	// Load material file from path
	bool Load_LuaFileAndTopTable(const char* i_Filepath, lua_State*& o_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		//Lua Logic
		//---------
		assert(i_Filepath);
		bool wereThereErrors = false;

		// Create a new Lua state
		o_luaState = NULL;
		{
			o_luaState = luaL_newstate();
			if (!o_luaState)
			{
				wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					*o_errorMessage = "Failed to create a new Lua state\n";
				}
#endif
				goto OnExit;
			}
		}

		// Load the asset file as a "chunk",
		// meaning there will be a callable function at the top of the stack
		{
			const int luaResult = luaL_loadfile(o_luaState, i_Filepath);
			if (luaResult != LUA_OK)
			{
				wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					*o_errorMessage = lua_tostring(o_luaState, -1);
				}
#endif
				// Pop the error message
				lua_pop(o_luaState, 1);
				goto OnExit;
			}
		}

		// Execute the "chunk", which should load the asset
		// into a table at the top of the stack
		{
			const int argumentCount = 0;
			const int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
			const int noMessageHandler = 0;
			const int luaResult = lua_pcall(o_luaState, argumentCount, returnValueCount, noMessageHandler);
			if (luaResult == LUA_OK)
			{
				// A well-behaved asset file will only return a single value
				const int returnedValueCount = lua_gettop(o_luaState);
				if (returnedValueCount == 1)
				{
					// A correct asset file _must_ return a table
					if (!lua_istable(o_luaState, -1))
					{
						wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "Asset files must return a table (instead of a "
								<< luaL_typename(o_luaState, -1) << ")\n";

							*o_errorMessage = errorMessage.str();
						}
#endif
						// Pop the returned non-table value
						lua_pop(o_luaState, 1);
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;

#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Asset files must return a single table (instead of " <<
							returnedValueCount << " values)\n";

						*o_errorMessage = errorMessage.str();
					}
#endif
					// Pop every value that was returned
					lua_pop(o_luaState, returnedValueCount);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					*o_errorMessage = lua_tostring(o_luaState, -1);
				}
#endif
				// Pop the error message
				lua_pop(o_luaState, 1);
				goto OnExit;
			}
		}

		// If this code is reached the asset file was loaded successfully,
		// and its table is now at index -1

	OnExit:

		return !wereThereErrors;
	}


	bool UnLoad_LuaFile(lua_State*& luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(luaState);
		bool WereThereErrors = false;
		

		// If I haven't made any mistakes
		// there shouldn't be anything on the stack,
		// regardless of any errors encountered while loading the file:
		if (lua_gettop(luaState) != 0)
		{
			WereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "Top should be zero when unloading a lua file (instead of  " <<
					lua_gettop(luaState) << " value)\n";

				*o_errorMessage = errorMessage.str();

			}
#endif
		}

		lua_close(luaState);
		luaState = NULL;

		return !WereThereErrors;
	}


	bool Load_LuaTable(lua_State& io_luaState, const char* TableName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(TableName);
		bool wereThereErrors = false;

		// Right now the asset table is at -1.
		// After the following table operation it will be at -2
		// and the "textures" table will be at -1:
		lua_pushstring(&io_luaState, TableName);
		lua_gettable(&io_luaState, -2);
		// It can be hard to remember where the stack is at
		// and how many values to pop.
		// One strategy I would suggest is to always call a new function
		// When you are at a new level:
		// Right now we know that we have an original table at -2,
		// and a new one at -1,
		// and so we _know_ that we always have to pop at least _one_
		// value before leaving this function
		// (to make the original table be back to index -1).
		// If we don't do any further stack manipulation in this function
		// then it becomes easy to remember how many values to pop
		// because it will always be one.
		// This is the strategy I'll take in this example
		// (look at the "OnExit" label):
		if (!lua_istable(&io_luaState, -1))
		{
			wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "The value at \"" << TableName << "\" must be a table "
					"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";

				*o_errorMessage = errorMessage.str();
			}
#endif

			goto OnExit;
		}

	OnExit:

		return !wereThereErrors;
	}


	bool GetStringValueFromKey(lua_State& io_luaState, const char* Key, std::string & o_Value
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(Key);
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
			if (lua_type(&io_luaState, -1) != LUA_TSTRING)
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "The value for \"" << Key << "\" must be a string value "
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
				o_Value = (lua_tostring(&io_luaState, -1));

				// Pop the value
				lua_pop(&io_luaState, 1);
				// (The asset table is now at -1)
			}
		}

		return true;
	}


	bool GetEachStringValuesInCurrentTable(lua_State& io_luaState, std::string * o_StringArray, const unsigned int i_DataCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(o_StringArray);
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

			if (lua_type(&io_luaState, -1) != LUA_TSTRING)
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

			o_StringArray[i - 1] = (lua_tostring(&io_luaState, -1));

			//Pop the value from the stack since it is stored
			lua_pop(&io_luaState, 1);
		}
		//At this point all the values are stored in o_DataVariable

		return true;
	}

	bool GetEachStringValuesInCurrentTable(lua_State& io_luaState, std::vector<std::string> & o_StringVector, const unsigned int i_DataCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		//Iterating through every value table
		const int DataCount = GetTableValuesLength(io_luaState);

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

			if (lua_type(&io_luaState, -1) != LUA_TSTRING)
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

			o_StringVector.push_back(lua_tostring(&io_luaState, -1));

			//Pop the value from the stack since it is stored
			lua_pop(&io_luaState, 1);
		}
		//At this point all the values are stored in o_DataVariable

		return true;
	}
}