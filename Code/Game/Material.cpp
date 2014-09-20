
#include "PreCompiled.h"
#include <d3dx9shader.h>
#include "Material.h"

namespace Engine
{

	Material::Material():
		m_vertexShader(NULL),
		m_fragmentShader(NULL)
	{

	}

	Material::~Material()
	{
		if (m_vertexShader)
		{
			m_vertexShader->Release();
			m_vertexShader = NULL;
		}

		if (m_fragmentShader)
		{
			m_fragmentShader->Release();
			m_fragmentShader = NULL;
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


	HRESULT Material::Load(const char* i_MaterialFilepath, IDirect3DDevice9* i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice && i_MaterialFilepath);
		
		//Temp
		std::string VertexShaderPath = "data/vertexShader.hlsl";
		std::string FragmentShaderPath = "data/fragmentShader.hlsl";

		if (!LoadFragmentShader(FragmentShaderPath.c_str(), i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif		
			))
		{
			return false;
		}

		if (!LoadVertexShader(VertexShaderPath.c_str(), i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif		
			))
		{
			return false;
		}

		return true;
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
			ID3DXConstantTable** noConstants = NULL;
			HRESULT result = D3DXCompileShaderFromFile(sourceCodeFileName, noMacros, noIncludes, entryPoint, profile, noFlags,
														&compiledShader, &errorMessages, noConstants);
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

		// Create the fragment shader object
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
			ID3DXConstantTable** noConstants = NULL;
			HRESULT result = D3DXCompileShaderFromFile(sourceCodeFileName, noMacros, noIncludes, entryPoint, profile, noFlags,
														&compiledShader, &errorMessages, noConstants);
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
