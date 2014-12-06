
#include "PreCompiled.h"
#include "ILine.h"
#include "Material.h"
#include "CameraSystem.h"

namespace Engine
{
	const char * ILine::s_VertexShaderPath = "data/vertexShaderSprite.shd";
	const char * ILine::s_FragmentShaderPath = "data/fragmentShaderSprite.shd";
	unsigned int ILine::s_MaxLines = 1;

	ILine::ILine(const char *iName, IDirect3DDevice9 *i_direct3dDevice, unsigned int iMaxLines) :
		m_name(iName),
		m_direct3dDevice(i_direct3dDevice),
		m_vertexShader(NULL),
		m_fragmentShader(NULL)
	{
		s_MaxLines = iMaxLines;
	}

	ILine::~ILine()
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
	}

	bool ILine::LoadFragmentShader(const char* i_FragmentShaderpath, IDirect3DDevice9 * i_direct3dDevice
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

	bool ILine::LoadFragmentShaderConstants(const char* i_FragmentShaderpath, ID3DXConstantTable* i_pfragmentShaderConsts, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_pfragmentShaderConsts && i_direct3dDevice && i_FragmentShaderpath);

		i_pfragmentShaderConsts->SetDefaults(i_direct3dDevice); //Set constant default

		return true;
	}


	bool ILine::LoadVertexShader(const char* i_VertexShaderpath, IDirect3DDevice9 * i_direct3dDevice
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


	bool ILine::LoadVertexShaderConstants(const char* i_VertexShaderpath, ID3DXConstantTable* i_pvertexShaderConsts, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_pvertexShaderConsts && i_direct3dDevice);

		i_pvertexShaderConsts->SetDefaults(i_direct3dDevice); //Set constant default

		D3DXMATRIX pMatrixValue;
		D3DXMatrixIdentity(&pMatrixValue);

		//-------------------g_transform_worldToView------------
		{
			const char * cGlobalConstantName = "g_transform_worldToView";



			m_transform_WorldToView_ConstHandle = i_pvertexShaderConsts->GetConstantByName(NULL, cGlobalConstantName);

			if (m_transform_WorldToView_ConstHandle == NULL)
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not find constant " << cGlobalConstantName << " in " << i_VertexShaderpath << " vertex shader";
					*o_errorMessage = errorMessage.str();
				}

				return false;
#endif
			}
		}


		//-----------------------g_transform_viewToScreen-------------
		{
			const char * cGlobalConstantName = "g_transform_viewToScreen";

			m_transform_ViewToScreen_ConstHandle = i_pvertexShaderConsts->GetConstantByName(NULL, cGlobalConstantName);

			if (m_transform_ViewToScreen_ConstHandle == NULL)
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not find constant " << cGlobalConstantName << " in " << i_VertexShaderpath << " vertex shader";
					*o_errorMessage = errorMessage.str();
				}

				return false;
#endif
			}
		}

		return true;
	}

	HRESULT ILine::Set(IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
		, std::string* o_errorMessage
#endif
		)
	{
		assert(i_direct3dDevice && m_vertexShader && m_fragmentShader);

		HRESULT result = D3D_OK;
#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
		std::wstringstream EventMessage;
		EventMessage << "Set Line " << (m_name.c_str());
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

		result = m_pvertexShaderConsts->SetMatrix(m_direct3dDevice, m_transform_WorldToView_ConstHandle, &CameraSystem::GetInstance()->GetWorldToView());
		assert(SUCCEEDED(result));

		if (FAILED(result))
		{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				*o_errorMessage = "Setting Vertex Shader constant WorldToView failed";
			}
#endif
			return result;
		}

		result = m_pvertexShaderConsts->SetMatrix(m_direct3dDevice, m_transform_ViewToScreen_ConstHandle, &CameraSystem::GetInstance()->GetViewToScreen());
		assert(SUCCEEDED(result));

		if (FAILED(result))
		{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			if (o_errorMessage)
			{
				*o_errorMessage = "Setting Vertex Shader constant GetViewToScreen failed";
			}
#endif
			return result;
		}

#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
		D3DPERF_EndEvent();
#endif
		return result;
	}

	bool ILine::CreateVertexBufferForLine(const std::vector<sLine> &i_LineInfo)
	{
		assert(i_LineInfo.size() <= s_MaxLines); //Line vertex data should be less than the max lines allowed / vertex buffer size

		IDirect3DVertexDeclaration9* vertexDeclaration = NULL;

		DWORD usage = 0;
		// The usage tells Direct3D how this vertex buffer will be used
		{
			usage |= D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			{
				D3DDEVICE_CREATION_PARAMETERS deviceCreationParameters;
				const HRESULT result = m_direct3dDevice->GetCreationParameters(&deviceCreationParameters);
				if (SUCCEEDED(result))
				{
					DWORD vertexProcessingType = deviceCreationParameters.BehaviorFlags &
						(D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING);
					usage |= (vertexProcessingType != D3DCREATE_SOFTWARE_VERTEXPROCESSING) ? 0 : D3DUSAGE_SOFTWAREPROCESSING;
				}
				else
				{
					MessageBox(NULL, "DirectX failed to get device creation parameters", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}
		}

		// Create a vertex buffer
			{
				// We are drawing a mesh
				const unsigned int vertexCount = s_MaxLines;
				const unsigned int bufferSize = vertexCount * sizeof(sVertexData);

				// We will define our own vertex format
				const DWORD useSeparateVertexDeclaration = 0;
				// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
				const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
				HANDLE* const notUsed = NULL;

				HRESULT result = m_direct3dDevice->CreateVertexBuffer(bufferSize, usage, useSeparateVertexDeclaration, useDefaultPool,
					&m_vertexBuffer, notUsed);
				if (FAILED(result))
				{
					MessageBox(NULL, "DirectX failed to create a vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}

			FillVertexBuffer(i_LineInfo);

			return true;
	}

	bool ILine::FillVertexBuffer(const std::vector<sLine> &i_LineInfo)
	{
		assert(i_LineInfo.size() <= s_MaxLines);
		assert(m_vertexBuffer);
		// Fill the vertex buffer with the Mesh's vertices
		{
			// Before the vertex buffer can be changed it must be "locked"
			sVertexData* vertexData;
			{
				const unsigned int lockEntireBuffer = 0;
				const DWORD useDefaultLockingBehavior = 0;
				HRESULT result = (m_vertexBuffer)->Lock(lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>(&vertexData), useDefaultLockingBehavior);
				if (FAILED(result))
				{
					MessageBox(NULL, "DirectX failed to lock the vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}

			const unsigned int vertexCount = s_MaxLines;
			const unsigned int bufferSize = vertexCount * sizeof(sVertexData);

			memset(vertexData, 0, bufferSize * vertexCount);

			// Fill the buffer
			{
				for (unsigned int i = 0; i < i_LineInfo.size(); i++)
				{
					vertexData[i].color = i_LineInfo[i].mColor;
					vertexData[i].x = i_LineInfo[i].mBegin.x();
					vertexData[i].y = i_LineInfo[i].mBegin.y();
					vertexData[i].z = i_LineInfo[i].mBegin.z();
				}
			}

			// The buffer must be "unlocked" before it can be used
			{
				HRESULT result = (m_vertexBuffer)->Unlock();
				if (FAILED(result))
				{
					MessageBox(NULL, "DirectX failed to unlock the vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}

			return true;
		}
	}
}