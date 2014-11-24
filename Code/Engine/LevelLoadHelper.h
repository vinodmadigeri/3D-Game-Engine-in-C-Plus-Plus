#ifndef __LEVEL_LOAD_HELPER_H
#define __LEVEL_LOAD_HELPER_H


#include "../LuaHelper/LuaHelper.h"

namespace Engine
{
	struct LightingData;

	bool LoadLevel(const char * pcLevelName);
	
	bool LoadClassTypes(lua_State &io_luaState, std::vector<std::string> & o_classTypes
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		);
	
	bool LoadLightingData(lua_State &io_luaState, std::vector<LightingData> & o_LightingDatas
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		);

	bool CreateLightingInstance(std::vector<LightingData> &LightingDatas
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		);

	bool LoadActorsData(lua_State &io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		);

	bool LoadAndCreateEachActorData(lua_State &io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		);

	bool LoadRenderSettings(lua_State &io_luaState, std::string & o_MeshPath, std::string & o_MaterialPath, bool & o_IsRenderable
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		);

	bool LoadPhysicsSettings(lua_State &io_luaState, std::vector<std::string> &o_CollidesWith, bool & o_IsCollidable
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		);

}

#endif //__LEVEL_LOAD_HELPER_H