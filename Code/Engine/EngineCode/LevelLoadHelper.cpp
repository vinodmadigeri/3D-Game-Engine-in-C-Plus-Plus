#include "PreCompiled.h"
#include "LevelLoadHelper.h"

#include "LightingData.h"
#include "LightingSystem.h"

#include "WorldSystem.h"
#include "Actor.h"
#include "PhysicsSystem.h"
#include "RenderableObjectSystem.h"
#include "CollisionSystem.h"
#include "CameraSystem.h"

namespace Engine
{

	//******************************************************************************
	bool LoadLevel(const char * pcLevelName)
	{
		assert(pcLevelName);

		std::vector<std::string> ClassTypes;
		std::vector<LightingData> LightingDatas;


		bool WereThereErrors = false;
		std::string errorMessage;

		lua_State *luaState = NULL;
		{

			if (!LuaHelper::Load_LuaFileAndTopTable(pcLevelName, luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, &errorMessage
#endif
				))
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				MessageBox(NULL, errorMessage.c_str(), pcLevelName, MB_OK | MB_ICONERROR);
#endif
				return false;
			}
		}

		if (!LoadClassTypes(*luaState, ClassTypes
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, &errorMessage
#endif
			))
		{
			WereThereErrors = true;
			goto OnExit;
		}

		//Add class types in global actor class types
		for (unsigned int i = 0; i < ClassTypes.size(); i++)
		{
			Actor::AddGlobalClassTypes(ClassTypes[i].c_str());
		}
		
		if (!LoadLightingData(*luaState, LightingDatas
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, &errorMessage
#endif
			))
		{
			WereThereErrors = true;
			goto OnExit;
		}

		if (!CreateLightingInstance(LightingDatas
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, &errorMessage
#endif
			))
		{
			WereThereErrors = true;
			goto OnExit;
		}

		if (!LoadCameraDataAndCreate(*luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, &errorMessage
#endif
			))
		{
			WereThereErrors = true;
			goto OnExit;
		}

		if (!LoadActorsData(*luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, &errorMessage
#endif
			))
		{
			WereThereErrors = true;
		}

	OnExit:

		// Pop the root table
		LuaHelper::UnLoad_LuaTable(*luaState);

		if (WereThereErrors)
		{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			MessageBox(NULL, errorMessage.c_str(), pcLevelName, MB_OK | MB_ICONERROR);
#endif
		}
		else
		{
			WereThereErrors = !LuaHelper::UnLoad_LuaFile(luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, &errorMessage
#endif
				);
		}
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		if (WereThereErrors)
		{
			MessageBox(NULL, errorMessage.c_str(), pcLevelName, MB_OK | MB_ICONERROR);
		}
#endif
		return !WereThereErrors;
	}

	//******************************************************************************
	bool CreateLightingInstance(std::vector<LightingData> &LightingDatas
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		//Instantiate light with data
		Vector3 *iAmbientLight = NULL;
		Vector3 *iDiffuseLight = NULL;
		Vector3 *iLightDirection = NULL;
		for (unsigned int i = 0; i < LightingDatas.size(); i++)
		{
			if (LightingDatas[i].mLightingDataName == "AmbientLight")
			{
				iAmbientLight = &LightingDatas[i].mDataValue;
			}

			if (LightingDatas[i].mLightingDataName == "DiffuseLight")
			{
				iDiffuseLight = &LightingDatas[i].mDataValue;
			}

			if (LightingDatas[i].mLightingDataName == "LightDirection")
			{
				iLightDirection = &LightingDatas[i].mDataValue;
			}
		}

		assert(iLightDirection && iDiffuseLight && iLightDirection);

		if (!LightingSystem::CreateInstance(*iAmbientLight, *iDiffuseLight, *iLightDirection))
		{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				*o_errorMessage = "Could not create Lighting system instance";
			}
#endif
			return false;
		}

		return true;
	}

	//******************************************************************************
	bool LoadClassTypes(lua_State &io_luaState, std::vector<std::string> & o_classTypes
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		bool WereThereErrors = false;
		if (!LuaHelper::Load_LuaTable(io_luaState, "ClassTypes"
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			WereThereErrors = true;
			goto OnExit;
		}

		int ClassCount = LuaHelper::GetTableValuesLength(io_luaState);

		o_classTypes.reserve(ClassCount);

		if (!LuaHelper::GetEachStringValuesInCurrentTable(io_luaState, o_classTypes, ClassCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			WereThereErrors = true;
			goto OnExit;
		}

	OnExit:

		LuaHelper::UnLoad_LuaTable(io_luaState);
		return !WereThereErrors;
	}

	//******************************************************************************
	bool LoadLightingData(lua_State &io_luaState, std::vector<LightingData> & o_LightingDatas
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		bool wereThereErrors = false;
		if (!LuaHelper::Load_LuaTable(io_luaState, "LightingData"
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			wereThereErrors = true;
			goto OnExit;
		}

		o_LightingDatas.reserve(LuaHelper::GetTableValuesLength(io_luaState));

		//Iterating through the lightingdata key value pairs
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
			const char * LightingDataTableName = lua_tostring(&io_luaState, IndexOfKey);

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

			const unsigned int DataCountPerLightingVar = 3;
			float EachLightingData[DataCountPerLightingVar];
			if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, EachLightingData, DataCountPerLightingVar
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

			o_LightingDatas.push_back(LightingData(LightingDataTableName, EachLightingData[0], EachLightingData[1], EachLightingData[2]));

			//Pop the value, but leave the key
			lua_pop(&io_luaState, 1);
		}


	OnExit:

		LuaHelper::UnLoad_LuaTable(io_luaState);
		return !wereThereErrors;

	}


	//******************************************************************************
	bool LoadCameraDataAndCreate(lua_State &io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		bool wereThereErrors = false;
		if (!LuaHelper::Load_LuaTable(io_luaState, "CameraData"
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			wereThereErrors = true;
			goto OnExit;
		}
		
		{
			//Default value for camera settings
			float InitialVelocity[3] = { 0.0f, 0.0f, 0.0f };
			float InitialAccln[3] = { 0.0f, 0.0f, 0.0f };
			float Size[3] = { 0.4f, 0.4f, 0.4f };
			float Rotation = 0.0f;

			float eyeLocation[3] = { 0.0f, 0.0f, -11.0f };
			float lookAtLocation[3] = { 0.0f, 0.0f, 1.0f };
			float up[3] = { 0.0f, 1.0f, 0.0f };
			float fieldOfView = 60.0f; //
			float nearPlane = { 0.1f };
			float farPlane = { 100.0f };
			char * iType = "Camera";
			std::vector<std::string> o_CollidesWith;
			bool IsCollidable = false;

			//Iterating through the lightingdata key value pairs
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
				const char * CameraDataTableName = lua_tostring(&io_luaState, IndexOfKey);


				//------------------Velocity-------------------------
				if ((strcmp(CameraDataTableName, "velocity") == 0))
				{
					if (!lua_istable(&io_luaState, IndexOfValue))
					{
						//********HANDLE ERRRRROR **************
						wereThereErrors = true;
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "Actor velocity must be a table (instead of a " <<
								luaL_typename(&io_luaState, IndexOfValue) << ")\n";

							*o_errorMessage = errorMessage.str();
						}
		#endif
						// Pop the returned key value pair on error
						lua_pop(&io_luaState, 2);
						goto OnExit;
					}

					const unsigned int VelocityCount = 3;
					if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, InitialVelocity, VelocityCount
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						, o_errorMessage
		#endif
						))
					{
						//********HANDLE ERRRRROR **************
						wereThereErrors = true;
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "Could not read Actor velocity data";
							*o_errorMessage = errorMessage.str();
						}
		#endif
						// Pop the returned key value pair on error
						lua_pop(&io_luaState, 2);
						goto OnExit;
					}
				}

				//------------------Acceleration---------------------------
				if ((strcmp(CameraDataTableName, "acceleration") == 0))
				{
					if (!lua_istable(&io_luaState, IndexOfValue))
					{
						//********HANDLE ERRRRROR **************
						wereThereErrors = true;
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "Actor acceleration must be a table (instead of a " <<
								luaL_typename(&io_luaState, IndexOfValue) << ")\n";

							*o_errorMessage = errorMessage.str();
						}
		#endif
						// Pop the returned key value pair on error
						lua_pop(&io_luaState, 2);
						goto OnExit;
					}

					const unsigned int accelerationCount = 3;
					if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, InitialAccln, accelerationCount
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						, o_errorMessage
		#endif
						))
					{
						//********HANDLE ERRRRROR **************
						wereThereErrors = true;
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "Could not read Actor acceleration data";
							*o_errorMessage = errorMessage.str();
						}
		#endif
						// Pop the returned key value pair on error
						lua_pop(&io_luaState, 2);
						goto OnExit;
					}
				}


				//------------------Size-------------------------
				if ((strcmp(CameraDataTableName, "size") == 0))
				{
					if (!lua_istable(&io_luaState, IndexOfValue))
					{
						//********HANDLE ERRRRROR **************
						wereThereErrors = true;
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "Actor size must be a table (instead of a " <<
								luaL_typename(&io_luaState, IndexOfValue) << ")\n";

							*o_errorMessage = errorMessage.str();
						}
		#endif
						// Pop the returned key value pair on error
						lua_pop(&io_luaState, 2);
						goto OnExit;
					}

					const unsigned int sizeCount = 3;
					if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, Size, sizeCount
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						, o_errorMessage
		#endif
						))
					{
						//********HANDLE ERRRRROR **************
						wereThereErrors = true;
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "Could not read Actor size data";
							*o_errorMessage = errorMessage.str();
						}
		#endif
						// Pop the returned key value pair on error
						lua_pop(&io_luaState, 2);
						goto OnExit;
					}
				}


				//------------------Rotation-------------------------
				if ((strcmp(CameraDataTableName, "rotation") == 0))
				{

					if (lua_type(&io_luaState, IndexOfValue) != LUA_TNUMBER)
					{
						//********HANDLE ERRRRROR **************
						wereThereErrors = true;
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "Name of actor must be a String (instead of a " <<
								luaL_typename(&io_luaState, IndexOfValue) << ")\n";

							*o_errorMessage = errorMessage.str();
						}
		#endif
						// Pop the returned key value pair on error
						lua_pop(&io_luaState, 2);
						goto OnExit;
					}

					Rotation = static_cast<float>(lua_tonumber(&io_luaState, IndexOfValue));
				}


				if (strcmp(CameraDataTableName, "EyeLocation") == 0)
				{
					if (!lua_istable(&io_luaState, IndexOfValue))
					{
						wereThereErrors = true;
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "value of each EyeLocation data must be a table (instead of a " <<
								luaL_typename(&io_luaState, IndexOfValue) << ")\n";

							*o_errorMessage = errorMessage.str();
						}
		#endif
						// Pop the returned key value pair on error
						lua_pop(&io_luaState, 2);
						goto OnExit;
					}


					const unsigned int DataCountPerVar = 3;
					if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, eyeLocation, DataCountPerVar
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
				}

				if (strcmp(CameraDataTableName, "LookAtLocation") == 0)
				{
					if (!lua_istable(&io_luaState, IndexOfValue))
					{
						wereThereErrors = true;
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "value of each LookAtLocation data must be a table (instead of a " <<
								luaL_typename(&io_luaState, IndexOfValue) << ")\n";

							*o_errorMessage = errorMessage.str();
						}
		#endif
						// Pop the returned key value pair on error
						lua_pop(&io_luaState, 2);
						goto OnExit;
					}


					const unsigned int DataCountPerVar = 3;
					if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, lookAtLocation, DataCountPerVar
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
				}

				if (strcmp(CameraDataTableName, "FieldOfView") == 0)
				{
					if (lua_type(&io_luaState, IndexOfValue) != LUA_TNUMBER)
					{
						wereThereErrors = true;
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "value of FieldOfView data must be a number (instead of a " <<
								luaL_typename(&io_luaState, IndexOfValue) << ")\n";

							*o_errorMessage = errorMessage.str();
						}
		#endif
						// Pop the returned key value pair on error
						lua_pop(&io_luaState, 2);
						goto OnExit;
					}


					fieldOfView = static_cast<float>(lua_tonumber(&io_luaState, IndexOfValue));
				}

				if (strcmp(CameraDataTableName, "class") == 0)
				{
					if (lua_type(&io_luaState, IndexOfValue) != LUA_TSTRING)
					{
						wereThereErrors = true;
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "value of FieldOfView data must be a number (instead of a " <<
								luaL_typename(&io_luaState, IndexOfValue) << ")\n";

							*o_errorMessage = errorMessage.str();
						}
		#endif
						// Pop the returned key value pair on error
						lua_pop(&io_luaState, 2);
						goto OnExit;
					}


					iType = const_cast<char *>((lua_tostring(&io_luaState, IndexOfValue)));
				}

				if (strcmp(CameraDataTableName, "Up") == 0)
				{
					if (!lua_istable(&io_luaState, IndexOfValue))
					{
						wereThereErrors = true;
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "value of each Up data must be a table (instead of a " <<
								luaL_typename(&io_luaState, IndexOfValue) << ")\n";

							*o_errorMessage = errorMessage.str();
						}
		#endif
						// Pop the returned key value pair on error
						lua_pop(&io_luaState, 2);
						goto OnExit;
					}


					const unsigned int DataCountPerVar = 3;
					if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, up, DataCountPerVar
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
				}

				if (strcmp(CameraDataTableName, "NearPlane") == 0)
				{
					if (lua_type(&io_luaState, IndexOfValue) != LUA_TNUMBER)
					{
						wereThereErrors = true;
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "value of NearPlane data must be a number (instead of a " <<
								luaL_typename(&io_luaState, IndexOfValue) << ")\n";

							*o_errorMessage = errorMessage.str();
						}
		#endif
						// Pop the returned key value pair on error
						lua_pop(&io_luaState, 2);
						goto OnExit;
					}

					nearPlane = static_cast<float>(lua_tonumber(&io_luaState, IndexOfValue));
				}

				if (strcmp(CameraDataTableName, "FarPlane") == 0)
				{
					if (lua_type(&io_luaState, IndexOfValue) != LUA_TNUMBER)
					{
						wereThereErrors = true;
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "value of FarPlane data must be a number (instead of a " <<
								luaL_typename(&io_luaState, IndexOfValue) << ")\n";

							*o_errorMessage = errorMessage.str();
						}
		#endif
						// Pop the returned key value pair on error
						lua_pop(&io_luaState, 2);
						goto OnExit;
					}

					farPlane = static_cast<float>(lua_tonumber(&io_luaState, IndexOfValue));
				}


				//------------------RenderSettings-------------------------
				if ((strcmp(CameraDataTableName, "collisionSettings") == 0))
				{
					if (!LoadPhysicsSettings(io_luaState, o_CollidesWith, IsCollidable
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
				}

				//Pop the value, but leave the key
				lua_pop(&io_luaState, 1);
			}

			SharedPointer<Actor> NewActor = Actor::Create(Vector3(eyeLocation[0], eyeLocation[1], eyeLocation[2]),
				Vector3(InitialVelocity[0], InitialVelocity[1], InitialVelocity[2]),
				Vector3(InitialAccln[0], InitialAccln[1], InitialAccln[2]),
				"Camera", iType, Vector3(Size[0], Size[1], Size[2]), Rotation, o_CollidesWith);

			assert(NewActor != NULL);

			fieldOfView = fieldOfView * static_cast<float>(Engine::Get_PI_Value() / 180.0f); //60 degrees to radians
			if (!Engine::CameraSystem::CreateInstance(NewActor, UserSettings::GetWidth(), UserSettings::GetHeight(), fieldOfView,
				nearPlane, farPlane, Vector3(lookAtLocation[0], lookAtLocation[1], lookAtLocation[2]),
				Vector3(up[0], up[1], up[2])))
			{
		#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					*o_errorMessage = "Could not create Camera instance";
				}
		#endif
				return false;
			}


			assert(PhysicsSystem::GetInstance());

			PhysicsSystem::GetInstance()->AddActorGameObject(NewActor);

			if (IsCollidable)
			{
				assert(CollisionSystem::GetInstance());
				CollisionSystem::GetInstance()->AddActorGameObject(NewActor);
			}
		}
	OnExit:

		LuaHelper::UnLoad_LuaTable(io_luaState);
		return !wereThereErrors;
	}

	//******************************************************************************
	bool LoadActorsData(lua_State &io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		bool WereThereErrors = false;
		if (!LuaHelper::Load_LuaTable(io_luaState, "Actors"
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			WereThereErrors = true;
			goto OnExit;
		}

		//Iterating through the constant key value pairs
		lua_pushnil(&io_luaState);
		int CurrentIndexOfActorsTable = -2;
		while (lua_next(&io_luaState, CurrentIndexOfActorsTable))
		{
			//Current Table is at -3 inside the while loop, -2 is the number of empty table name
			int IndexOfValue = -1;	//Is the value inside which is a table

			if (!lua_istable(&io_luaState, IndexOfValue))
			{
				WereThereErrors = true;
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

			if (!LoadAndCreateEachActorData(io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif
				))
			{
				WereThereErrors = true;
				// Pop the returned key value pair on error
				lua_pop(&io_luaState, 2);
				goto OnExit;
			}

			//Pop the value, but leave the key
			lua_pop(&io_luaState, 1);
		}


	OnExit:

		LuaHelper::UnLoad_LuaTable(io_luaState);
		return !WereThereErrors;
	}

	//******************************************************************************
	bool LoadAndCreateEachActorData(lua_State &io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		std::string ActorName = "";
		std::string ActorClass = "";
		Vector3 Position = Vector3(0.0f, -1.0f, 0.0f);
		Vector3 Velocity = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 Acceleration = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 Size = Vector3(1.0f, 1.0f, 0.0f);
		bool IsRenderable = false;
		bool IsCollidable = false;

		float Rotation = 0.0f;

		std::string MaterialPath = "data/genericMaterial.mat";
		std::string MeshPath = "data/plane.dat";
		std::vector<std::string> o_CollidesWith;

		bool wereThereErrors = false;

		//Iterating through the actors data key value pairs
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
			const char * EachActorDataName = lua_tostring(&io_luaState, IndexOfKey);

			//------------------Name-------------------------
			if ((strcmp(EachActorDataName, "name") == 0))
			{

				if (lua_type(&io_luaState, IndexOfValue) != LUA_TSTRING)
				{
					//********HANDLE ERRRRROR **************
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Name of actor must be a String (instead of a " <<
							luaL_typename(&io_luaState, IndexOfValue) << ")\n";

						*o_errorMessage = errorMessage.str();
					}
#endif
					// Pop the returned key value pair on error
					lua_pop(&io_luaState, 2);
					goto OnExit;
				}

				ActorName = lua_tostring(&io_luaState, IndexOfValue);
			}

			//------------------Class-------------------------
			if ((strcmp(EachActorDataName, "class") == 0))
			{
				if (lua_type(&io_luaState, IndexOfValue) != LUA_TSTRING)
				{
					//********HANDLE ERRRRROR **************
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Actor class must be a String (instead of a " <<
							luaL_typename(&io_luaState, IndexOfValue) << ")\n";

						*o_errorMessage = errorMessage.str();
					}
#endif
					// Pop the returned key value pair on error
					lua_pop(&io_luaState, 2);
					goto OnExit;
				}

				ActorClass = lua_tostring(&io_luaState, IndexOfValue);
			}

			//------------------Position-------------------------
			if ((strcmp(EachActorDataName, "position") == 0))
			{
				if (!lua_istable(&io_luaState, IndexOfValue))
				{
					//********HANDLE ERRRRROR **************
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Actor Position must be a table (instead of a " <<
							luaL_typename(&io_luaState, IndexOfValue) << ")\n";

						*o_errorMessage = errorMessage.str();
					}
#endif
					// Pop the returned key value pair on error
					lua_pop(&io_luaState, 2);
					goto OnExit;
				}

				const unsigned int PosCount = 3;
				float PositionData[PosCount];
				if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, PositionData, PosCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					//********HANDLE ERRRRROR **************
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Could not read Actor Position data";
						*o_errorMessage = errorMessage.str();
					}
#endif
					// Pop the returned key value pair on error
					lua_pop(&io_luaState, 2);
					goto OnExit;
				}

				Position = Vector3(PositionData[0], PositionData[1], PositionData[2]);
			}

			//------------------Velocity-------------------------
			if ((strcmp(EachActorDataName, "velocity") == 0))
			{
				if (!lua_istable(&io_luaState, IndexOfValue))
				{
					//********HANDLE ERRRRROR **************
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Actor velocity must be a table (instead of a " <<
							luaL_typename(&io_luaState, IndexOfValue) << ")\n";

						*o_errorMessage = errorMessage.str();
					}
#endif
					// Pop the returned key value pair on error
					lua_pop(&io_luaState, 2);
					goto OnExit;
				}

				const unsigned int VelocityCount = 3;
				float VelocityData[VelocityCount];
				if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, VelocityData, VelocityCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					//********HANDLE ERRRRROR **************
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Could not read Actor velocity data";
						*o_errorMessage = errorMessage.str();
					}
#endif
					// Pop the returned key value pair on error
					lua_pop(&io_luaState, 2);
					goto OnExit;
				}

				Velocity = Vector3(VelocityData[0], VelocityData[1], VelocityData[2]);
			}

			//------------------Acceleration---------------------------
			if ((strcmp(EachActorDataName, "acceleration") == 0))
			{
				if (!lua_istable(&io_luaState, IndexOfValue))
				{
					//********HANDLE ERRRRROR **************
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Actor acceleration must be a table (instead of a " <<
							luaL_typename(&io_luaState, IndexOfValue) << ")\n";

						*o_errorMessage = errorMessage.str();
					}
#endif
					// Pop the returned key value pair on error
					lua_pop(&io_luaState, 2);
					goto OnExit;
				}

				const unsigned int accelerationCount = 3;
				float accelerationData[accelerationCount];
				if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, accelerationData, accelerationCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					//********HANDLE ERRRRROR **************
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Could not read Actor acceleration data";
						*o_errorMessage = errorMessage.str();
					}
#endif
					// Pop the returned key value pair on error
					lua_pop(&io_luaState, 2);
					goto OnExit;
				}

				Acceleration = Vector3(accelerationData[0], accelerationData[1], accelerationData[2]);
			}


			//------------------Size-------------------------
			if ((strcmp(EachActorDataName, "size") == 0))
			{
				if (!lua_istable(&io_luaState, IndexOfValue))
				{
					//********HANDLE ERRRRROR **************
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Actor size must be a table (instead of a " <<
							luaL_typename(&io_luaState, IndexOfValue) << ")\n";

						*o_errorMessage = errorMessage.str();
					}
#endif
					// Pop the returned key value pair on error
					lua_pop(&io_luaState, 2);
					goto OnExit;
				}

				const unsigned int sizeCount = 3;
				float sizeData[sizeCount];
				if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, sizeData, sizeCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					//********HANDLE ERRRRROR **************
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Could not read Actor size data";
						*o_errorMessage = errorMessage.str();
					}
#endif
					// Pop the returned key value pair on error
					lua_pop(&io_luaState, 2);
					goto OnExit;
				}

				Size = Vector3(sizeData[0], sizeData[1], sizeData[2]);
			}


			//------------------Rotation-------------------------
			if ((strcmp(EachActorDataName, "rotation") == 0))
			{

				if (lua_type(&io_luaState, IndexOfValue) != LUA_TNUMBER)
				{
					//********HANDLE ERRRRROR **************
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Name of actor must be a String (instead of a " <<
							luaL_typename(&io_luaState, IndexOfValue) << ")\n";

						*o_errorMessage = errorMessage.str();
					}
#endif
					// Pop the returned key value pair on error
					lua_pop(&io_luaState, 2);
					goto OnExit;
				}

				Rotation = static_cast<float>(lua_tonumber(&io_luaState, IndexOfValue));
			}



			//------------------RenderSettings-------------------------
			if ((strcmp(EachActorDataName, "renderSettings") == 0))
			{
				if (!LoadRenderSettings(io_luaState, MeshPath, MaterialPath, IsRenderable
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
			}

			//------------------RenderSettings-------------------------
			if ((strcmp(EachActorDataName, "collisionSettings") == 0))
			{
				if (!LoadPhysicsSettings(io_luaState, o_CollidesWith, IsCollidable
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
			}

			//Pop the value, but leave the key
			lua_pop(&io_luaState, 1);

		}

		//Create new actor and add them in different systems
		{
			//If the execution is here that means, all the required values were read and no error was found
			assert(WorldSystem::GetInstance());
			assert(RenderableObjectSystem::GetInstance());
			assert(PhysicsSystem::GetInstance());
			//-----------------------------Actor Creation and adding to systems-----------------------------
			SharedPointer<Actor> NewActor = Actor::Create(Position, Velocity, Acceleration, ActorName.c_str(), ActorClass.c_str(), Size, Rotation, o_CollidesWith);
			WorldSystem::GetInstance()->AddActorGameObject(NewActor);

			PhysicsSystem::GetInstance()->AddActorGameObject(NewActor);

			if (IsRenderable)
			{
				RenderableObjectSystem::GetInstance()->Add3DActorGameObject(NewActor, MaterialPath.c_str(), MeshPath.c_str());
			}

			if (IsCollidable)
			{
				CollisionSystem::GetInstance()->AddActorGameObject(NewActor);
			}
		}
	OnExit:

		return !wereThereErrors;
	}

	bool LoadRenderSettings(lua_State &io_luaState, std::string & o_MeshPath, std::string & o_MaterialPath, bool & o_IsRenderable
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		bool WereThereErrors = false;

		if (!LuaHelper::GetStringValueFromKey(io_luaState, "meshPath", o_MeshPath
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			WereThereErrors = true;
			goto OnExit;
		}

		if (!LuaHelper::GetStringValueFromKey(io_luaState, "materialPath", o_MaterialPath
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			WereThereErrors = true;
			goto OnExit;
		}

		o_IsRenderable = true;
	OnExit:

		return !WereThereErrors;
	}


	bool LoadPhysicsSettings(lua_State &io_luaState, std::vector<std::string> &o_CollidesWith, bool & o_IsCollidable
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		bool WereThereErrors = false;
		if (LuaHelper::Load_LuaTable(io_luaState, "canCollideWith"
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			int CanCollideWithCount = LuaHelper::GetTableValuesLength(io_luaState);

			o_CollidesWith.reserve(CanCollideWithCount);

			//Check if can collide count is > 0 else ignore the CollidesWith
			if ((CanCollideWithCount > 0) && (!LuaHelper::GetEachStringValuesInCurrentTable(io_luaState, o_CollidesWith, CanCollideWithCount
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif
				)))
			{
				WereThereErrors = true;
			}

			o_IsCollidable = true;
		}
		else
		{
			WereThereErrors = false; //Actor can have no collider
		}

		LuaHelper::UnLoad_LuaTable(io_luaState);
		return !WereThereErrors;
	}

}