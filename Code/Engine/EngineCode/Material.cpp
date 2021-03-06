
#include "PreCompiled.h"
#include <d3dx9shader.h>
#include "Material.h"
#include "Actor.h"
#include "WindowsError.h"
#include "CameraSystem.h"
#include "LightingSystem.h"

#include "../LuaHelper/LuaHelper.h"

namespace Engine
{
	Material::Material(const char *iName, IDirect3DDevice9 * i_direct3dDevice) :
		mName(iName),
		mHashedName(iName),
		m_direct3dDevice(i_direct3dDevice),
		m_vertexShader(NULL),
		m_fragmentShader(NULL),
		m_pvertexShaderConsts(NULL),
		m_pfragmentShaderConsts(NULL),
		m_texture(NULL),
		m_normal_texture(NULL)

	{
		assert(m_direct3dDevice);
	}

	Material::~Material()
	{
		for (unsigned int i = 0; i < m_perMaterialConstantDatas.size(); i++)
		{
			delete m_perMaterialConstantDatas[i];
		}

		for (unsigned int i = 0; i < m_perInstanceConstantDatas.size(); i++)
		{
			delete m_perInstanceConstantDatas[i];
		}

		for (unsigned int i = 0; i < m_perViewConstantDatas.size(); i++)
		{
			delete m_perViewConstantDatas[i];
		}

		for (unsigned int i = 0; i < m_perFrameConstantDatas.size(); i++)
		{
			delete m_perFrameConstantDatas[i];
		}

		if (m_vertexShader)
		{
			m_vertexShader->Release();
			m_vertexShader = NULL;
		}

		if (m_pvertexShaderConsts)
		{
			m_pvertexShaderConsts->Release();
			m_pvertexShaderConsts = NULL;
		}

		if (m_fragmentShader)
		{
			m_fragmentShader->Release();
			m_fragmentShader = NULL;
		}

		if (m_pfragmentShaderConsts)
		{
			m_pfragmentShaderConsts->Release();
			m_pfragmentShaderConsts = NULL;
		}

		if (m_texture)
		{
			m_texture->Release();
			m_texture = NULL;
		}
		
		if (m_normal_texture)
		{
			m_normal_texture->Release();
			m_normal_texture = NULL;
		}
	}

	HRESULT Material::Set(IDirect3DDevice9 * i_direct3dDevice, SharedPointer<Actor> ThisObject
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice && m_vertexShader && m_fragmentShader && m_texture);

		HRESULT result = D3D_OK;
#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
		std::wstringstream EventMessage;
		EventMessage << "Set Material " << (GetName().c_str());
		D3DPERF_BeginEvent(0, EventMessage.str().c_str());
#endif
		// Set the shaders
		result = i_direct3dDevice->SetVertexShader(m_vertexShader);
		assert(SUCCEEDED(result));

		if (FAILED(result))
		{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				*o_errorMessage = "Direct3D failed to set the vertex shader";
			}
#endif
			return result;
		}

		result = i_direct3dDevice->SetPixelShader(m_fragmentShader);
		assert(SUCCEEDED(result));

		if (FAILED(result))
		{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				*o_errorMessage = "Direct3D failed to set the fragment shader";
			}
#endif
			return result;
		}


		result = i_direct3dDevice->SetTexture(m_samplerRegister, m_texture);
		assert(SUCCEEDED(result));

		if (FAILED(result))
		{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				*o_errorMessage = "Direct3D failed to set the texture";
			}
#endif
			return result;
		}

		if (m_normal_texture)
		{
			result = i_direct3dDevice->SetTexture(m_normalsamplerRegister, m_normal_texture);
			assert(SUCCEEDED(result));

			if (FAILED(result))
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					*o_errorMessage = "Direct3D failed to set the m_normal_texture";
				}
#endif
				return result;
			}
		}

		//Create a D3DMATRIX
		D3DXMATRIX LocalToWorld;

		//Transpose the stored matrix
		Matrix4x4 ObjectMatrix = ThisObject->GetLocalToWorldMatrix().GetTranspose();
		LocalToWorld._11 = ObjectMatrix.At(1, 1);
		LocalToWorld._12 = ObjectMatrix.At(1, 2);
		LocalToWorld._13 = ObjectMatrix.At(1, 3);
		LocalToWorld._14 = ObjectMatrix.At(1, 4);
		LocalToWorld._21 = ObjectMatrix.At(2, 1);
		LocalToWorld._22 = ObjectMatrix.At(2, 2);
		LocalToWorld._23 = ObjectMatrix.At(2, 3);
		LocalToWorld._24 = ObjectMatrix.At(2, 4);
		LocalToWorld._31 = ObjectMatrix.At(3, 1);
		LocalToWorld._32 = ObjectMatrix.At(3, 2);
		LocalToWorld._33 = ObjectMatrix.At(3, 3);
		LocalToWorld._34 = ObjectMatrix.At(3, 4);
		LocalToWorld._41 = ObjectMatrix.At(4, 1);
		LocalToWorld._42 = ObjectMatrix.At(4, 2);
		LocalToWorld._43 = ObjectMatrix.At(4, 3);
		LocalToWorld._44 = ObjectMatrix.At(4, 4);

#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
		D3DPERF_BeginEvent(0, L"Set Material Constant (Per-Instance \"g_transform_modelToWorld\")");
#endif
		unsigned int count = 1;
		//Set per-Instance constants
		if (!SetPerInstanceConstantDataByName("ModelToWorld", &LocalToWorld, count))
		{
			assert(false);
		}

#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
		D3DPERF_EndEvent();
#endif
		assert(CameraSystem::GetInstance());

#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
		D3DPERF_BeginEvent(0, L"Set Material Constants (Per-View \"g_transform_worldToView, g_transform_viewToScreen, g_camera_position, g_totalSecondsElapsed \")");
#endif
		//Set per-view constants
		if (!SetPerViewConstantDataByName("WorldToView", &CameraSystem::GetInstance()->GetWorldToView(), count))
		{
			assert(false);
		}

		//Set per-view constants
		if (!SetPerViewConstantDataByName("ViewToScreen", &CameraSystem::GetInstance()->GetViewToScreen(), count))
		{
			assert(false);
		}

		//Set per-view constants
		static float TimeElapsed;
		{
			TimeElapsed += ThisObject->GetDeltaTime();
		}

		if (!SetPerViewConstantDataByName("TimeElapsed", &TimeElapsed, count))
		{
			assert(false);
		}

		//Set per-view constants
		float CameraPosition[3];
		int PosotionDatacount;
		CameraSystem::GetInstance()->m_WorldObject->GetPosition().GetAsFloatArray(CameraPosition, PosotionDatacount);
		if (!SetPerViewConstantDataByName("CameraPosition", CameraPosition, PosotionDatacount))
		{
			assert(false);
		}

#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
		D3DPERF_EndEvent();
#endif


#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
		D3DPERF_BeginEvent(0, L"Set Material Constant (Per-Material)");
#endif
		if (!SetPerMaterialConstantDataFromMaterialFile(
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			o_errorMessage
#endif
			))
		{
			assert(false);

			return false;
		}

#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
	D3DPERF_EndEvent();
#endif

#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
	D3DPERF_BeginEvent(0, L"Set Material Constant (Per-Frame \"g_lighting_ambient, g_lighting, g_light_direction\")");
#endif
		//Set per-Instance constants
		float Data[3];
		int Datacount;
		LightingSystem::GetInstance()->GetAmbientLight().GetAsFloatArray(Data, Datacount);

		if (!SetPerFrameConstantDataByName("AmbientLight", Data, Datacount))
		{
			assert(false);
		}

		LightingSystem::GetInstance()->GetDiffuseLight().GetAsFloatArray(Data, Datacount);

		if (!SetPerFrameConstantDataByName("DiffuseLight", Data, Datacount))
		{
			assert(false);
		}

		LightingSystem::GetInstance()->GetLightDirection().GetAsFloatArray(Data, Datacount);

		if (!SetPerFrameConstantDataByName("LightDirection", Data, Datacount))
		{
			assert(false);
		}

#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
	D3DPERF_EndEvent();
#endif

#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
		D3DPERF_EndEvent();
#endif
		return result;
	}

	// Load material file from path
	bool Material::Load(const char* i_MaterialFilepath, IDirect3DDevice9* i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice && i_MaterialFilepath);

		//Lua Logic
		//---------
		bool wereThereErrors = false;

		// Create a new Lua state
		lua_State* luaState = NULL;
		{
			if (!LuaHelper::Load_LuaFileAndTopTable(i_MaterialFilepath, luaState
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

		// Pop the root table
		LuaHelper::UnLoad_LuaTable(*luaState);

	OnExit:

		if (!wereThereErrors)
		{
			wereThereErrors = !LuaHelper::UnLoad_LuaFile(luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif
				);
		}

		return !wereThereErrors;
	}

	//Load Fragment and Vertex Shader Path.
	bool Material::LoadTableValues(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		if (!LoadTableValues_VertexShader(io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			return false;
		}


		if (!LoadTableValues_FragmentShader(io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			return false;
		}

		//Load Texture and Sampler path.
		if (!LoadTableValues_Texture(io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			return false;
		}

		//Load Normal Texture and Sampler path.
		if (!LoadTableValues_NormalTextureIfPresent(io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			return false;
		}

		if (!LoadConstantDataTable(io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			return false;
		}

		return true;
	}

	//================================================================
	//Constant load logic
	bool Material::LoadConstantDataTable(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		bool wereThereErrors = false;

		if (!LuaHelper::Load_LuaTable(io_luaState, "Constants"
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			wereThereErrors = true;
			goto OnExit;
		}

		//Iterating through the constants key value pairs
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
			const char * ConstantDataTableName = lua_tostring(&io_luaState, IndexOfKey);

			if (lua_istable(&io_luaState, IndexOfValue)) //If it is a table
			{
				const unsigned int DataCountOfValue = luaL_len(&io_luaState, IndexOfValue);
				float *EachDataOfValue = new float[DataCountOfValue];
				if (!LuaHelper::GetEachNumberDataValuesInCurrentTable<float>(io_luaState, EachDataOfValue, DataCountOfValue
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

				//Store the constant
				//At this point both constant name and value are stored,
				//Get reference to per-instance constant
				D3DXHANDLE ConstHandle = m_pvertexShaderConsts->GetConstantByName(NULL, ConstantDataTableName);
				BelongsToenum::BELONGSTO iBelongsTo = BelongsToenum::BELONGSTO::VERTEX_SHADER;

				if (ConstHandle == NULL)
				{
					ConstHandle = m_pfragmentShaderConsts->GetConstantByName(NULL, ConstantDataTableName);
					iBelongsTo = BelongsToenum::FRAGMENT_SHADER;
				}

				if (ConstHandle == NULL)
				{
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "ConstHandle for " << ConstantDataTableName << " is NULL";
						*o_errorMessage = errorMessage.str();
					}
#endif
					iBelongsTo = BelongsToenum::NONE;
				}

				const unsigned int dataCount = DataCountOfValue;
				IsAenum::IsA iIsA = IsAenum::IsA::FLOAT_ARRAY;

				if (ConstHandle != NULL)
				{
					MaterialConstantData<float> *PerMaterialConstant = new MaterialConstantData<float>(ConstantDataTableName, EachDataOfValue, dataCount, ConstHandle, iBelongsTo, iIsA);
					assert(PerMaterialConstant);

					IMaterialConstant * BaseClassPointer = PerMaterialConstant;

					m_perMaterialConstantDatas.push_back(BaseClassPointer);
				}

				delete[] EachDataOfValue;
			}
			else if (lua_type(&io_luaState, IndexOfValue) == LUA_TNUMBER)
			{
				//store the constant
				//At this point both constant name and value are stored,
				//Get reference to per-instance constant
				D3DXHANDLE ConstHandle = m_pvertexShaderConsts->GetConstantByName(NULL, ConstantDataTableName);
				BelongsToenum::BELONGSTO iBelongsTo = BelongsToenum::BELONGSTO::VERTEX_SHADER;

				if (ConstHandle == NULL)
				{
					ConstHandle = m_pfragmentShaderConsts->GetConstantByName(NULL, ConstantDataTableName);
					iBelongsTo = BelongsToenum::FRAGMENT_SHADER;
				}

				if (ConstHandle == NULL)
				{
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "ConstHandle for " << ConstantDataTableName << " is NULL";
						*o_errorMessage = errorMessage.str();
					}
#endif
					iBelongsTo = BelongsToenum::NONE;
				}

				const unsigned int dataCount = 1;
				IsAenum::IsA iIsA = IsAenum::IsA::FLOAT;
				float ConstantValue = static_cast<float>(lua_tonumber(&io_luaState, IndexOfValue));

				if (ConstHandle != NULL)
				{
					MaterialConstantData<float> *PerMaterialConstant = new MaterialConstantData<float>(ConstantDataTableName, &ConstantValue, dataCount, ConstHandle, iBelongsTo, iIsA);
					assert(PerMaterialConstant);

					IMaterialConstant * BaseClassPointer = PerMaterialConstant;

					m_perMaterialConstantDatas.push_back(BaseClassPointer);
				}
			}
			else
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "value must have atleast one value";

					*o_errorMessage = errorMessage.str();
				}
#endif
				wereThereErrors = true;
				goto OnExit;
			}

			//Pop the value, but leave the key
			lua_pop(&io_luaState, 1);
		}

	OnExit:
		// Pop the constant table
		LuaHelper::UnLoad_LuaTable(io_luaState);

		return !wereThereErrors;
	}

	//==================================================================
	//Vertex Shader load logic
	bool Material::LoadTableValues_VertexShader(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		bool wereThereErrors = false;

		if (!LuaHelper::Load_LuaTable(io_luaState, "VertexShader"
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			wereThereErrors = true;
			goto OnExit;
		}

		//Add scope for PathVertexShader to remove error: "initialization of 'PathVertexShader' is skipped by 'goto OnExit'"
		{
			std::string PathVertexShader;
			{
				if (!LuaHelper::GetStringValueFromKey(io_luaState, "Path", PathVertexShader
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					wereThereErrors = true;
					goto OnExit;
				}
			}


			std::map<std::string, std::string> constNameMap;

			std::string ModelToWorldConstantName;
			{
				if (!LuaHelper::GetStringValueFromKey(io_luaState, "ModelToWorld", ModelToWorldConstantName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					wereThereErrors = true;
					goto OnExit;
				}

				constNameMap.insert(std::pair<std::string, std::string>("ModelToWorld", ModelToWorldConstantName));
			}


			std::string WorldToViewConstantName;
			{
				if (!LuaHelper::GetStringValueFromKey(io_luaState, "WorldToView", WorldToViewConstantName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					wereThereErrors = true;
					goto OnExit;
				}

				constNameMap.insert(std::pair<std::string, std::string>("WorldToView", WorldToViewConstantName));
			}

			std::string ViewToScreenConstantName;
			{
				if (!LuaHelper::GetStringValueFromKey(io_luaState, "ViewToScreen", ViewToScreenConstantName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					wereThereErrors = true;
					goto OnExit;
				}

				constNameMap.insert(std::pair<std::string, std::string>("ViewToScreen", ViewToScreenConstantName));
			}

			std::string TimeElapsedConstantName;
			{
				if (LuaHelper::GetStringValueFromKey(io_luaState, "TimeElapsed", TimeElapsedConstantName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					constNameMap.insert(std::pair<std::string, std::string>("TimeElapsed", TimeElapsedConstantName));
				}
			}

			//Load the shader itself from the path
			if (!LoadVertexShader(PathVertexShader.c_str(), constNameMap, m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif		
				))
			{
				wereThereErrors = true;
				goto OnExit;
			}
		}
		//Add More Get functions if  more in the table
	OnExit:

		// Pop the shader table
		LuaHelper::UnLoad_LuaTable(io_luaState);

		return !wereThereErrors;
	}


	//==================================================================
	//Vertex Shader load logic
	bool Material::LoadTableValues_FragmentShader(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		bool wereThereErrors = false;

		if (!LuaHelper::Load_LuaTable(io_luaState, "FragmentShader"
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			wereThereErrors = true;
			goto OnExit;
		}

		//Add scope for PathFragmentShader to remove error: "initialization of 'PathFragmentShader' is skipped by 'goto OnExit'"
		{
			std::string PathFragmentShader;
			{
				if (!LuaHelper::GetStringValueFromKey(io_luaState, "Path", PathFragmentShader
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					wereThereErrors = true;
					goto OnExit;
				}
			}


			std::map<std::string, std::string> constNameMap;

			std::string AmbientLightConstantName;
			{
				if (!LuaHelper::GetStringValueFromKey(io_luaState, "AmbientLight", AmbientLightConstantName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					wereThereErrors = true;
					goto OnExit;
				}

				constNameMap.insert(std::pair<std::string, std::string>("AmbientLight", AmbientLightConstantName));
			}

			std::string DiffuseLightConstantName;
			{
				if (!LuaHelper::GetStringValueFromKey(io_luaState, "DiffuseLight", DiffuseLightConstantName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					wereThereErrors = true;
					goto OnExit;
				}

				constNameMap.insert(std::pair<std::string, std::string>("DiffuseLight", DiffuseLightConstantName));
			}

			std::string LightDirectionConstantName;
			{
				if (!LuaHelper::GetStringValueFromKey(io_luaState, "LightDirection", LightDirectionConstantName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					wereThereErrors = true;
					goto OnExit;
				}

				constNameMap.insert(std::pair<std::string, std::string>("LightDirection", LightDirectionConstantName));
			}

			std::string CameraPositionConstantName;
			{
				if (LuaHelper::GetStringValueFromKey(io_luaState, "CameraPosition", CameraPositionConstantName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					constNameMap.insert(std::pair<std::string, std::string>("CameraPosition", CameraPositionConstantName));
				}
			}

			//Load the shader itself from the path
			if (!LoadFragmentShader(PathFragmentShader.c_str(), constNameMap, m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif		
				))
			{
				wereThereErrors = true;
				goto OnExit;
			}
		}


		//Add More Get functions if  more in the table
	OnExit:

		// Pop the shader table
		LuaHelper::UnLoad_LuaTable(io_luaState);

		return !wereThereErrors;
	}


	//==================================================================
	//Texture load logic
	bool Material::LoadTableValues_Texture(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		bool wereThereErrors = false;
		std::string SamplerName = "g_color_sampler";
		std::string PathTexture = "data/missingTexture.dds";
		bool CouldLoadTexture = true;
		if (LuaHelper::Load_LuaTable(io_luaState, "Texture"
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			SamplerName = "g_color_sampler";
			PathTexture = "data/missingTexture.dds";
			{
				if (LuaHelper::GetStringValueFromKey(io_luaState, "Path", PathTexture
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					if (LuaHelper::GetStringValueFromKey(io_luaState, "Sampler", SamplerName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						, o_errorMessage
#endif
						))
					{

						if (!LoadTextureAndSamplerRegister(PathTexture.c_str(), SamplerName.c_str(), m_texture, m_samplerRegister, m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
							, o_errorMessage
#endif
							))
						{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
							MessageBox(NULL, o_errorMessage->c_str(), PathTexture.c_str(), MB_OK | MB_ICONERROR);
#endif
							CouldLoadTexture = false;
						}
					}
					else
					{
						//Could not find the sampler name for a valid texture
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						std::stringstream errorMessage;
						errorMessage << "Could not find the sampler name for a valid texture: " << o_errorMessage;
						MessageBox(NULL, errorMessage.str().c_str(), SamplerName.c_str(), MB_OK | MB_ICONERROR);
#endif
						wereThereErrors = true;
						goto OnExit;
					}
				}
				else
				{

#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					std::stringstream errorMessage;
					errorMessage << "Could not find the texture path for a valid texture table: " << o_errorMessage;
					MessageBox(NULL, errorMessage.str().c_str(), PathTexture.c_str(), MB_OK | MB_ICONERROR);
#endif
					CouldLoadTexture = false;
				}
			}
		}
		else
		{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			MessageBox(NULL, o_errorMessage->c_str(), PathTexture.c_str(), MB_OK | MB_ICONERROR);
#endif
			CouldLoadTexture = false;
		}

		if (!CouldLoadTexture)
		{
			//Try to laod default texture
			SamplerName = "g_color_sampler";
			PathTexture = "data/missingTexture.dds";
			if (!LoadTextureAndSamplerRegister(PathTexture.c_str(), SamplerName.c_str(), m_texture, m_samplerRegister, m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif
				))
			{
				//Could not load default texture, fail the function
				wereThereErrors = true;
				goto OnExit;
			}
		}

	OnExit:

		// Pop the texture table
		LuaHelper::UnLoad_LuaTable(io_luaState);
		return !wereThereErrors;
	}

	//Texture load logic
		bool Material::LoadTableValues_NormalTextureIfPresent(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		bool wereThereErrors = false;
		std::string SamplerName;
		std::string PathTexture;
		if (LuaHelper::Load_LuaTable(io_luaState, "NormalTexture"
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			{
				if (LuaHelper::GetStringValueFromKey(io_luaState, "Path", PathTexture
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, o_errorMessage
#endif
					))
				{
					if (LuaHelper::GetStringValueFromKey(io_luaState, "Sampler", SamplerName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						, o_errorMessage
#endif
						))
					{

						if (!LoadTextureAndSamplerRegister(PathTexture.c_str(), SamplerName.c_str(), m_normal_texture, m_normalsamplerRegister, m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
							, o_errorMessage
#endif
							))
						{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
							MessageBox(NULL, o_errorMessage->c_str(), PathTexture.c_str(), MB_OK | MB_ICONERROR);
#endif
							wereThereErrors = true;
						}
					}
					else
					{
						//Could not find the sampler name for a valid texture
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						std::stringstream errorMessage;
						errorMessage << "Could not find the sampler name for a valid normal texture: " << o_errorMessage;
						MessageBox(NULL, errorMessage.str().c_str(), SamplerName.c_str(), MB_OK | MB_ICONERROR);
#endif
						wereThereErrors = true;
					}
				}
				else
				{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					std::stringstream errorMessage;
					errorMessage << "Could not find the texture path for a valid normal texture table: " << o_errorMessage;
					MessageBox(NULL, errorMessage.str().c_str(), PathTexture.c_str(), MB_OK | MB_ICONERROR);
#endif
					wereThereErrors = true;
				}
			}
		}

		// Pop the texture table
		LuaHelper::UnLoad_LuaTable(io_luaState);
		return !wereThereErrors;
	}

	bool Material::LoadTextureAndSamplerRegister(const char* iTexturePath, const char* iSamplerName, 
		IDirect3DTexture9* & o_texture, DWORD &o_samplerRegister, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice && iTexturePath && iSamplerName);

		bool WereThereErrors = false;

		HRESULT result = D3DXCreateTextureFromFile(i_direct3dDevice, iTexturePath, &o_texture);
		
		if (FAILED(result))
		{
			WereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "Failed to load texture: " << GetLastWindowsError();
				*o_errorMessage = errorMessage.str();
			}
#endif
			goto OnExit;
		}

		D3DXHANDLE samplerHandle = m_pfragmentShaderConsts->GetConstantByName(NULL, iSamplerName);
		
		if (samplerHandle == NULL)
		{
			WereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "Failed to get Constant by name: " << GetLastWindowsError();
				*o_errorMessage = errorMessage.str();
			}
#endif
			goto OnExit;
		}

		o_samplerRegister = static_cast<DWORD>(m_pfragmentShaderConsts->GetSamplerIndex(samplerHandle));

	OnExit:
		return !WereThereErrors;
	}


	bool Material::SetPerMaterialConstantDataFromMaterialFile(
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		std::string* o_errorMessage
#endif		
		)
	{
		bool WereThereErrors = false;

		for (unsigned int i = 0; i < m_perMaterialConstantDatas.size(); i++)
		{
			
			if (!SetConstantDataByType(m_perMaterialConstantDatas.at(i)->GetName().c_str(), 
										m_perMaterialConstantDatas.at(i)->GetValue(), 
										m_perMaterialConstantDatas.at(i),
										m_perMaterialConstantDatas.at(i)->GetCount()))
			{
				WereThereErrors = true;
			}

			if (WereThereErrors)
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Setting Constant table value failed with error: " << "\n";

					*o_errorMessage = errorMessage.str();
				}
#endif
				WereThereErrors = true;
				assert(false);
			}
		}

		return !WereThereErrors;
	}

	bool Material::SetPerViewConstantDataByName(const char * i_name, const void* i_pValue, const unsigned int i_count)
	{
		bool WereThereErrors = false;

		assert(i_pValue && i_name);

		for (unsigned int i = 0; i < m_perViewConstantDatas.size(); i++)
		{

			if (i_name != m_perViewConstantDatas.at(i)->GetName())
			{
				continue;
			}

			if (!SetConstantDataByType(i_name, i_pValue, m_perViewConstantDatas.at(i), i_count))
			{
				WereThereErrors = true;
			}
		}

		return !WereThereErrors;
	}

	bool Material::SetPerFrameConstantDataByName(const char * i_name, const void* i_pValue, const unsigned int i_count)
	{
		bool WereThereErrors = false;

		assert(i_pValue && i_name);

		for (unsigned int i = 0; i < m_perFrameConstantDatas.size(); i++)
		{

			if (i_name != m_perFrameConstantDatas.at(i)->GetName())
			{
				continue;
			}

			if (!SetConstantDataByType(i_name, i_pValue, m_perFrameConstantDatas.at(i), i_count))
			{
				WereThereErrors = true;
			}
		}

		return !WereThereErrors;
	}


	bool Material::SetPerInstanceConstantDataByName(const char * i_name, const void* i_pValue, const unsigned int i_count)
	{
		bool WereThereErrors = false;

		assert(i_pValue && i_name);

		for (unsigned int i = 0; i < m_perInstanceConstantDatas.size(); i++)
		{
			if (i_name != m_perInstanceConstantDatas.at(i)->GetName())
			{
				continue;
			}

			if (!SetConstantDataByType(i_name, i_pValue, m_perInstanceConstantDatas.at(i), i_count))
			{
				WereThereErrors = true;
			}
		}

		return !WereThereErrors;
	}

	bool Material::SetConstantDataByType(const char * i_name, const void* i_pValue, IMaterialConstant *io_materialContantData, const unsigned int i_count)
	{
		assert(io_materialContantData);

		bool WereThereErrors = false;

		ID3DXConstantTable* ThisConstTable;
		{
			switch (io_materialContantData->GetBelongsTo())
			{
				case BelongsToenum::FRAGMENT_SHADER:
				{
					ThisConstTable = m_pfragmentShaderConsts;
					break;
				}
				case BelongsToenum::VERTEX_SHADER:
				{
					ThisConstTable = m_pvertexShaderConsts;
					break;
				}
				default:
				{
					assert(false);
				}
			}
		}

		const D3DXHANDLE ThisShaderConstHandle = io_materialContantData->GetHandle();

		HRESULT result;

		switch (io_materialContantData->GetIsA())
		{
			case IsAenum::IsA::FLOAT:
			{
				assert(i_count == 1);

				const float pValue = *(reinterpret_cast<const float *>(i_pValue));

				result = ThisConstTable->SetFloat(m_direct3dDevice, ThisShaderConstHandle, pValue);

				break;
			}
			case IsAenum::IsA::FLOAT_ARRAY:
			{
				assert(i_count == io_materialContantData->GetCount());

				const float* pValue = (reinterpret_cast<const float *>(i_pValue));

				result = ThisConstTable->SetFloatArray(m_direct3dDevice, ThisShaderConstHandle, pValue, i_count);

				break;
			}

			case IsAenum::IsA::VECTOR_4D:
			{
				assert(i_count == 1);

				const D3DXVECTOR4* p4DVectorValue = (reinterpret_cast<const D3DXVECTOR4 *>(i_pValue));

				result = ThisConstTable->SetVector(m_direct3dDevice, ThisShaderConstHandle, p4DVectorValue);

				break;
			}

			case IsAenum::IsA::MATRIX_4X4:
			{
				assert(i_count == 1);

				const D3DXMATRIX* p4DVectorValue = (reinterpret_cast<const D3DXMATRIX *>(i_pValue));

				result = ThisConstTable->SetMatrix(m_direct3dDevice, ThisShaderConstHandle, p4DVectorValue);

				break;
			}

			default:
			{
				result = -1; //failure
				assert(false);
			}
		}

		if (FAILED(result))
		{
			WereThereErrors = true;
			assert(false);
		}

		return !WereThereErrors;
	}

	bool Material::LoadAndAllocateShaderProgram(const char* i_path, void*& o_compiledShader
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		bool wereThereErrors = false;

		// Load the compiled shader from disk
		o_compiledShader = NULL;
		HANDLE fileHandle = INVALID_HANDLE_VALUE;
		{
			// Open the file
			{
				const DWORD desiredAccess = FILE_GENERIC_READ;
				const DWORD otherProgramsCanStillReadTheFile = FILE_SHARE_READ;
				SECURITY_ATTRIBUTES* useDefaultSecurity = NULL;
				const DWORD onlySucceedIfFileExists = OPEN_EXISTING;
				const DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
				const HANDLE dontUseTemplateFile = NULL;
				fileHandle = CreateFile(i_path, desiredAccess, otherProgramsCanStillReadTheFile,
					useDefaultSecurity, onlySucceedIfFileExists, useDefaultAttributes, dontUseTemplateFile);
				if (fileHandle == INVALID_HANDLE_VALUE)
				{
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Windows failed to open the shader file: " << GetLastWindowsError();
						*o_errorMessage = errorMessage.str();
					}
#endif
					goto OnExit;
				}
			}
			// Get the file's size
			size_t fileSize;
			{
				LARGE_INTEGER fileSize_integer;
				if (GetFileSizeEx(fileHandle, &fileSize_integer) != FALSE)
				{
					// This is unsafe if the file's size is bigger than a size_t
					fileSize = static_cast<size_t>(fileSize_integer.QuadPart);
				}
				else
				{
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Windows failed to get the size of shader: " << GetLastWindowsError();
						*o_errorMessage = errorMessage.str();
					}
#endif
					goto OnExit;
				}
			}
			// Read the file's contents into temporary memory
			o_compiledShader = malloc(fileSize);
			if (o_compiledShader)
			{
				DWORD bytesReadCount;
				OVERLAPPED* readSynchronously = NULL;
				if (ReadFile(fileHandle, o_compiledShader, fileSize,
					&bytesReadCount, readSynchronously) == FALSE)
				{
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Windows failed to read the contents of shader: " << GetLastWindowsError();
						*o_errorMessage = errorMessage.str();
					}
#endif
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
					errorMessage << "Failed to allocate " << fileSize << " bytes to read in the shader program " << i_path;
					*o_errorMessage = errorMessage.str();
				}
#endif
				goto OnExit;
			}
		}

	OnExit:

		if (wereThereErrors && o_compiledShader)
		{
			free(o_compiledShader);
			o_compiledShader = NULL;
		}

		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			if (CloseHandle(fileHandle) == FALSE)
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (!wereThereErrors && o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Windows failed to close the shader file handle: " << GetLastWindowsError();
					*o_errorMessage = errorMessage.str();
				}
#endif
				wereThereErrors = true;
			}
			fileHandle = INVALID_HANDLE_VALUE;
		}

		return !wereThereErrors;
	}


	bool Material::LoadFragmentShader(const char* i_FragmentShaderpath, std::map<std::string, std::string> & i_constantNameMap, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice && i_FragmentShaderpath);
		
		// Load the compiled source file it
		void * compiledShader;
		{
			if (!LoadAndAllocateShaderProgram(i_FragmentShaderpath, compiledShader
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif
				))
			{

				return false;
			}
		}

		D3DXGetShaderConstantTable(reinterpret_cast<DWORD*>(compiledShader), &m_pfragmentShaderConsts);

		if (m_pfragmentShaderConsts != NULL)
		{
			if (!LoadFragmentShaderConstants(i_FragmentShaderpath, m_pfragmentShaderConsts, i_constantNameMap, i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif
				))
			{
				return false;
			}
		}

		bool wereThereErrors = false;
		{
			HRESULT result = i_direct3dDevice->CreatePixelShader(reinterpret_cast<DWORD*>(compiledShader),
																&m_fragmentShader);
			if (FAILED(result))
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					*o_errorMessage = "DirectX failed to create the fragment shader";
				}
#endif
				wereThereErrors = true;
			}

			if (compiledShader) free(compiledShader);
		}

		return !wereThereErrors;
	}

	bool Material::LoadFragmentShaderConstants(const char* i_FragmentShaderpath, ID3DXConstantTable* i_pfragmentShaderConsts, std::map<std::string, std::string> & i_constantNameMap, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_pfragmentShaderConsts && i_direct3dDevice && i_FragmentShaderpath);

		i_pfragmentShaderConsts->SetDefaults(i_direct3dDevice); //Set constant default

		//-----------------------g_lighting_ambient-------------
		{
			const char * cGlobalConstantName = "AmbientLight";
			std::map<std::string, std::string>::iterator it;

			//Find the actual constant name 
			it = i_constantNameMap.find(cGlobalConstantName);

			if (it == i_constantNameMap.end())
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not find constant name in fragment shader for " << cGlobalConstantName;
					*o_errorMessage = errorMessage.str();
				}

				return false;
#endif
			}

			D3DXHANDLE Handle = i_pfragmentShaderConsts->GetConstantByName(NULL, it->second.c_str());

			if (Handle == NULL)
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not find constant " << it->second.c_str() << " in " << i_FragmentShaderpath << " fragment shader for " << cGlobalConstantName;
					*o_errorMessage = errorMessage.str();
				}

				return false;
#endif
			}

			assert(LightingSystem::GetInstance());
			float FloatDataArray[3];
			int dataCount;
			LightingSystem::GetInstance()->GetAmbientLight().GetAsFloatArray(FloatDataArray, dataCount);

			BelongsToenum::BELONGSTO iBelongsTo = BelongsToenum::BELONGSTO::FRAGMENT_SHADER;
			IsAenum::IsA iIsA = IsAenum::IsA::FLOAT_ARRAY;

			MaterialConstantData<float> *PerFrameFragmentShaderConstant = new MaterialConstantData<float>(cGlobalConstantName, FloatDataArray, dataCount, Handle, iBelongsTo, iIsA);
			assert(PerFrameFragmentShaderConstant);

			IMaterialConstant * BaseClassPointer = PerFrameFragmentShaderConstant;

			m_perFrameConstantDatas.push_back(BaseClassPointer);

		}

		//-----------------------g_lighting-------------
		{
			const char * cGlobalConstantName = "DiffuseLight";
			std::map<std::string, std::string>::iterator it;

			//Find the actual constant name 
			it = i_constantNameMap.find(cGlobalConstantName);

			if (it == i_constantNameMap.end())
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not find constant name in fragment shader for " << cGlobalConstantName;
					*o_errorMessage = errorMessage.str();
				}

				return false;
#endif
			}

			D3DXHANDLE Handle = i_pfragmentShaderConsts->GetConstantByName(NULL, it->second.c_str());
			
			if (Handle == NULL)
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not find constant " << it->second.c_str() << " in " << i_FragmentShaderpath <<" fragment shader for " << cGlobalConstantName;
					*o_errorMessage = errorMessage.str();
				}

				return false;
#endif
			}

			assert(LightingSystem::GetInstance());
			float FloatDataArray[3];
			int dataCount;
			LightingSystem::GetInstance()->GetDiffuseLight().GetAsFloatArray(FloatDataArray, dataCount);

			BelongsToenum::BELONGSTO iBelongsTo = BelongsToenum::BELONGSTO::FRAGMENT_SHADER;
			IsAenum::IsA iIsA = IsAenum::IsA::FLOAT_ARRAY;

			MaterialConstantData<float> *PerFrameFragmentShaderConstant = new MaterialConstantData<float>(cGlobalConstantName, FloatDataArray, dataCount, Handle, iBelongsTo, iIsA);
			assert(PerFrameFragmentShaderConstant);

			IMaterialConstant * BaseClassPointer = PerFrameFragmentShaderConstant;

			m_perFrameConstantDatas.push_back(BaseClassPointer);

		}

		//-----------------------g_light_direction-------------
		{
			const char * cGlobalConstantName = "LightDirection";
			std::map<std::string, std::string>::iterator it;

			//Find the actual constant name 
			it = i_constantNameMap.find(cGlobalConstantName);

			if (it == i_constantNameMap.end())
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not find constant name in fragment shader for " << cGlobalConstantName;
					*o_errorMessage = errorMessage.str();
				}

				return false;
#endif
			}

			D3DXHANDLE Handle = i_pfragmentShaderConsts->GetConstantByName(NULL, it->second.c_str());

			if (Handle == NULL)
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not find constant " << it->second.c_str() << " in " << i_FragmentShaderpath << " fragment shader for " << cGlobalConstantName;
					*o_errorMessage = errorMessage.str();
				}

				return false;
#endif
			}

			assert(LightingSystem::GetInstance());
			float FloatDataArray[3];
			int dataCount;
			LightingSystem::GetInstance()->GetLightDirection().GetAsFloatArray(FloatDataArray, dataCount);

			BelongsToenum::BELONGSTO iBelongsTo = BelongsToenum::BELONGSTO::FRAGMENT_SHADER;
			IsAenum::IsA iIsA = IsAenum::IsA::FLOAT_ARRAY;

			MaterialConstantData<float> *PerFrameFragmentShaderConstant = new MaterialConstantData<float>(cGlobalConstantName, FloatDataArray, dataCount, Handle, iBelongsTo, iIsA);
			assert(PerFrameFragmentShaderConstant);

			IMaterialConstant * BaseClassPointer = PerFrameFragmentShaderConstant;

			m_perFrameConstantDatas.push_back(BaseClassPointer);
		}


		//-----------------------g_camera_position-------------
		{
			const char * cGlobalConstantName = "CameraPosition";
			std::map<std::string, std::string>::iterator it;

			//Find the actual constant name 
			it = i_constantNameMap.find(cGlobalConstantName);

			if (it != i_constantNameMap.end())
			{
				D3DXHANDLE Handle = i_pfragmentShaderConsts->GetConstantByName(NULL, it->second.c_str());
				if (Handle == NULL)
				{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Could not find constant " << it->second.c_str() << " in " << i_FragmentShaderpath << " fragment shader for " << cGlobalConstantName;
						*o_errorMessage = errorMessage.str();
					}

					return false;
				}
#endif
				assert(LightingSystem::GetInstance());
				float FloatDataArray[3];
				int dataCount;
				LightingSystem::GetInstance()->GetLightDirection().GetAsFloatArray(FloatDataArray, dataCount);

				BelongsToenum::BELONGSTO iBelongsTo = BelongsToenum::BELONGSTO::FRAGMENT_SHADER;
				IsAenum::IsA iIsA = IsAenum::IsA::FLOAT_ARRAY;

				MaterialConstantData<float> *PerViewFragmentShaderConstant = new MaterialConstantData<float>(cGlobalConstantName, FloatDataArray, dataCount, Handle, iBelongsTo, iIsA);
				assert(PerViewFragmentShaderConstant);

				IMaterialConstant * BaseClassPointer = PerViewFragmentShaderConstant;

				m_perViewConstantDatas.push_back(BaseClassPointer);
			}
		}

		return true;
	}

	bool Material::LoadVertexShader(const char* i_VertexShaderpath, std::map<std::string, std::string> & i_constantNameMap, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice && i_VertexShaderpath);

		// Load the compiled source file it
		void * compiledShader;
		{
			if (!LoadAndAllocateShaderProgram(i_VertexShaderpath, compiledShader
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif
				))
			{

				return false;
			}
		}

		D3DXGetShaderConstantTable(reinterpret_cast<DWORD*>(compiledShader), &m_pvertexShaderConsts);

		//Get reference to per-instance constant
		if (m_pvertexShaderConsts != NULL)
		{
			if (!LoadVertexShaderConstants(i_VertexShaderpath, m_pvertexShaderConsts, i_constantNameMap, i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif
				))
			{
				return false;
			}
		}

		// Create the vertex shader object
		bool wereThereErrors = false;
		{
			HRESULT result = i_direct3dDevice->CreateVertexShader(reinterpret_cast<DWORD*>(compiledShader),//->GetBufferPointer()),
																	&m_vertexShader);

			if (FAILED(result))
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					*o_errorMessage = "DirectX failed to create the vertex shader";
				}
#endif
				wereThereErrors = true;
			}

			if (compiledShader) free(compiledShader);
		}

		return !wereThereErrors;
	}


	bool Material::LoadVertexShaderConstants(const char* i_VertexShaderpath, ID3DXConstantTable* i_pvertexShaderConsts, std::map<std::string, std::string> & i_constantNameMap, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_pvertexShaderConsts && i_direct3dDevice);

		i_pvertexShaderConsts->SetDefaults(i_direct3dDevice); //Set constant default

		D3DXMATRIX pMatrixValue;
		D3DXMatrixIdentity(&pMatrixValue);

		//-------------------g_transform_modelToWorld------------
		{
			const char * cGlobalConstantName = "ModelToWorld";
			std::map<std::string, std::string>::iterator it;

			//Find the actual constant name 
			it = i_constantNameMap.find(cGlobalConstantName);

			if (it == i_constantNameMap.end())
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not find constant name in fragment shader for " << cGlobalConstantName;
					*o_errorMessage = errorMessage.str();
				}

				return false;
#endif
			}

			D3DXHANDLE Handle = i_pvertexShaderConsts->GetConstantByName(NULL, it->second.c_str());

			if (Handle == NULL)
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not find constant " << it->second.c_str() << " in " << i_VertexShaderpath << " vertex shader for " << cGlobalConstantName;
					*o_errorMessage = errorMessage.str();
				}

				return false;
#endif
			}

			const unsigned int dataCount = 1;
			BelongsToenum::BELONGSTO iBelongsTo = BelongsToenum::BELONGSTO::VERTEX_SHADER;
			IsAenum::IsA iIsA = IsAenum::IsA::MATRIX_4X4;

			MaterialConstantData<D3DXMATRIX> *PerInstanceMaterialConstant = new MaterialConstantData<D3DXMATRIX>(cGlobalConstantName, &pMatrixValue, dataCount, Handle, iBelongsTo, iIsA);
			assert(PerInstanceMaterialConstant);

			IMaterialConstant * BaseClassPointer = PerInstanceMaterialConstant;

			m_perInstanceConstantDatas.push_back(BaseClassPointer);
		}

		//-----------------------g_transform_worldToView-------------
		{
			const char * cGlobalConstantName = "WorldToView";
			std::map<std::string, std::string>::iterator it;

			//Find the actual constant name 
			it = i_constantNameMap.find(cGlobalConstantName);

			if (it == i_constantNameMap.end())
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not find constant name in fragment shader for " << cGlobalConstantName;
					*o_errorMessage = errorMessage.str();
				}

				return false;
#endif
			}

			D3DXHANDLE Handle = i_pvertexShaderConsts->GetConstantByName(NULL, it->second.c_str());

			if (Handle == NULL)
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not find constant " << it->second.c_str() << " in " << i_VertexShaderpath << " vertex shader for " << cGlobalConstantName;
					*o_errorMessage = errorMessage.str();
				}

				return false;
#endif
			}

			const unsigned int dataCount = 1;
			BelongsToenum::BELONGSTO iBelongsTo = BelongsToenum::BELONGSTO::VERTEX_SHADER;
			IsAenum::IsA iIsA = IsAenum::IsA::MATRIX_4X4;

			MaterialConstantData<D3DXMATRIX> *PerViewMaterialConstant = new MaterialConstantData<D3DXMATRIX>(cGlobalConstantName, &pMatrixValue, dataCount, Handle, iBelongsTo, iIsA);
			assert(PerViewMaterialConstant);

			IMaterialConstant * BaseClassPointer = PerViewMaterialConstant;

			m_perViewConstantDatas.push_back(BaseClassPointer);

		}

		//-----------------------g_transform_viewToScreen-------------
		{
			const char * cGlobalConstantName = "ViewToScreen";
			std::map<std::string, std::string>::iterator it;

			//Find the actual constant name 
			it = i_constantNameMap.find(cGlobalConstantName);

			if (it == i_constantNameMap.end())
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not find constant name in fragment shader for " << cGlobalConstantName;
					*o_errorMessage = errorMessage.str();
				}

				return false;
#endif
			}

			D3DXHANDLE Handle = i_pvertexShaderConsts->GetConstantByName(NULL, it->second.c_str());

			if (Handle == NULL)
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not find constant " << it->second.c_str() << " in " << i_VertexShaderpath << " vertex shader for " << cGlobalConstantName;
					*o_errorMessage = errorMessage.str();
				}

				return false;
#endif
			}

			const unsigned int dataCount = 1;
			BelongsToenum::BELONGSTO iBelongsTo = BelongsToenum::BELONGSTO::VERTEX_SHADER;
			IsAenum::IsA iIsA = IsAenum::IsA::MATRIX_4X4;

			MaterialConstantData<D3DXMATRIX> *PerViewMaterialConstant = new MaterialConstantData<D3DXMATRIX>(cGlobalConstantName, &pMatrixValue, dataCount, Handle, iBelongsTo, iIsA);
			assert(PerViewMaterialConstant);

			IMaterialConstant * BaseClassPointer = PerViewMaterialConstant;

			m_perViewConstantDatas.push_back(BaseClassPointer);
		}

		//-----------------------g_totalSecondsElapsed-------------
		{
			const char * cGlobalConstantName = "TimeElapsed";
			std::map<std::string, std::string>::iterator it;

			//Find the actual constant name 
			it = i_constantNameMap.find(cGlobalConstantName);

			if (it != i_constantNameMap.end())
			{
				D3DXHANDLE Handle = i_pvertexShaderConsts->GetConstantByName(NULL, it->second.c_str());

				if (Handle == NULL)
				{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Could not find constant " << it->second.c_str() << " in " << i_VertexShaderpath << " vertex shader for " << cGlobalConstantName;
						*o_errorMessage = errorMessage.str();
					}

					return false;
#endif
				}

				const unsigned int dataCount = 1;
				const float constantTimeFrame = 1000.0f / 60.0f;
				BelongsToenum::BELONGSTO iBelongsTo = BelongsToenum::BELONGSTO::VERTEX_SHADER;
				IsAenum::IsA iIsA = IsAenum::IsA::FLOAT;

				MaterialConstantData<float> *PerViewMaterialConstant = new MaterialConstantData<float>(cGlobalConstantName, &constantTimeFrame, dataCount, Handle, iBelongsTo, iIsA);
				assert(PerViewMaterialConstant);

				IMaterialConstant * BaseClassPointer = PerViewMaterialConstant;

				m_perViewConstantDatas.push_back(BaseClassPointer);
			}
		}

		return true;
	}
}
