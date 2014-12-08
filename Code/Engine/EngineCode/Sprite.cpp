#include "PreCompiled.h"
#include "WindowsError.h"
#include "Sprite.h"
#include "Material.h"


namespace Engine
{
	const char * Sprite::s_VertexShaderPath = "data/vertexShaderSprite.shd";
	const char * Sprite::s_FragmentShaderPath = "data/fragmentShaderSprite.shd";
	const char* Sprite::s_SamplerName = "g_color_sampler";

	Sprite::Sprite(const char *iTextureName, IDirect3DDevice9 *i_direct3dDevice, IDirect3DVertexBuffer9* i_vertexBuffer) :
		m_name(iTextureName),
		m_direct3dDevice(i_direct3dDevice),
		m_vertexShader(NULL),
		m_fragmentShader(NULL),
		m_pfragmentShaderConsts(NULL),
		m_texture(NULL),
		m_samplerRegister(NULL),
		m_vertexBuffer(i_vertexBuffer)
	{
		assert(i_vertexBuffer);
	}

	Sprite::~Sprite()
	{
		if (m_vertexBuffer)
		{
			m_vertexBuffer->Release();
			m_vertexBuffer = NULL;
		}

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

	bool Sprite::Load(const char* i_TexturePath, const sRectangle &i_texcoords, const sSprite &i_spriteDetails, IDirect3DDevice9* i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_TexturePath && s_FragmentShaderPath && s_VertexShaderPath && s_SamplerName && i_direct3dDevice);

		//Load the shader from the path
		if (!LoadFragmentShader(s_FragmentShaderPath, m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif		
			))
		{
			return false;
		}

		//Load the shader from the path
		if (!LoadVertexShader(s_VertexShaderPath, m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif		
			))
		{
			return false;
		}

		if (!LoadTextureAndSamplerRegister(i_TexturePath, s_SamplerName, m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, o_errorMessage
#endif
			))
		{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			
#endif
			return false;
		}

		if (!CreateSpriteDrawInfoandFillVertexBuffer(i_spriteDetails, i_texcoords))
		{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED

#endif
			return false;
		}

		return true;
	}

	bool Sprite::CreateSpriteDrawInfoandFillVertexBuffer(const sSprite &i_spriteDetails, const sRectangle &i_texcoords)
	{
		//Once we have texture we can go ahead and create the SpriteDrawInfo
		D3DSURFACE_DESC surfaceDescription;
		assert(m_texture);
		HRESULT result = m_texture->GetLevelDesc(0, &surfaceDescription);
		float TextureOriginalWidth = static_cast<float>(surfaceDescription.Width) / i_spriteDetails.horizontalSpriteCount;
		float TextureOriginalHeight = static_cast<float>(surfaceDescription.Height) / i_spriteDetails.verticalSpriteCount;

		m_spriteDrawInfo = SpriteDrawInfo(i_spriteDetails, i_texcoords, TextureOriginalWidth, TextureOriginalHeight);

		return FillVertexBuffer();
	}

	bool Sprite::LoadFragmentShader(const char* i_FragmentShaderpath, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice && i_FragmentShaderpath);

		// Load the compiled source file it
		void * compiledShader;
		{
			if (!Material::LoadAndAllocateShaderProgram(i_FragmentShaderpath, compiledShader
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
			if (!LoadFragmentShaderConstants(i_FragmentShaderpath, m_pfragmentShaderConsts, i_direct3dDevice
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
			HRESULT result = i_direct3dDevice->CreatePixelShader(reinterpret_cast<DWORD*>(compiledShader), &m_fragmentShader);
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

	bool Sprite::LoadFragmentShaderConstants(const char* i_FragmentShaderpath, ID3DXConstantTable* i_pfragmentShaderConsts, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_pfragmentShaderConsts && i_direct3dDevice && i_FragmentShaderpath);

		i_pfragmentShaderConsts->SetDefaults(i_direct3dDevice); //Set constant default

		return true;
	}


	bool Sprite::LoadVertexShader(const char* i_VertexShaderpath, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice && i_VertexShaderpath);

		// Load the compiled source file it
		void * compiledShader;
		{
			if (!Material::LoadAndAllocateShaderProgram(i_VertexShaderpath, compiledShader
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif
				))
			{

				return false;
			}
		}

#if 0
		D3DXGetShaderConstantTable(reinterpret_cast<DWORD*>(compiledShader), &m_pvertexShaderConsts);

		//Get reference to per-instance constant
		if (m_pvertexShaderConsts != NULL)
		{
			if (!LoadVertexShaderConstants(i_VertexShaderpath, m_pvertexShaderConsts, i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, o_errorMessage
#endif
				))
			{
				return false;
			}
		}
#endif

		// Create the vertex shader object
		bool wereThereErrors = false;
		{
			HRESULT result = i_direct3dDevice->CreateVertexShader(reinterpret_cast<DWORD*>(compiledShader), &m_vertexShader);
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


	bool Sprite::LoadTextureAndSamplerRegister(const char* iTexturePath, const char* iSamplerName, IDirect3DDevice9 * i_direct3dDevice
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

	HRESULT Sprite::Set(IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice && m_vertexShader && m_fragmentShader && m_texture);

		HRESULT result = D3D_OK;
#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
		std::wstringstream EventMessage;
		EventMessage << "Set Sprite " << (m_name.c_str());
		D3DPERF_BeginEvent(0, EventMessage.str().c_str());
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

	bool Sprite::FillVertexBuffer()
	{
		assert(m_vertexBuffer);
		
		HRESULT result;
		// Fill the vertex buffer with the Mesh's vertices
		{
			// Before the vertex buffer can be changed it must be "locked"
			sVertexData* vertexData;
			{
				const unsigned int lockEntireBuffer = 0;
				const DWORD useDefaultLockingBehavior = 0;
				result = m_vertexBuffer->Lock(lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>(&vertexData), useDefaultLockingBehavior);
				if (FAILED(result))
				{
					MessageBox(NULL, "DirectX failed to lock the vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}

			// Fill the buffer
			{
				memcpy(vertexData, m_spriteDrawInfo.m_pVerticesData, m_spriteDrawInfo.m_VertexStride * m_spriteDrawInfo.m_NumOfVertices);
			}

			// The buffer must be "unlocked" before it can be used
			{
				result = m_vertexBuffer->Unlock();
				if (FAILED(result))
				{
					MessageBox(NULL, "DirectX failed to unlock the vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}
		}

		return true;
	}

	bool Sprite::FillSpriteSheet(unsigned int i_HorizontalCount, unsigned int i_VerticalCount)
	{
		assert(m_vertexBuffer && (i_HorizontalCount >= 0) && (i_VerticalCount >= 0) && (i_HorizontalCount < m_spriteDrawInfo.m_MaxHorizontalCount)
			&& (i_VerticalCount < m_spriteDrawInfo.m_MaxVerticalCount));

		// Fill the vertex buffer with the Mesh's vertices
		{
			// Before the vertex buffer can be changed it must be "locked"
			sVertexData* vertexData;
			{
				const unsigned int lockEntireBuffer = 0;
				const DWORD useDefaultLockingBehavior = 0;
				HRESULT result = m_vertexBuffer->Lock(lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>(&vertexData), useDefaultLockingBehavior);
				if (FAILED(result))
				{
					MessageBox(NULL, "DirectX failed to lock the vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}

			// Fill the buffer
			{
				//memcpy(vertexData, i_SpriteDrawInfo.m_pVerticesData, i_SpriteDrawInfo.m_VertexStride * i_SpriteDrawInfo.m_NumOfVertices);
				vertexData[0].U = m_spriteDrawInfo.m_pVerticesData[0].U + static_cast<float>(i_HorizontalCount) / static_cast<float>(m_spriteDrawInfo.m_MaxHorizontalCount);
				vertexData[0].V = m_spriteDrawInfo.m_pVerticesData[0].V + static_cast<float>(i_VerticalCount) / static_cast<float>(m_spriteDrawInfo.m_MaxVerticalCount);

				vertexData[1].U = m_spriteDrawInfo.m_pVerticesData[1].U + static_cast<float>(i_HorizontalCount) / static_cast<float>(m_spriteDrawInfo.m_MaxHorizontalCount);
				vertexData[1].V = m_spriteDrawInfo.m_pVerticesData[1].V + static_cast<float>(i_VerticalCount) / static_cast<float>(m_spriteDrawInfo.m_MaxVerticalCount);

				vertexData[2].U = m_spriteDrawInfo.m_pVerticesData[2].U + static_cast<float>(i_HorizontalCount) / static_cast<float>(m_spriteDrawInfo.m_MaxHorizontalCount);
				vertexData[2].V = m_spriteDrawInfo.m_pVerticesData[2].V + static_cast<float>(i_VerticalCount) / static_cast<float>(m_spriteDrawInfo.m_MaxVerticalCount);

				vertexData[3].U = m_spriteDrawInfo.m_pVerticesData[3].U + static_cast<float>(i_HorizontalCount) / static_cast<float>(m_spriteDrawInfo.m_MaxHorizontalCount);
				vertexData[3].V = m_spriteDrawInfo.m_pVerticesData[3].V + static_cast<float>(i_VerticalCount) / static_cast<float>(m_spriteDrawInfo.m_MaxVerticalCount);
			}

			// The buffer must be "unlocked" before it can be used
			{
				HRESULT result = m_vertexBuffer->Unlock();
				if (FAILED(result))
				{
					MessageBox(NULL, "DirectX failed to unlock the vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}
		}

		return true;
	}
}