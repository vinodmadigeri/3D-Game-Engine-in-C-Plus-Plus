// Header Files
//=============

#include "cMeshBuilder.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <fstream>

// Interface
//==========

// Build
//------

Mesh::cMeshBuilder::~cMeshBuilder()
{
	if (mMeshData.mVertices)
	{
		delete mMeshData.mVertices;
	}

	if (mMeshData.mIndices)
	{
		delete mMeshData.mIndices;
	}
}

bool Mesh::cMeshBuilder::Build(const std::vector<const std::string>&)
{
	bool wereThereErrors = false;

	std::string errorMessage;

	if (!LoadMeshFile(m_path_source, &errorMessage))
	{
		wereThereErrors = true;
	}

	std::ofstream TargetMeshFile;
	TargetMeshFile.open(m_path_target, std::ios::out | std::ios::binary);
	
	//First Write, Vertex Count and Index Count
	char * DataOffsetToWriteToFile = reinterpret_cast<char *>(&mMeshData);
	unsigned int Datasize = sizeof(unsigned int) + sizeof(unsigned int); //Vertices and Indices count
	TargetMeshFile.write(DataOffsetToWriteToFile, Datasize);

	//Second Write, Vertices
	DataOffsetToWriteToFile = reinterpret_cast<char *>(mMeshData.mVertices);
	Datasize = sizeof(Engine::VertexData) * mMeshData.VertexCount;
	TargetMeshFile.write(DataOffsetToWriteToFile, Datasize);

	//Third Write, Indices
	DataOffsetToWriteToFile = reinterpret_cast<char *>(mMeshData.mIndices);
	Datasize = sizeof(unsigned int) * mMeshData.IndexCount;
	TargetMeshFile.write(DataOffsetToWriteToFile, Datasize);

	TargetMeshFile.close();
	return !wereThereErrors;
}

bool Mesh::cMeshBuilder::LoadMeshFile(const char* i_MeshFilepath, std::string* o_errorMessage)
{
	//Lua Logic
	//---------
	assert(i_MeshFilepath);
	bool wereThereErrors = false;

	// Create a new Lua state
	lua_State* luaState = NULL;
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			wereThereErrors = true;
			if (o_errorMessage)
			{
				*o_errorMessage = "Failed to create a new Lua state\n";
			}

			goto OnExit;
		}
	}

	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
		{
			const int luaResult = luaL_loadfile(luaState, i_MeshFilepath);
			if (luaResult != LUA_OK)
			{
				wereThereErrors = true;

				if (o_errorMessage)
				{
					*o_errorMessage = lua_tostring(luaState, -1);
				}

				// Pop the error message
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
		{
			const int argumentCount = 0;
			const int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
			const int noMessageHandler = 0;
			const int luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
			if (luaResult == LUA_OK)
			{
				// A well-behaved asset file will only return a single value
				const int returnedValueCount = lua_gettop(luaState);
				if (returnedValueCount == 1)
				{
					// A correct asset file _must_ return a table
					if (!lua_istable(luaState, -1))
					{
						wereThereErrors = true;

						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "Asset files must return a table (instead of a "
								<< luaL_typename(luaState, -1) << ")\n";

							*o_errorMessage = errorMessage.str();
						}

						// Pop the returned non-table value
						lua_pop(luaState, 1);
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;

					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Asset files must return a single table (instead of " <<
							returnedValueCount << " values)\n";

						*o_errorMessage = errorMessage.str();
					}

					// Pop every value that was returned
					lua_pop(luaState, returnedValueCount);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;

				if (o_errorMessage)
				{
					*o_errorMessage = lua_tostring(luaState, -1);
				}

				// Pop the error message
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	if (!LoadTableValues(*luaState, o_errorMessage))
	{
		wereThereErrors = true;
	}

	// Pop the table
	lua_pop(luaState, 1);

OnExit:

	if (luaState)
	{
		// If I haven't made any mistakes
		// there shouldn't be anything on the stack,
		// regardless of any errors encountered while loading the file:
		assert(lua_gettop(luaState) == 0);

		lua_close(luaState);
		luaState = NULL;
	}

	return !wereThereErrors;
}


bool Mesh::cMeshBuilder::LoadTableValues(lua_State& io_luaState, std::string* o_errorMessage)
{
	if (!LoadDataCount(io_luaState, "VertexCount", mMeshData.VertexCount, o_errorMessage))
	{
		return false;
	}

	if (!LoadDataCount(io_luaState, "IndexCount", mMeshData.IndexCount, o_errorMessage))
	{
		return false;
	}

	if (!LoadVertexDataTable(io_luaState, "Vertices", o_errorMessage))
	{
		return false;
	}

	if (!LoadIndexDataTable(io_luaState, "Indices", o_errorMessage))
	{
		return false;
	}

	return true;
}

bool Mesh::cMeshBuilder::LoadDataCount(lua_State& io_luaState, const char* DataCountKey, unsigned int & o_DataCount, std::string* o_errorMessage)
{
	// Get the value of "name"
	{
		// Remember in Lua you access a table's value like this:
		//	v = t[k]
		// where t is the table, k is the key, and v is the value

		// In C/C++ code you access a table's value like this pseudocode:
		//	lua_push( lua_State*, k );
		//	lua_gettable( lua_State*, t );
		//	v = lua_tovalue( lua_State*, -1 );
		// where t is the _index_ of the table.
		// In words, you:
		//	* Push a table on the stack
		//	* Push the key on the top of the stack
		//	* Call lua_gettable(), providing the index of the table
		//		(the index of the key is required to be -1)
		//	* lua_gettable() will pop the key, and push the value.
		//		In other words, everything on the stack will be as it was before the call
		//		except the key will have been replaced with a value
		//		(which will be nil if you asked for a key that doesn't exist in the table).

		// Let's do it for real, and be clear at every step what the stack looks like:

		// Right now, the stack only has _one_ object,
		// which is the table from the asset file, at index -1.
		// We want to look up the value associated with the key "name":
		const char* key = DataCountKey;
		lua_pushstring(&io_luaState, key);

		// Now, the key is at index -1
		// and the table is at index -2.
		// (Does that make sense why the table is at -2?
		// I think of it like "the table was on top,
		// but when I pushed something new on the top it got pushed down".)

		// We can now call lua_gettable() to get the value associated
		// with the key at index -1:
		{
			const int currentIndexOfTable = -2;
			lua_gettable(&io_luaState, currentIndexOfTable);
		}

		// Now, the value that was at t[k] is at index -1
		// and the table is still at index -2.

		// The following situations are possible:
		//	* There _was_ a value at the key
		//		* It may be the value type that we expect (a string, in this example)
		//		* It may be a different value type (like a boolean, in our current example)
		//	* There was no value at the key

		// It is up to you how explicit you want to make your error checking.
		// In general, writing more error checking code is annoying to do
		// and makes the code harder to read,
		// but it makes life easier for the users of your software
		// (and for you, in cases where something that "should never happen" does.

		// In this first example we will do as much error checking as we can,
		// and then in the next example we will not do any.
		// This will allow you to see both and decide how much you personally prefer.

		// First, then, we will make sure that a value (_any_ value) existed for the key:
		if (lua_isnil(&io_luaState, -1))
		{
			std::cerr << "No value for \"" << key << "\" was found in the asset table\n";
			// When using Lua in C/C++ it is important
			// to always return the stack to its original state.
			// Leaving objects on the stack is kind of like a Lua memory leak.
			// Remember that our value is currently at -1 and the table is at -2;
			// In order to restore it to the way it was when we entered this function
			// we need to pop the value off the stack:
			lua_pop(&io_luaState, 1);
			// Now the only thing on the stack is the asset table at -1,
			// and the calling function will deal with this
			// (regardless of whether this function succeeds or fails).
			return false;
		}

		// If we reach this code then we know that a value exists,
		// but we don't know if it's the right type or not.
		// One way we could find out in the current example is this:
		//	if ( lua_isstring( &io_luaState, -1 ) )
		// This would work (and is perfectly valid),
		// but it actually returns true if the value is a number
		// (because numbers are always convertible to strings).
		// If we really want to be strict, we can do the following instead:
		if (lua_type(&io_luaState, -1) != LUA_TNUMBER)
		{
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "The value for \"" << key << "\" must be a float value "
					"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";

				*o_errorMessage = errorMessage.str().c_str();
			}

			// Pop the value
			lua_pop(&io_luaState, 1);
			// (The asset table is now at -1)
			return false;
		}

		// Now we know that the value stored in the table at the key is valid:
			{
				unsigned int value = static_cast<unsigned int>(lua_tonumber(&io_luaState, -1));
				// You can now do whatever you want with the value.
				// NOTE!
				// which means that Lua owns the memory.
				// You should do whatever you need with the value
				// _before_ popping the value from the stack.
				o_DataCount = value;

				if (o_DataCount == 0) //Check if vertex or index count is always greater than zero
				{
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "The value for DataCount\"" << key << "\" must be a non-zero value\n";

						*o_errorMessage = errorMessage.str().c_str();
					}

					return false;
				}

				// Pop the value
				lua_pop(&io_luaState, 1);
				// (The asset table is now at -1)
			}
	}

	return true;
}

bool Mesh::cMeshBuilder::LoadVertexDataTable(lua_State& io_luaState, const char* RootConstantTableName, std::string* o_errorMessage)
{
	assert(RootConstantTableName);
	bool wereThereErrors = false;

	// Right now the mesh table is at -1.
	// After the following table operation it will be at -2
	// and the "vertex" table will be at -1:
	lua_pushstring(&io_luaState, RootConstantTableName);
	int CurrentIndexOfVertexTable = -2;
	lua_gettable(&io_luaState, CurrentIndexOfVertexTable);
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
	if (lua_istable(&io_luaState, -1))
	{
		if (!LoadEachVertexDataTable(io_luaState, o_errorMessage))
		{
			wereThereErrors = true;
			goto OnExit;
		}
	}
	else
	{
		//No constant table in material file.
	}

OnExit:

	// Pop the table
	lua_pop(&io_luaState, 1);

	return !wereThereErrors;
}

bool Mesh::cMeshBuilder::LoadEachVertexDataTable(lua_State& io_luaState, std::string* o_errorMessage)
{
	unsigned int VertexIndex = 0;
	mMeshData.mVertices = new Engine::VertexData[mMeshData.VertexCount];

	bool wereThereErrors = false;

	//Iterating through the constant key value pairs
	lua_pushnil(&io_luaState);
	int CurrentIndexOfVertexTable = -2;
	while (lua_next(&io_luaState, CurrentIndexOfVertexTable))
	{
		//Current Table is at -3 inside the while loop, -2 is the number of empty table name
		int IndexOfValue = -1;	//Is the value inside which is a table

		if (!lua_istable(&io_luaState, IndexOfValue))
		{
			wereThereErrors = true;
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "value of each vertex data must be a table (instead of a " <<
					luaL_typename(&io_luaState, IndexOfValue) << ")\n";

				*o_errorMessage = errorMessage.str();
			}

			// Pop the returned key value pair on error
			lua_pop(&io_luaState, 2);
			goto OnExit;
		}

		if (!LoadEachVertexData(io_luaState, mMeshData.mVertices[VertexIndex], o_errorMessage))
		{
			wereThereErrors = true;
			// Pop the returned key value pair on error
			lua_pop(&io_luaState, 2);
			goto OnExit;
		}

		//Pop the value, but leave the key
		lua_pop(&io_luaState, 1);
		VertexIndex++;
	}


OnExit:

	if (wereThereErrors)
	{
		delete mMeshData.mVertices;
	}

	return !wereThereErrors;
}


bool Mesh::cMeshBuilder::LoadEachVertexData(lua_State& io_luaState, Engine::VertexData& o_VertexData, std::string* o_errorMessage)
{

	bool wereThereErrors = false;

	//Iterating through the constant key value pairs
	lua_pushnil(&io_luaState);
	int CurrentIndexOfConstantTable = -2;
	while (lua_next(&io_luaState, CurrentIndexOfConstantTable))
	{
		//Current Table is at -3 inside the while loop
		//Current Table is at -3 inside the while loop
		int IndexOfKey = -2; int IndexOfValue = -1;
		if (lua_type(&io_luaState, IndexOfKey) != LUA_TSTRING)
		{
			wereThereErrors = true;

			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "key must be a string (instead of a " <<
					luaL_typename(&io_luaState, IndexOfKey) << ")\n";

				*o_errorMessage = errorMessage.str();
			}

			// Pop the returned key value pair on error
			lua_pop(&io_luaState, 2);
			goto OnExit;
		}

		//Store the valid key in a variable
		const char * VertexDataTableName = lua_tostring(&io_luaState, IndexOfKey);


		if (!lua_istable(&io_luaState, IndexOfValue))
		{
			wereThereErrors = true;
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "value of each vertex data must be a table (instead of a " <<
					luaL_typename(&io_luaState, IndexOfValue) << ")\n";

				*o_errorMessage = errorMessage.str();
			}

			// Pop the returned key value pair on error
			lua_pop(&io_luaState, 2);
			goto OnExit;
		}

		if (strcmp(VertexDataTableName, "Position") == 0)
		{
			unsigned int PositionCountPerVertex = 3;
			if (!LoadEachFloatDataValues(io_luaState, (o_VertexData.Position), PositionCountPerVertex, o_errorMessage))
			{
				wereThereErrors = true;
				// Pop the returned key value pair on error
				lua_pop(&io_luaState, 2);
				goto OnExit;
			}
		}
		else if (strcmp(VertexDataTableName, "Color") == 0)
		{
			unsigned int PositionCountPerVertex = 3;
			if (!LoadEachUCHARDataValues(io_luaState, (o_VertexData.Color), PositionCountPerVertex, o_errorMessage))
			{
				wereThereErrors = true;
				// Pop the returned key value pair on error
				lua_pop(&io_luaState, 2);
				goto OnExit;
			}
		}

		//Pop the value, but leave the key
		lua_pop(&io_luaState, 1);
	}


OnExit:

	return !wereThereErrors;
}


bool Mesh::cMeshBuilder::LoadEachFloatDataValues(lua_State& io_luaState, float * o_DataVariable, const unsigned int i_DataCount, std::string* o_errorMessage)
{
	assert(o_DataVariable);
	//Iterating through every value table
	const int DataCount = luaL_len(&io_luaState, -1);
	assert(DataCount == i_DataCount);

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

		o_DataVariable[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
		float Val = static_cast<float>(lua_tonumber(&io_luaState, -1));
		//Pop the value from the stack since it is stored
		lua_pop(&io_luaState, 1);
	}
	//At this point all the values are stored in o_DataVariable

	return true;
}


bool Mesh::cMeshBuilder::LoadEachUCHARDataValues(lua_State& io_luaState, unsigned char * o_DataVariable, const unsigned int i_DataCount, std::string* o_errorMessage)
{
	assert(o_DataVariable);
	//Iterating through every value table
	const int DataCount = luaL_len(&io_luaState, -1);
	assert(DataCount == i_DataCount);

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

		o_DataVariable[i - 1] = static_cast<unsigned char>(lua_tointeger(&io_luaState, -1));
		float Val = static_cast<float>(lua_tonumber(&io_luaState, -1));
		//Pop the value from the stack since it is stored
		lua_pop(&io_luaState, 1);
	}
	//At this point all the values are stored in o_DataVariable

	return true;
}


bool Mesh::cMeshBuilder::LoadEachUINTDataValues(lua_State& io_luaState, unsigned int * o_DataVariable, const unsigned int i_DataCount, std::string* o_errorMessage)
{
	assert(o_DataVariable);
	//Iterating through every value table
	const int DataCount = luaL_len(&io_luaState, -1);
	assert(DataCount == i_DataCount);

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

		o_DataVariable[i - 1] = static_cast<unsigned int>(lua_tointeger(&io_luaState, -1));
		float Val = static_cast<float>(lua_tonumber(&io_luaState, -1));
		//Pop the value from the stack since it is stored
		lua_pop(&io_luaState, 1);
	}
	//At this point all the values are stored in o_DataVariable

	return true;
}


bool Mesh::cMeshBuilder::LoadIndexDataTable(lua_State& io_luaState, const char* RootConstantTableName, std::string* o_errorMessage)
{
	assert(RootConstantTableName);
	bool wereThereErrors = false;

	// Right now the mesh table is at -1.
	// After the following table operation it will be at -2
	// and the "vertex" table will be at -1:
	lua_pushstring(&io_luaState, RootConstantTableName);
	int CurrentIndexOfVertexTable = -2;
	lua_gettable(&io_luaState, CurrentIndexOfVertexTable);
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
	if (lua_istable(&io_luaState, -1))
	{
		mMeshData.mIndices = new unsigned int[mMeshData.IndexCount];

		if (!LoadEachUINTDataValues(io_luaState, mMeshData.mIndices, mMeshData.IndexCount, o_errorMessage))
		{
			wereThereErrors = true;
			goto OnExit;
		}
	}
	else
	{
		//No constant table in material file.
	}

OnExit:

	// Pop the table
	lua_pop(&io_luaState, 1);

	if (wereThereErrors)
	{
		if (mMeshData.mIndices)
		{
			delete mMeshData.mIndices;
		}
	}

	return !wereThereErrors;
}