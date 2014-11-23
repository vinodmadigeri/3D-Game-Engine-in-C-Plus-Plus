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
		bool LoadMeshFile(const char* i_MeshFilepath
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool LoadTableValues(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool LoadDataCount(lua_State& io_luaState, const char* DataCountKey, DWORD32 & o_DataCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool LoadVertexDataTable(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool LoadEachVertexDataTable(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool LoadEachVertexData(lua_State& io_luaState, Engine::sVertexData& o_VertexData
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool LoadIndexDataTable(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);
	};
}

#endif	// __CMESHBUILDER_H
