// Header Files
//=============

#include "cMeshBuilder.h"
#include "../../Engine/LuaHelper/LuaHelper.h"

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
		delete [] mMeshData.mVertices;
	}

	if (mMeshData.mIndices)
	{
		delete [] mMeshData.mIndices;
	}
}

bool Mesh::cMeshBuilder::Build(const std::vector<const std::string>&)
{
	bool wereThereErrors = false;

	std::string errorMessage;

	if (!LoadMeshFile(m_path_source
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, &errorMessage
#endif
		))
	{
		wereThereErrors = true;

		goto OnExit;
	}
	{
		std::ofstream TargetMeshFile;
		{
			TargetMeshFile.open(m_path_target, std::ios::out | std::ios::binary);

			//First Write, Vertex Count and Index Count
			char * DataOffsetToWriteToFile = reinterpret_cast<char *>(&mMeshData);
			unsigned int Datasize = sizeof(DWORD32) + sizeof(DWORD32); //Vertices and Indices count
			TargetMeshFile.write(DataOffsetToWriteToFile, Datasize);

			//Second Write, Vertices
			DataOffsetToWriteToFile = reinterpret_cast<char *>(mMeshData.mVertices);
			Datasize = sizeof(Engine::sVertexData) * mMeshData.VertexCount;
			TargetMeshFile.write(DataOffsetToWriteToFile, Datasize);

			//Third Write, Indices
			DataOffsetToWriteToFile = reinterpret_cast<char *>(mMeshData.mIndices);
			Datasize = sizeof(DWORD32) * mMeshData.IndexCount;
			TargetMeshFile.write(DataOffsetToWriteToFile, Datasize);

			TargetMeshFile.close();
		}
	}

OnExit:

	if (wereThereErrors)
	{
		BuilderHelper::OutputErrorMessage(errorMessage.c_str());
	}

	return !wereThereErrors;
}

bool Mesh::cMeshBuilder::LoadMeshFile(const char* i_MeshFilepath
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
	, std::string* o_errorMessage
#endif
	)
{
	//Lua Logic
	//---------
	assert(i_MeshFilepath);
	bool wereThereErrors = false;

	lua_State *luaState = NULL;
	{
		if (!LuaHelper::Load_LuaFileAndTopTable(i_MeshFilepath, luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			wereThereErrors = true;
			goto OnExit;
		}
	}
	
	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	if (!LoadTableValues(*luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, o_errorMessage
#endif
		))
	{
		wereThereErrors = true;
	}

	// Pop the table
	lua_pop(luaState, 1);

OnExit:

	if (!wereThereErrors)
	{
		if (!LuaHelper::UnLoad_LuaFile(luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			wereThereErrors = true;
		}
	}

	return !wereThereErrors;
}


bool Mesh::cMeshBuilder::LoadTableValues(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
	, std::string* o_errorMessage
#endif
	)
{
	if (!LoadDataCount(io_luaState, "VertexCount", mMeshData.VertexCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, o_errorMessage
#endif
		))
	{
		return false;
	}

	if (!LoadDataCount(io_luaState, "IndexCount", mMeshData.IndexCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, o_errorMessage
#endif
		))
	{
		return false;
	}

	if (!LoadVertexDataTable(io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, o_errorMessage
#endif
		))
	{
		return false;
	}

	if (!LoadIndexDataTable(io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, o_errorMessage
#endif
		))
	{
		return false;
	}

	return true;
}

bool Mesh::cMeshBuilder::LoadDataCount(lua_State& io_luaState, const char* DataCountKey, DWORD32 & o_DataCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
	, std::string* o_errorMessage
#endif
	)
{
	assert(DataCountKey);
	bool WereThereErrors = false;

	if (!LuaHelper::GetNumberValueFromKey<DWORD32>(io_luaState, DataCountKey, o_DataCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, o_errorMessage
#endif
		))
	{

		WereThereErrors = true;
		goto OnExit;
	}

	if (o_DataCount == 0) //Check if vertex or index count is always greater than zero
	{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		if (o_errorMessage)
		{
			std::stringstream errorMessage;
			errorMessage << "The value for DataCount \"" << DataCountKey << "\" must be a non-zero value\n";

			*o_errorMessage = errorMessage.str().c_str();
		}
#endif

		WereThereErrors = true;
	}

OnExit:

	return !WereThereErrors;
}

bool Mesh::cMeshBuilder::LoadVertexDataTable(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
	, std::string* o_errorMessage
#endif
	)
{
	bool wereThereErrors = false;

	if (!LuaHelper::Load_LuaTable(io_luaState, "Vertices"
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, o_errorMessage
#endif
		))
	{
		wereThereErrors = true;
		goto OnExit;
	}

	if (lua_istable(&io_luaState, -1))
	{
		if (!LoadEachVertexDataTable(io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			wereThereErrors = true;
			goto OnExit;
		}
	}
	else
	{
		wereThereErrors = true;
		goto OnExit;
	}

OnExit:

	// Pop the table
	LuaHelper::UnLoad_LuaTable(io_luaState);

	return !wereThereErrors;
}

bool Mesh::cMeshBuilder::LoadEachVertexDataTable(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
	, std::string* o_errorMessage
#endif
	)
{
	unsigned int VertexIndex = 0;
	mMeshData.mVertices = new Engine::sVertexData[mMeshData.VertexCount];

	bool wereThereErrors = false;

	//Iterating through the constant key value pairs
	lua_pushnil(&io_luaState);
	int CurrentIndexOfVertexTable = -2;
	while (lua_next(&io_luaState, CurrentIndexOfVertexTable))
	{
		if (VertexIndex > mMeshData.VertexCount)
		{
			wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				std::stringstream errormessage;
				errormessage << "VertexIndex: " << VertexIndex << " is greater than max vertex count: " << mMeshData.VertexCount << "\n";
				*o_errorMessage = errormessage.str();
			}
#endif
			// Pop the returned key value pair on error
			lua_pop(&io_luaState, 2);
			goto OnExit;
		}

		//Current Table is at -3 inside the while loop, -2 is the number of empty table name
		int IndexOfValue = -1;	//Is the value inside which is a table

		if (!lua_istable(&io_luaState, IndexOfValue))
		{
			wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "value of each vertex data must be a table (instead of a " <<
					luaL_typename(&io_luaState, IndexOfValue) << ")\n";

				*o_errorMessage = errorMessage.str();
			}
#endif
			// Pop the returned key value pair on error
			lua_pop(&io_luaState, 2);
			goto OnExit;
		}

		if (!LoadEachVertexData(io_luaState, mMeshData.mVertices[VertexIndex]
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
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
		delete [] mMeshData.mVertices;
		mMeshData.mVertices = NULL;
	}

	return !wereThereErrors;
}


bool Mesh::cMeshBuilder::LoadEachVertexData(lua_State& io_luaState, Engine::sVertexData& o_VertexData
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
	, std::string* o_errorMessage
#endif
	)
{

	bool wereThereErrors = false;

	//Iterating through the constant key value pairs
	lua_pushnil(&io_luaState);
	int CurrentIndexOfConstantTable = -2;
	while (lua_next(&io_luaState, CurrentIndexOfConstantTable))
	{
		//Current Table is at -3 inside the while loop
		int IndexOfKey = -2; int IndexOfValue = -1;
		if (lua_type(&io_luaState, IndexOfKey) != LUA_TSTRING)
		{
			wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "key must be a string (instead of a " <<
					luaL_typename(&io_luaState, IndexOfKey) << ")\n";

				*o_errorMessage = errorMessage.str();
			}
#endif
			// Pop the returned key value pair on error
			lua_pop(&io_luaState, 2);
			goto OnExit;
		}

		//Store the valid key in a variable
		const char * VertexDataTableName = lua_tostring(&io_luaState, IndexOfKey);


		if (!lua_istable(&io_luaState, IndexOfValue))
		{
			wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "value of each vertex data must be a table (instead of a " <<
					luaL_typename(&io_luaState, IndexOfValue) << ")\n";

				*o_errorMessage = errorMessage.str();
			}
#endif
			// Pop the returned key value pair on error
			lua_pop(&io_luaState, 2);
			goto OnExit;
		}

		if (strcmp(VertexDataTableName, "Position") == 0)
		{
			const unsigned int PositionCountPerVertex = 3;
			float Position[PositionCountPerVertex];
			if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, Position, PositionCountPerVertex
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif
				))
			{
				wereThereErrors = true;
				// Pop the returned key value pair on error
				lua_pop(&io_luaState, 2);
				goto OnExit;
			}

			o_VertexData.x = Position[0];
			o_VertexData.y = Position[1];
			o_VertexData.z = Position[2];
		}
		else if (strcmp(VertexDataTableName, "Normal") == 0)
		{
			const unsigned int NormalCountPerVertex = 3;
			float Normal[NormalCountPerVertex];
			if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, Normal, NormalCountPerVertex
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif
				))
			{
				wereThereErrors = true;
				// Pop the returned key value pair on error
				lua_pop(&io_luaState, 2);
				goto OnExit;
			}

			o_VertexData.nx = Normal[0];
			o_VertexData.ny = Normal[1];
			o_VertexData.nz = Normal[2];
		}
		else if (strcmp(VertexDataTableName, "Color") == 0)
		{
			const unsigned int PositionCountPerVertex = 3;
			unsigned char Color[PositionCountPerVertex];
			if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<unsigned char>(io_luaState, Color, PositionCountPerVertex
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif
				))
			{
				wereThereErrors = true;
				// Pop the returned key value pair on error
				lua_pop(&io_luaState, 2);
				goto OnExit;
			}

			o_VertexData.color = D3DCOLOR_XRGB(Color[0], Color[1], Color[2]);
		}
		else if (strcmp(VertexDataTableName, "UV") == 0)
		{
			const unsigned int PositionCountPerVertex = 2;
			float UV[PositionCountPerVertex];
			if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, UV, PositionCountPerVertex
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif
				))
			{
				wereThereErrors = true;
				// Pop the returned key value pair on error
				lua_pop(&io_luaState, 2);
				goto OnExit;
			}

			o_VertexData.U = UV[0];
			o_VertexData.V = UV[1];
		}

		//Pop the value, but leave the key
		lua_pop(&io_luaState, 1);
	}


OnExit:

	return !wereThereErrors;
}


bool Mesh::cMeshBuilder::LoadIndexDataTable(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
	, std::string* o_errorMessage
#endif
	)
{
	bool wereThereErrors = false;

	if (!LuaHelper::Load_LuaTable(io_luaState, "Indices"
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, o_errorMessage
#endif
		))
	{
		wereThereErrors = true;
		goto OnExit;
	}

	if (lua_istable(&io_luaState, -1))
	{
		mMeshData.mIndices = new unsigned int[mMeshData.IndexCount];
		if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<unsigned int>(io_luaState, mMeshData.mIndices, mMeshData.IndexCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			wereThereErrors = true;
			goto OnExit;
		}
	}
	else
	{
		wereThereErrors = true;
		goto OnExit;
	}

OnExit:

	// Pop the table
	LuaHelper::UnLoad_LuaTable(io_luaState);
	return !wereThereErrors;
}