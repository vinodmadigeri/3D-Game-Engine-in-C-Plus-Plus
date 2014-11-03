/*
	This is an example builder program that just copies the source file to the target path
	(as opposed to actually "building" it in some way so that the target is different than the source)
*/

#ifndef __CMESHBUILDER_H
#define __CMESHBUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"

#include "../../External/Lua/Includes.h"

// Class Declaration
//==================

namespace Mesh
{
	struct VertexData
	{
		float Position[3];
		float Color[3];
	};

	struct MeshData
	{
		unsigned int VertexCount;
		unsigned int IndexCount;
		VertexData *mVertices;
		float  *mIndices;

		MeshData()
		{
			mVertices = NULL;
			mIndices = NULL;
		}
	};

	class cMeshBuilder : public BuilderHelper::cbBuilder
	{
		// Interface
		//==========
		MeshData mMeshData;
	public:
		~cMeshBuilder();
		// Build
		//------

		virtual bool Build( const std::vector<const std::string>& );
		
		// Lua Logic to load mesh file
		//-----------------------------
		bool LoadMeshFile(const char* i_MeshFilepath, std::string* o_errorMessage);
		bool LoadTableValues(lua_State& io_luaState, std::string* o_errorMessage);
		bool LoadDataCount(lua_State& io_luaState, const char* DataCountKey, unsigned int & o_DataCount, std::string* o_errorMessage);
		bool LoadVertexDataTable(lua_State& io_luaState, const char* RootConstantTableName, std::string* o_errorMessage);
		bool LoadEachVertexDataTable(lua_State& io_luaState, std::string* o_errorMessage);
		bool LoadEachVertexData(lua_State& io_luaState, VertexData& o_VertexData, std::string* o_errorMessage);
		bool LoadEachFloatDataValues(lua_State& io_luaState, float * o_DataVariable, const unsigned int i_DataCount, std::string* o_errorMessage);
		bool LoadIndexDataTable(lua_State& io_luaState, const char* RootConstantTableName, std::string* o_errorMessage);

	};
}

#endif	// __CMESHBUILDER_H
