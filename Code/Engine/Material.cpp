
#include "PreCompiled.h"
#include <d3dx9shader.h>
#include "Material.h"
#include "WindowsError.h"

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
		m_texture(NULL)

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
	}

	HRESULT Material::Set(IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice && m_vertexShader && m_fragmentShader && m_texture);

		HRESULT result = D3D_OK;
#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
		D3DPERF_BeginEvent(0, L"Set Material (Per-Material)");
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
		// Set the shaders
		result = i_direct3dDevice->SetVertexShader(m_vertexShader);
		assert(SUCCEEDED(result));
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		if (FAILED(result))
		{
			if (o_errorMessage)
			{
				*o_errorMessage = "Direct3D failed to set the vertex shader";
			}
			return result;
		}
#endif
		result = i_direct3dDevice->SetPixelShader(m_fragmentShader);
		assert(SUCCEEDED(result));
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		if (FAILED(result))
		{
			if (o_errorMessage)
			{
				*o_errorMessage = "Direct3D failed to set the fragment shader";
			}
			return result;
		}
#endif

		result = i_direct3dDevice->SetTexture(m_samplerRegister, m_texture);
		assert(SUCCEEDED(result));
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		if (FAILED(result))
		{
			if (o_errorMessage)
			{
				*o_errorMessage = "Direct3D failed to set the texture";
			}
			return result;
		}
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
		assert(i_MaterialFilepath);
		bool wereThereErrors = false;

		// Create a new Lua state
		lua_State* luaState = NULL;
		{
			luaState = luaL_newstate();
			if (!luaState)
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
			const int luaResult = luaL_loadfile(luaState, i_MaterialFilepath);
			if (luaResult != LUA_OK)
			{
				wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					*o_errorMessage = lua_tostring(luaState, -1);
				}
#endif
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
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						if (o_errorMessage)
						{
							std::stringstream errorMessage;
							errorMessage << "Asset files must return a table (instead of a "
								<< luaL_typename(luaState, -1) << ")\n";

							*o_errorMessage = errorMessage.str();
						}
#endif
						// Pop the returned non-table value
						lua_pop(luaState, 1);
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
					lua_pop(luaState, returnedValueCount);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					*o_errorMessage = lua_tostring(luaState, -1);
				}
#endif
				// Pop the error message
				lua_pop(luaState, 1);
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

//Load Fragment and Vertex Shader Path.
//Load Texture and Sampler path.
	bool Material::LoadTableValues(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		std::string PathVertexShader;
		std::string PathFragmentShader;
		std::string PathTexture = "data/missingTexture.dds";
		std::string SamplerName = "g_color_sampler";
		if (!LoadTableValues_Shaders(io_luaState, "VertexShader", PathVertexShader
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			return false;
		}

		//Load the shader itself from the path
		if (!LoadVertexShader(PathVertexShader.c_str(), m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif		
			))
		{
			return false;
		}

		if (!LoadTableValues_Shaders(io_luaState, "FragmentShader", PathFragmentShader
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			return false;
		}

		//Load the shader itself from the path
		if (!LoadFragmentShader(PathFragmentShader.c_str(), m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif		
			))
		{
			return false;
		}

		LoadTableValues_Texture(io_luaState, "Texture", PathTexture, SamplerName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, o_errorMessage
#endif
		);

		if (!LoadTextureAndSamplerRegister(PathTexture.c_str(), SamplerName.c_str(), m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			return false;
		}

		if (!LoadConstantDataTable(io_luaState, "Constants"
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
	bool Material::LoadConstantDataTable(lua_State& io_luaState, const char* RootConstantTableName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(RootConstantTableName);
		bool wereThereErrors = false;

		// Right now the asset table is at -1.
		// After the following table operation it will be at -2
		// and the "textures" table will be at -1:
		lua_pushstring(&io_luaState, RootConstantTableName);
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
		if (lua_istable(&io_luaState, -1))
		{
			if (!LoadEachConstantData(io_luaState
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
			//No constant table in material file.
		}

	OnExit:

		// Pop the table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}


	bool Material::LoadEachConstantData(lua_State& io_luaState
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

				// Pop the returned key value pair on error
				lua_pop(&io_luaState, 2);
				goto OnExit;
#endif
			}

			//Store the valid key in a variable
			const char * ConstantName = lua_tostring(&io_luaState, IndexOfKey);

			if (!lua_istable(&io_luaState, IndexOfValue))
			{
				wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "value must be a table (instead of a " <<
						luaL_typename(&io_luaState, IndexOfValue) << ")\n";

					*o_errorMessage = errorMessage.str();
				}

				// Pop the returned key value pair on error
				lua_pop(&io_luaState, 2);
				goto OnExit;
#endif
			}

			if (!LoadEachConstantDataValue(io_luaState, ConstantName
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
		}


	OnExit:

		return !wereThereErrors;
	}

	bool Material::LoadEachConstantDataValue(lua_State& io_luaState, const char * i_ConstantName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(m_pvertexShaderConsts && m_pfragmentShaderConsts);
		bool wereThereErrors = false;

		//Iterating through every value table
		if (strcmp(i_ConstantName, "g_color_perMaterial") == 0)
		{
			const int DataCount = luaL_len(&io_luaState, -1);
			float *pValue = new float[DataCount];
			assert(pValue);

			for (int i = 1; i <= DataCount; ++i)
			{
				lua_pushinteger(&io_luaState, i);
				const int currentIndexOfConstantDataTable = -2;
				lua_gettable(&io_luaState, currentIndexOfConstantDataTable);

				if (lua_type(&io_luaState, -1) != LUA_TNUMBER)
				{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "value must be a number (instead of a " <<
							luaL_typename(&io_luaState, -1) << ")\n";
						*o_errorMessage = errorMessage.str();
					}
#endif
					//Pop the invalid data value from stack and return false on error
					lua_pop(&io_luaState, 1);

					return false;
				}

				pValue[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
				//Pop the value from the stack since it is stored
				lua_pop(&io_luaState, 1);
			}


			//At this point both key and value pairs are stored in ThisData,
			//Get reference to per-instance constant


			D3DXHANDLE ConstHandle = m_pvertexShaderConsts->GetConstantByName(NULL, i_ConstantName);
			BelongsToenum::BELONGSTO iBelongsTo = BelongsToenum::BELONGSTO::VERTEX_SHADER;

			if (ConstHandle == NULL)
			{
				ConstHandle = m_pfragmentShaderConsts->GetConstantByName(NULL, i_ConstantName);
				iBelongsTo = BelongsToenum::FRAGMENT_SHADER;
			}

			if (ConstHandle == NULL)
			{
				wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "ConstHandle for " << i_ConstantName <<" is NULL";
					*o_errorMessage = errorMessage.str();
				}
#endif
				iBelongsTo = BelongsToenum::NONE;
			}

			const unsigned int dataCount = DataCount;
			IsAenum::IsA iIsA = IsAenum::IsA::FLOAT_ARRAY;

			if (ConstHandle != NULL)
			{
				MaterialConstantData<float> *PerMaterialConstant = new MaterialConstantData<float>(i_ConstantName, pValue, dataCount, ConstHandle, iBelongsTo, iIsA);
				assert(PerMaterialConstant);

				IMaterialConstant * BaseClassPointer = PerMaterialConstant;

				m_perMaterialConstantDatas.push_back(BaseClassPointer);
			}

			delete[] pValue;
		}
		else
		{
			assert(false);
			//handle case for new constants
		}
		
		return !wereThereErrors;
	}

	//==================================================================
	//Shader load logic
	bool Material::LoadTableValues_Shaders(lua_State& io_luaState, const char* key, std::string& o_PathShader
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(key);
		bool wereThereErrors = false;

		// Right now the asset table is at -1.
		// After the following table operation it will be at -2
		// and the "textures" table will be at -1:
		lua_pushstring(&io_luaState, key);
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
		if (lua_istable(&io_luaState, -1))
		{
			if (!LoadTableValues_Shader_paths(io_luaState, o_PathShader
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
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "The value at \"" << key << "\" must be a table "
					"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";

				*o_errorMessage = errorMessage.str();
			}
#endif

			goto OnExit;
		}

	OnExit:

		// Pop the shader table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool Material::LoadTableValues_Shader_paths(lua_State& io_luaState, std::string& o_PathShader
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		// Right now the asset table is at -2
		// and the textures table is at -1.
		// NOTE, however, that it doesn't matter to me in this function
		// that the asset table is at -2.
		// Because I've carefully called a new function for every "stack level"
		// The only thing I care about is that the textures table that I care about
		// is at the top of the stack.
		// As long as I make sure that when I leave this function it is _still_
		// at -1 then it doesn't matter to me at all what is on the stack below it.
		bool wereThereErrors = false;
		
		const int MAX_COUNT_OF_VALUES = 1;
		int CountOfValues = 0;

		//Iterating through the constant key value pairs
		lua_pushnil(&io_luaState);
		int CurrentIndexOfConstantTable = -2;
		while (lua_next(&io_luaState, CurrentIndexOfConstantTable))
		{
			//Only one path should be present in this table
			assert(CountOfValues <= MAX_COUNT_OF_VALUES);

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
			const char * PathKeyName = lua_tostring(&io_luaState, IndexOfKey);

			if (strcmp(PathKeyName, "Path") == 0)
			{
				// A correct asset file _must_ return a string as value
				if (lua_type(&io_luaState, IndexOfValue) != LUA_TSTRING)
				{
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "file paths must be a string (instead of a " <<
							luaL_typename(&io_luaState, IndexOfValue) << ")\n";

						*o_errorMessage = errorMessage.str();
					}
#endif

					// Pop the returned non-string value
					lua_pop(&io_luaState, 1);
					goto OnExit;
				}

				//No Errors, Value is a string
				o_PathShader = lua_tostring(&io_luaState, -1);
			}
			else
			{
				wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "key must be named as string \"Path\" \n";

					*o_errorMessage = errorMessage.str();
				}
#endif
				// Pop the returned key value pair on error
				lua_pop(&io_luaState, 2);
				goto OnExit;
			}

			CountOfValues++;

			//Pop the value, but leave the key
			lua_pop(&io_luaState, 1);
		}

	OnExit:
		return !wereThereErrors;
	}



	//==================================================================
	//Texture load logic
	bool Material::LoadTableValues_Texture(lua_State& io_luaState, const char* key, std::string& o_PathTexture, std::string& o_NameSampler
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(key);
		bool wereThereErrors = false;

		// Right now the asset table is at -1.
		// After the following table operation it will be at -2
		// and the "textures" table will be at -1:
		lua_pushstring(&io_luaState, key);
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
		if (lua_istable(&io_luaState, -1))
		{
			if (!LoadTableValues_TexturePathAndSampler(io_luaState, o_PathTexture, o_NameSampler
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
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "The value at \"" << key << "\" must be a table "
					"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";

				*o_errorMessage = errorMessage.str();
			}
#endif

			goto OnExit;
		}

	OnExit:

		// Pop the shader table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool Material::LoadTableValues_TexturePathAndSampler(lua_State& io_luaState, std::string& o_PathTexture, std::string& o_NameSampler
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
		)
	{
		// Right now the asset table is at -2
		// and the textures table is at -1.
		// NOTE, however, that it doesn't matter to me in this function
		// that the asset table is at -2.
		// Because I've carefully called a new function for every "stack level"
		// The only thing I care about is that the textures table that I care about
		// is at the top of the stack.
		// As long as I make sure that when I leave this function it is _still_
		// at -1 then it doesn't matter to me at all what is on the stack below it.
		bool wereThereErrors = false;

		const int MAX_COUNT_OF_VALUES = 2; //Path and Sampler Name
		int CountOfValues = 0;

		//Iterating through the constant key value pairs
		lua_pushnil(&io_luaState);
		int CurrentIndexOfConstantTable = -2;
		while (lua_next(&io_luaState, CurrentIndexOfConstantTable))
		{
			//Only one path should be present in this table
			assert(CountOfValues <= MAX_COUNT_OF_VALUES);

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
			const char * PathKeyName = lua_tostring(&io_luaState, IndexOfKey);

			if (strcmp(PathKeyName, "Path") == 0)
			{
				// A correct asset file _must_ return a string as value
				if (lua_type(&io_luaState, IndexOfValue) != LUA_TSTRING)
				{
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "file paths must be a string (instead of a " <<
							luaL_typename(&io_luaState, IndexOfValue) << ")\n";

						*o_errorMessage = errorMessage.str();
					}
#endif

					// Pop the returned non-string value
					lua_pop(&io_luaState, 1);
					goto OnExit;
				}

				//No Errors, Value is a string
				o_PathTexture = lua_tostring(&io_luaState, -1);
			}
			else if (strcmp(PathKeyName, "Sampler") == 0)
			{
				// A correct asset file _must_ return a string as value
				if (lua_type(&io_luaState, IndexOfValue) != LUA_TSTRING)
				{
					wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "file paths must be a string (instead of a " <<
							luaL_typename(&io_luaState, IndexOfValue) << ")\n";

						*o_errorMessage = errorMessage.str();
					}
#endif

					// Pop the returned non-string value
					lua_pop(&io_luaState, 1);
					goto OnExit;
				}

				//No Errors, Value is a string
				o_NameSampler = lua_tostring(&io_luaState, -1);
			}
			else
			{
				wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "key must be named as string \"Path\" or \"Sampler\" \n";

					*o_errorMessage = errorMessage.str();
				}
#endif
				// Pop the returned key value pair on error
				lua_pop(&io_luaState, 2);
				goto OnExit;
			}

			CountOfValues++;

			//Pop the value, but leave the key
			lua_pop(&io_luaState, 1);
		}

OnExit:
		if (CountOfValues == 0)
		{
			wereThereErrors = true;
		}

		return !wereThereErrors;
	}

	bool Material::LoadTextureAndSamplerRegister(const char* iTexturePath, const char* iSamplerName, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice && iTexturePath && iSamplerName);

		bool WereThereErrors = false;

		HRESULT result = D3DXCreateTextureFromFile(i_direct3dDevice, iTexturePath, &m_texture);
		
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

		m_samplerRegister = static_cast<DWORD>(m_pfragmentShaderConsts->GetSamplerIndex(samplerHandle));

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


	bool Material::LoadFragmentShader(const char* i_FragmentShaderpath, IDirect3DDevice9 * i_direct3dDevice
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
			m_pfragmentShaderConsts->SetDefaults(i_direct3dDevice); //Set constant default
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


	bool Material::LoadVertexShader(const char* i_VertexShaderpath, IDirect3DDevice9 * i_direct3dDevice
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
			if (!LoadVertexShaderConstants(m_pvertexShaderConsts, i_direct3dDevice))
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					*o_errorMessage = "Failed to load the vertex shader constants";
				}
#endif
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
#if 0
			compiledShader->Release();
#else
			if (compiledShader) free(compiledShader);
#endif
		}

		return !wereThereErrors;
	}

	bool Material::LoadVertexShaderConstants(ID3DXConstantTable* i_pvertexShaderConsts, IDirect3DDevice9 * i_direct3dDevice)
	{
		assert(i_pvertexShaderConsts && i_direct3dDevice);

		i_pvertexShaderConsts->SetDefaults(i_direct3dDevice); //Set constant default

		D3DXMATRIX pMatrixValue;
		D3DXMatrixIdentity(&pMatrixValue);

		//-------------------g_transform_modelToWorld------------
		{
			const char * cConstantName = "g_transform_modelToWorld";

			D3DXHANDLE Handle = i_pvertexShaderConsts->GetConstantByName(NULL, cConstantName);

			if (Handle != NULL)
			{
				const unsigned int dataCount = 1;
				BelongsToenum::BELONGSTO iBelongsTo = BelongsToenum::BELONGSTO::VERTEX_SHADER;
				IsAenum::IsA iIsA = IsAenum::IsA::MATRIX_4X4;

				MaterialConstantData<D3DXMATRIX> *PerInstanceMaterialConstant = new MaterialConstantData<D3DXMATRIX>(cConstantName, &pMatrixValue, dataCount, Handle, iBelongsTo, iIsA);
				assert(PerInstanceMaterialConstant);

				IMaterialConstant * BaseClassPointer = PerInstanceMaterialConstant;

				m_perInstanceConstantDatas.push_back(BaseClassPointer);
			}
		}

		//-----------------------g_transform_worldToView-------------
		{
			const char * cConstantName = "g_transform_worldToView";

			D3DXHANDLE Handle = i_pvertexShaderConsts->GetConstantByName(NULL, cConstantName);

			if (Handle != NULL)
			{
				const unsigned int dataCount = 1;
				BelongsToenum::BELONGSTO iBelongsTo = BelongsToenum::BELONGSTO::VERTEX_SHADER;
				IsAenum::IsA iIsA = IsAenum::IsA::MATRIX_4X4;

				MaterialConstantData<D3DXMATRIX> *PerViewMaterialConstant = new MaterialConstantData<D3DXMATRIX>(cConstantName, &pMatrixValue, dataCount, Handle, iBelongsTo, iIsA);
				assert(PerViewMaterialConstant);

				IMaterialConstant * BaseClassPointer = PerViewMaterialConstant;

				m_perViewConstantDatas.push_back(BaseClassPointer);
			}
		}
		//-----------------------g_transform_viewToScreen-------------
		{
			const char * cConstantName = "g_transform_viewToScreen";

			D3DXHANDLE Handle = i_pvertexShaderConsts->GetConstantByName(NULL, cConstantName);

			if (Handle != NULL)
			{
				const unsigned int dataCount = 1;
				BelongsToenum::BELONGSTO iBelongsTo = BelongsToenum::BELONGSTO::VERTEX_SHADER;
				IsAenum::IsA iIsA = IsAenum::IsA::MATRIX_4X4;

				MaterialConstantData<D3DXMATRIX> *PerViewMaterialConstant = new MaterialConstantData<D3DXMATRIX>(cConstantName, &pMatrixValue, dataCount, Handle, iBelongsTo, iIsA);
				assert(PerViewMaterialConstant);

				IMaterialConstant * BaseClassPointer = PerViewMaterialConstant;

				m_perViewConstantDatas.push_back(BaseClassPointer);
			}
		}
		return true;
	}
}
