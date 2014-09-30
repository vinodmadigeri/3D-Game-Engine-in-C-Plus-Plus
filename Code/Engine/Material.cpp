
#include "PreCompiled.h"
#include <d3dx9shader.h>
#include "Material.h"


namespace Engine
{
	Material::Material(IDirect3DDevice9 * i_direct3dDevice) :
		m_direct3dDevice(i_direct3dDevice),
		m_vertexShader(NULL),
		m_fragmentShader(NULL),
		m_pvertexShaderConsts(NULL),
		m_pfragmentShaderConsts(NULL)

	{
		assert(m_direct3dDevice);
	}

	Material::~Material()
	{
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
	}

	HRESULT Material::Set(IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice);

		HRESULT result = D3D_OK;

		// Set the shaders
		{
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
		}

		return result;
	}

	bool Material::LoadMaterialLuaAsset(const char* i_path
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_path);
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
			const int luaResult = luaL_loadfile(luaState, i_path);
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
										<<	luaL_typename(luaState, -1) << ")\n";

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

	bool Material::LoadTableValues(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		if (!LoadConstantDataTable(io_luaState, "Constants"
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			return false;
		}


		if (!LoadTableValues_Shaders(io_luaState, "VertexShader", mPathVertexShader
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			return false;
		}

		if (!LoadTableValues_Shaders(io_luaState, "FragmentShader", mPathFragmentShader
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
			MaterialConstantData ThisData;
			ThisData.ConstantName = lua_tostring(&io_luaState, IndexOfKey);

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

			if (!LoadEachConstantDataValue(io_luaState, ThisData.DefaultValues
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
			
			//At this point both key and value pairs are stored in ThisData,
			//Store it in the vector
			m_perMaterialConstantDatas.push_back(ThisData);

			//Pop the value, but leave the key
			lua_pop(&io_luaState, 1);
		}


	OnExit:

		return !wereThereErrors;
	}

	bool Material::LoadEachConstantDataValue(lua_State& io_luaState, std::vector<float> & DataVector
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		// Right now the asset table is at -2
		// and the constant table is at -1.
		// NOTE, however, that it doesn't matter to me in this function
		// that the asset table is at -2.
		// Because I've carefully called a new function for every "stack level"
		// The only thing I care about is that the textures table that I care about
		// is at the top of the stack.
		// As long as I make sure that when I leave this function it is _still_
		// at -1 then it doesn't matter to me at all what is on the stack below it.

		//Iterating through every table values
		const int DataCount = luaL_len(&io_luaState, -1);
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

			DataVector.push_back(static_cast<float>(lua_tonumber(&io_luaState, -1)));
			//Pop the value from the stack since it is stored
			lua_pop(&io_luaState, 1);
		}

		return true;
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

		const int ShaderPathCount = luaL_len(&io_luaState, -1);

		if (ShaderPathCount == 1)
		{
			lua_pushinteger(&io_luaState, ShaderPathCount);
			const int currentIndexOfTable = -2;
			lua_gettable(&io_luaState, currentIndexOfTable);

			// A correct asset file _must_ return a table
			if (lua_type(&io_luaState, -1) != LUA_TSTRING)
			{
				wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "file paths must be a string (instead of a " <<
									luaL_typename(&io_luaState, -1) << ")\n";

					*o_errorMessage = errorMessage.str();
				}
#endif

				// Pop the returned non-table value
				lua_pop(&io_luaState, 1);
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				*o_errorMessage = "Only one return value for shader path should be returned";
			}
#endif
			lua_pop(&io_luaState, ShaderPathCount);
			goto OnExit;
		}

		//No Errors, Value is a string
		o_PathShader = lua_tostring(&io_luaState, -1);
		lua_pop(&io_luaState, ShaderPathCount);

	OnExit:
		return !wereThereErrors;
	}



	bool Material::Load(const char* i_MaterialFilepath, IDirect3DDevice9* i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice && i_MaterialFilepath);
		

		//Lua Logic
		//---------
		if (!LoadMaterialLuaAsset(i_MaterialFilepath
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif		
			))
		{
			return false;
		}

		if (!LoadFragmentShader(mPathFragmentShader.c_str(), i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif		
			))
		{
			return false;
		}

		if (!LoadVertexShader(mPathVertexShader.c_str(), i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif		
			))
		{
			return false;
		}

		if (!InitilizeConstantDataFromMaterialFile(i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif		
			))
		{
			return false;
		}

		return true;
	}

	bool Material::InitilizeConstantDataFromMaterialFile(IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif		
		)
	{
		m_pfragmentShaderConsts->SetDefaults(i_direct3dDevice);
		m_pvertexShaderConsts->SetDefaults(i_direct3dDevice);

		for (unsigned int i = 0; i < m_perMaterialConstantDatas.size(); i++)
		{
			D3DXHANDLE ConstHandle = m_pvertexShaderConsts->GetConstantByName(NULL, m_perMaterialConstantDatas.at(i).ConstantName.c_str());
			m_perMaterialConstantDatas.at(i).eBelongsTo = MaterialConstantData::BelongsToenum::VERTEX_SHADER;
				
			if (ConstHandle == NULL)
			{
				ConstHandle = m_pfragmentShaderConsts->GetConstantByName(NULL, m_perMaterialConstantDatas.at(i).ConstantName.c_str());
				m_perMaterialConstantDatas.at(i).eBelongsTo = MaterialConstantData::BelongsToenum::FRAGMENT_SHADER;
			}

			if (ConstHandle == NULL)
			{
				m_perMaterialConstantDatas.at(i).eBelongsTo = MaterialConstantData::BelongsToenum::NONE;
			}

			m_perMaterialConstantDatas.at(i).Handle = ConstHandle;
		}

		if (!SetPerMaterialConstantDataFromMaterialFile(i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
			assert(false);

			return false;
		}

		return true;;
	}

	bool Material::SetPerMaterialConstantDataFromMaterialFile(IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif		
		)
	{
		bool WereThereErrors = false;

		for (unsigned int i = 0; i < m_perMaterialConstantDatas.size(); i++)
		{
			ID3DXConstantTable* ThisConstTable;
			D3DXHANDLE ThisShaderConstHandle;

			if (m_perMaterialConstantDatas.at(i).eBelongsTo == MaterialConstantData::BelongsToenum::FRAGMENT_SHADER)
			{
				ThisConstTable = m_pfragmentShaderConsts;
			}
			else if (m_perMaterialConstantDatas.at(i).eBelongsTo == MaterialConstantData::BelongsToenum::VERTEX_SHADER)
			{
				ThisConstTable = m_pvertexShaderConsts;
			}
			else
			{
				continue;
			}

			ThisShaderConstHandle = ThisConstTable->GetConstantByName(NULL, m_perMaterialConstantDatas.at(i).ConstantName.c_str());

			if (m_perMaterialConstantDatas.at(i).DefaultValues.size() > 0)
			{
				int count = m_perMaterialConstantDatas.at(i).DefaultValues.size();
				float *pfvalue = new float[count];

				for (int j = 0; j < count; j++)
				{
					pfvalue[j] = m_perMaterialConstantDatas.at(i).DefaultValues.at(j);
				}

				HRESULT result = ThisConstTable->SetFloatArray(m_direct3dDevice, ThisShaderConstHandle, pfvalue, count);

				if (FAILED(result))
				{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Setting Constant table valye failed with error: " << HRESULT_CODE(result) << "\n";

						*o_errorMessage = errorMessage.str();
					}
#endif
					WereThereErrors = true;
					assert(false);
				}

				delete pfvalue;
			}
		}

		return !WereThereErrors;
	}

	bool Material::SetPerInstanceConstantDataByName(const std::string &i_name, std::vector<float> &i_Value)
	{
		bool WereThereErrors = false;
		
		if (i_Value.size() == 0)
		{
			return true;
		}

		for (unsigned int i = 0; i < m_perInstanceConstantDatas.size(); i++)
		{

			if (m_perInstanceConstantDatas.at(i).ConstantName != i_name)
			{
				continue;
			}

			assert(m_perInstanceConstantDatas.at(i).DefaultValues.size() == i_Value.size());

			ID3DXConstantTable* ThisConstTable;
			D3DXHANDLE ThisShaderConstHandle;

			if (m_perInstanceConstantDatas.at(i).eBelongsTo == MaterialConstantData::BelongsToenum::FRAGMENT_SHADER)
			{
				ThisConstTable = m_pfragmentShaderConsts;
			}
			else if (m_perInstanceConstantDatas.at(i).eBelongsTo == MaterialConstantData::BelongsToenum::VERTEX_SHADER)
			{
				ThisConstTable = m_pvertexShaderConsts;
			}
			else
			{
				continue;
			}

			ThisShaderConstHandle = ThisConstTable->GetConstantByName(NULL, m_perInstanceConstantDatas.at(i).ConstantName.c_str());

			int count = i_Value.size();
			float *pfvalue = new float[count];

			for (int j = 0; j < count; j++)
			{
				pfvalue[j] = i_Value.at(j);
			}

			HRESULT result = ThisConstTable->SetFloatArray(m_direct3dDevice, ThisShaderConstHandle, pfvalue, count);

			if (FAILED(result))
			{
				WereThereErrors = true;

				WereThereErrors = true;
				assert(false);
			}

			delete pfvalue;
		}

		return !WereThereErrors;
	}

	bool Material::LoadFragmentShader(const char* i_FragmentShaderpath, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice && i_FragmentShaderpath);
		
		// Load the source code from file and compile it
		ID3DXBuffer* compiledShader;
		{
			const char* sourceCodeFileName = i_FragmentShaderpath;
			const D3DXMACRO* noMacros = NULL;
			ID3DXInclude* noIncludes = NULL;
			const char* entryPoint = "main";
			const char* profile = "ps_3_0";
			const DWORD noFlags = 0;
			ID3DXBuffer* errorMessages = NULL;
			
			HRESULT result = D3DXCompileShaderFromFile(sourceCodeFileName, noMacros, noIncludes, entryPoint, profile, noFlags,
				&compiledShader, &errorMessages, &m_pfragmentShaderConsts);
			if (SUCCEEDED(result))
			{
				if (errorMessages)
				{
					errorMessages->Release();
				}
			}
			else
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage && errorMessages)
				{
					*o_errorMessage = std::string("DirectX failed to compile the fragment shader from the file ") +
						sourceCodeFileName + ":\n" +
						reinterpret_cast<char*>(errorMessages->GetBufferPointer());
					errorMessages->Release();
				}
				else if (o_errorMessage)
				{
					*o_errorMessage = "DirectX failed to compile the fragment shader from the file ";
					*o_errorMessage += sourceCodeFileName;
				}
#endif
				return false;
			}
		}

		bool wereThereErrors = false;
		{
			HRESULT result = i_direct3dDevice->CreatePixelShader(reinterpret_cast<DWORD*>(compiledShader->GetBufferPointer()),
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

			compiledShader->Release();
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

		// Load the source code from file and compile it
		ID3DXBuffer* compiledShader;
		{
			const char* sourceCodeFileName = i_VertexShaderpath;
			const D3DXMACRO* noMacros = NULL;
			ID3DXInclude* noIncludes = NULL;
			const char* entryPoint = "main";
			const char* profile = "vs_3_0";
			const DWORD noFlags = 0;
			ID3DXBuffer* errorMessages = NULL;
			HRESULT result = D3DXCompileShaderFromFile(sourceCodeFileName, noMacros, noIncludes, entryPoint, profile, noFlags,
														&compiledShader, &errorMessages, &m_pvertexShaderConsts);
			if (SUCCEEDED(result))
			{
				if (errorMessages)
				{
					errorMessages->Release();
				}
			}
			else
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage && errorMessages)
				{
					*o_errorMessage = std::string("DirectX failed to compile the vertex shader from the file ") +
						sourceCodeFileName + ":\n" + reinterpret_cast<char*>(errorMessages->GetBufferPointer());
					
					errorMessages->Release();
				}
				else if (o_errorMessage)
				{
					*o_errorMessage = "DirectX failed to compile the vertex shader from the file ";
					*o_errorMessage += sourceCodeFileName;
				}
#endif
				return false;
			}
		}

		//Get reference to per-instance constant
		if (m_pvertexShaderConsts != NULL)
		{
			MaterialConstantData PerInstanceMaterialConstant;

			PerInstanceMaterialConstant.ConstantName = "g_meshPosition_screen";
			
			//Two float values for position, change it to 3 for 3D
			PerInstanceMaterialConstant.DefaultValues.push_back(0.0f);
			PerInstanceMaterialConstant.DefaultValues.push_back(0.0f);

			PerInstanceMaterialConstant.eBelongsTo = MaterialConstantData::BelongsToenum::VERTEX_SHADER;
			PerInstanceMaterialConstant.Handle = m_pvertexShaderConsts->GetConstantByName(NULL, PerInstanceMaterialConstant.ConstantName.c_str());

			if (PerInstanceMaterialConstant.Handle != NULL)
			{
				m_perInstanceConstantDatas.push_back(PerInstanceMaterialConstant);
			}
		}

		// Create the vertex shader object
		bool wereThereErrors = false;
		{
			HRESULT result = i_direct3dDevice->CreateVertexShader(reinterpret_cast<DWORD*>(compiledShader->GetBufferPointer()),
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

			compiledShader->Release();
		}

		return !wereThereErrors;
	}

}
