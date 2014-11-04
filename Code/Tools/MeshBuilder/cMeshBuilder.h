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

#include "../../Engine/MeshData.h"

// Class Declaration
//==================

namespace Mesh
{
	class cMeshBuilder : public BuilderHelper::cbBuilder
	{
		// Interface
		//==========
		Engine::MeshData mMeshData;
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
		bool LoadEachVertexData(lua_State& io_luaState, Engine::VertexData& o_VertexData, std::string* o_errorMessage);
		bool LoadEachFloatDataValues(lua_State& io_luaState, float * o_DataVariable, const unsigned int i_DataCount, std::string* o_errorMessage);
		bool LoadEachUCHARDataValues(lua_State& io_luaState, unsigned char * o_DataVariable, const unsigned int i_DataCount, std::string* o_errorMessage);
		bool LoadEachUINTDataValues(lua_State& io_luaState, unsigned int * o_DataVariable, const unsigned int i_DataCount, std::string* o_errorMessage);
		bool LoadIndexDataTable(lua_State& io_luaState, const char* RootConstantTableName, std::string* o_errorMessage);

	};
}

#endif	// __CMESHBUILDER_H
