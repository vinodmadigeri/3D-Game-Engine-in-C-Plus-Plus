// Header Files
//=============

#include "PreCompiled.h"
#include "GraphicsSystem.h"

#include <d3dx9shader.h>

// Static Data Initialization
//===========================
namespace
{
	D3DVERTEXELEMENT9 s_vertexElements[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END()
	};
}

GraphicsSystem *GraphicsSystem ::m_pInstance = NULL;

// Interface
//==========

GraphicsSystem::GraphicsSystem(const HWND i_mainWindow, const std::string &i_VertexShaderPath, const std::string &i_FragmentShaderPath) :
	mInitilized(false),
	m_mainWindow(i_mainWindow),
	m_VertexShaderFilePath(i_VertexShaderPath),
	m_FragmentShaderFilePath(i_FragmentShaderPath),
	m_direct3dInterface(NULL),
	m_direct3dDevice(NULL),
	m_vertexDeclaration(NULL),
	m_vertexBuffer(NULL),
	m_vertexShader(NULL),
	m_fragmentShader(NULL)
{
	if (true == Initialize())
	{
		mInitilized = true;
	}
	else
	{
		assert(false);
	}
}

GraphicsSystem::~GraphicsSystem()
{
	ShutDown();
}

//Creates only one instance
GraphicsSystem * GraphicsSystem::CreateInstance(const HWND i_mainWindow, const std::string &i_VertexShaderPath, const std::string &i_FragmentShaderPath)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new GraphicsSystem(i_mainWindow, i_VertexShaderPath, i_FragmentShaderPath);
	}

	return m_pInstance;
}

GraphicsSystem * GraphicsSystem::GetInstance()
{
	assert((m_pInstance != NULL) && (m_pInstance->mInitilized == true));

	return m_pInstance;
}

void GraphicsSystem::Destroy(void)
{
	if (m_pInstance)
		delete m_pInstance;

	m_pInstance = NULL;
}

bool GraphicsSystem::Initialize(void)
{
	// Initialize the interface to the Direct3D9 library
	if (!CreateInterface(m_mainWindow))
	{
		return false;
	}
	// Create an interface to a Direct3D device
	if (!CreateDevice(m_mainWindow))
	{
		goto OnError;
	}

	if ( !CreateVertexBuffer() )
	{
		goto OnError;
	}
	if ( !LoadVertexShader() )
	{
		goto OnError;
	}
	if ( !LoadFragmentShader() )
	{
		goto OnError;
	}

	return true;

OnError:

	ShutDown();
	return false;
}

void GraphicsSystem::Render()
{
	assert(mInitilized == true);

	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
	{
		const D3DRECT* subRectanglesToClear = NULL;
		const DWORD subRectangleCount = 0;
		const DWORD clearTheRenderTarget = D3DCLEAR_TARGET;
		D3DCOLOR clearColor;
		{
			// Black is usually used:
			clearColor = D3DCOLOR_XRGB( 0, 0, 0 );
		}
		const float noZBuffer = 0.0f;
		const DWORD noStencilBuffer = 0;
		HRESULT result = m_direct3dDevice->Clear( subRectangleCount, subRectanglesToClear,
			clearTheRenderTarget, clearColor, noZBuffer, noStencilBuffer );
		assert( SUCCEEDED( result ) );
	}

	// The actual function calls that draw geometry must be made between paired calls to
	// BeginScene() and EndScene()
	{
		HRESULT result = m_direct3dDevice->BeginScene();
		assert( SUCCEEDED( result ) );
		{
			// Set the shaders
			{
				HRESULT result = m_direct3dDevice->SetVertexShader(m_vertexShader);
				assert( SUCCEEDED( result ) );
				result = m_direct3dDevice->SetPixelShader(m_fragmentShader);
				assert( SUCCEEDED( result ) );
			}
			// Bind a specific vertex buffer to the device as a data source
			{
				// There can be multiple streams of data feeding the display adaptor at the same time
				const unsigned int streamIndex = 0;
				// It's possible to start streaming data in the middle of a vertex buffer
				const unsigned int bufferOffset = 0;
				// The "stride" defines how large a single vertex is in the stream of data
				const unsigned int bufferStride = sizeof( sVertex );
				result = m_direct3dDevice->SetStreamSource(streamIndex, m_vertexBuffer, bufferOffset, bufferStride);
				assert( SUCCEEDED( result ) );
			}
			// Render objects from the current streams
			{
				// We are using triangles as the "primitive" type,
				// and we have defined the vertex buffer as a triangle list
				// (meaning that every triangle is defined by three vertices)
				const D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST;
				// It's possible to start rendering primitives in the middle of the stream
				const unsigned int indexOfFirstVertexToRender = 0;
				// We are currently only rendering a single triangle
				const unsigned int primitiveCountToRender = 1;
				result = m_direct3dDevice->DrawPrimitive(primitiveType, indexOfFirstVertexToRender, primitiveCountToRender);
				assert( SUCCEEDED( result ) );
			}
		}
		result = m_direct3dDevice->EndScene();
		assert( SUCCEEDED( result ) );
	}

	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it, the contents of the back buffer must be "presented"
	// (to the front buffer)
	{
		const RECT* noSourceRectangle = NULL;
		const RECT* noDestinationRectangle = NULL;
		const HWND useDefaultWindow = NULL;
		const RGNDATA* noDirtyRegion = NULL;
		HRESULT result = m_direct3dDevice->Present(noSourceRectangle, noDestinationRectangle, useDefaultWindow, noDirtyRegion);
		assert( SUCCEEDED( result ) );
	}
}

bool GraphicsSystem::ShutDown()
{
	bool wereThereErrors = false;

	if (m_direct3dInterface)
	{
		if ( m_direct3dDevice )
		{
			if ( m_vertexShader )
			{
				m_vertexShader->Release();
				m_vertexShader = NULL;
			}
			if ( m_fragmentShader )
			{
				m_fragmentShader->Release();
				m_fragmentShader = NULL;
			}

			if ( m_vertexBuffer )
			{
				m_vertexBuffer->Release();
				m_vertexBuffer = NULL;
			}
			if ( m_vertexDeclaration )
			{
				m_direct3dDevice->SetVertexDeclaration( NULL );
				m_vertexDeclaration->Release();
				m_vertexDeclaration = NULL;
			}

			m_direct3dDevice->Release();
			m_direct3dDevice = NULL;
		}

		m_direct3dInterface->Release();
		m_direct3dInterface = NULL;
	}

	m_mainWindow = NULL;

	return !wereThereErrors;
}

// Helper Functions
//=================

// Initialize
//-----------

bool GraphicsSystem::CreateDevice(const HWND i_mainWindow)
{
	const UINT useDefaultDevice = D3DADAPTER_DEFAULT;
	const D3DDEVTYPE useHardwareRendering = D3DDEVTYPE_HAL;
	const DWORD useHardwareVertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	D3DPRESENT_PARAMETERS presentationParameters = { 0 };
	{
		presentationParameters.BackBufferWidth = g_windowWidth;
		presentationParameters.BackBufferHeight = g_windowHeight;
		presentationParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
		presentationParameters.BackBufferCount = 1;
		presentationParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
		presentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		presentationParameters.hDeviceWindow = i_mainWindow;
		presentationParameters.Windowed = g_shouldRenderFullScreen ? FALSE : TRUE;
		presentationParameters.EnableAutoDepthStencil = FALSE;
		presentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	}
	HRESULT result = m_direct3dInterface->CreateDevice( useDefaultDevice, useHardwareRendering,
		i_mainWindow, useHardwareVertexProcessing, &presentationParameters, &m_direct3dDevice );
	if ( SUCCEEDED( result ) )
	{
		return true;
	}
	else
	{
		MessageBox( i_mainWindow, "DirectX failed to create a Direct3D9 device", "No D3D9 Device", MB_OK | MB_ICONERROR );
		return false;
	}
}

bool GraphicsSystem::CreateInterface(const HWND i_mainWindow)
{
	// D3D_SDK_VERSION is #defined by the Direct3D header files,
	// and is just a way to make sure that everything is up-to-date
	m_direct3dInterface = Direct3DCreate9( D3D_SDK_VERSION );
	if ( m_direct3dInterface )
	{
		return true;
	}
	else
	{
		MessageBox( i_mainWindow, "DirectX failed to create a Direct3D9 interface", "No D3D9 Interface", MB_OK | MB_ICONERROR );
		return false;
	}
}

bool GraphicsSystem::CreateVertexBuffer()
{
	// Initialize the vertex format
	HRESULT result = m_direct3dDevice->CreateVertexDeclaration( s_vertexElements, &m_vertexDeclaration );
	if ( SUCCEEDED( result ) )
	{
		result = m_direct3dDevice->SetVertexDeclaration( m_vertexDeclaration );
		if ( FAILED( result ) )
		{
			MessageBox(m_mainWindow, "DirectX failed to set the vertex declaration", "No Vertex Declaration", MB_OK | MB_ICONERROR);
			return false;
		}
	}
	else
	{
		MessageBox( m_mainWindow, "DirectX failed to create a Direct3D9 vertex declaration", "No Vertex Declaration", MB_OK | MB_ICONERROR );
		return false;
	}

	// Create a vertex buffer
	{
		// A triangle has three vertices
		const unsigned int bufferSize = 3 * sizeof( sVertex );
		// The usage tells Direct3D how this vertex buffer will be used
		DWORD usage = 0;
		{
			// Our code will only ever write to the buffer
			usage |= D3DUSAGE_WRITEONLY;
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			{
				D3DDEVICE_CREATION_PARAMETERS deviceCreationParameters;
				result = m_direct3dDevice->GetCreationParameters( &deviceCreationParameters );
				if ( SUCCEEDED( result ) )
				{
					DWORD vertexProcessingType = deviceCreationParameters.BehaviorFlags &
						( D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING );
					usage |= ( vertexProcessingType != D3DCREATE_SOFTWARE_VERTEXPROCESSING ) ?
						0 : D3DUSAGE_SOFTWAREPROCESSING;
				}
				else
				{
					MessageBox( m_mainWindow, "DirectX failed to get device creation parameters", "No Vertex Buffer", MB_OK | MB_ICONERROR );
					return false;
				}
			}
		}
		// We will define our own vertex format
		const DWORD useSeparateVertexDeclaration = 0;
		// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
		const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
		HANDLE* const notUsed = NULL;

		result = m_direct3dDevice->CreateVertexBuffer( bufferSize, usage, useSeparateVertexDeclaration, useDefaultPool,
			&m_vertexBuffer, notUsed );
		if ( FAILED( result ) )
		{
			MessageBox( m_mainWindow, "DirectX failed to create a vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR );
			return false;
		}
	}
	// Fill the vertex buffer with the triangle's vertices
	{
		// Before the vertex buffer can be changed it must be "locked"
		sVertex* vertexData;
		{
			const unsigned int lockEntireBuffer = 0;
			const DWORD useDefaultLockingBehavior = 0;
			result = m_vertexBuffer->Lock( lockEntireBuffer, lockEntireBuffer,
				reinterpret_cast<void**>( &vertexData ), useDefaultLockingBehavior );
			if ( FAILED( result ) )
			{
				MessageBox( m_mainWindow, "DirectX failed to lock the vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR );
				return false;
			}
		}
		// Fill the buffer
		{
			vertexData[0].x = 0.0f;
			vertexData[0].y = 0.0f;

			vertexData[1].x = 1.0f;
			vertexData[1].y = 1.0f;

			vertexData[2].x = 1.0f;
			vertexData[2].y = 0.0f;
		}
		// The buffer must be "unlocked" before it can be used
		{
			result = m_vertexBuffer->Unlock();
			if ( FAILED( result ) )
			{
				MessageBox( m_mainWindow, "DirectX failed to unlock the vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR );
				return false;
			}
		}
	}

	return true;
}

bool GraphicsSystem::LoadFragmentShader()
{
	// Load the source code from file and compile it
	ID3DXBuffer* compiledShader;
	{
		const char* sourceCodeFileName = m_FragmentShaderFilePath.c_str();
		const D3DXMACRO* noMacros = NULL;
		ID3DXInclude* noIncludes = NULL;
		const char* entryPoint = "main";
		const char* profile = "ps_3_0";
		const DWORD noFlags = 0;
		ID3DXBuffer* errorMessages = NULL;
		ID3DXConstantTable** noConstants = NULL;
		HRESULT result = D3DXCompileShaderFromFile( sourceCodeFileName, noMacros, noIncludes, entryPoint, profile, noFlags,
			&compiledShader, &errorMessages, noConstants );
		if ( SUCCEEDED( result ) )
		{
			if ( errorMessages )
			{
				errorMessages->Release();
			}
		}
		else
		{
			if ( errorMessages )
			{
				std::string errorMessage = std::string( "DirectX failed to compiled the fragment shader from the file " ) +
					sourceCodeFileName + ":\n" +
					reinterpret_cast<char*>( errorMessages->GetBufferPointer() );
				MessageBox( m_mainWindow, errorMessage.c_str(), "No Fragment Shader", MB_OK | MB_ICONERROR );
				errorMessages->Release();
				return false;
			}
			else
			{
				std::string errorMessage = "DirectX failed to compiled the fragment shader from the file ";
				errorMessage += sourceCodeFileName;
				MessageBox( m_mainWindow, errorMessage.c_str(), "No Fragment Shader", MB_OK | MB_ICONERROR );
				return false;
			}
		}
	}
	// Create the fragment shader object
	bool wereThereErrors = false;
	{
		HRESULT result = m_direct3dDevice->CreatePixelShader( reinterpret_cast<DWORD*>( compiledShader->GetBufferPointer() ),
			&m_fragmentShader );
		if ( FAILED( result ) )
		{
			MessageBox( m_mainWindow, "DirectX failed to create the fragment shader", "No Fragment Shader", MB_OK | MB_ICONERROR );
			wereThereErrors = true;
		}
		compiledShader->Release();
	}
	return !wereThereErrors;
}

bool GraphicsSystem::LoadVertexShader()
{
	// Load the source code from file and compile it
	ID3DXBuffer* compiledShader;
	{
		const char* sourceCodeFileName = m_VertexShaderFilePath.c_str();
		const D3DXMACRO* noMacros = NULL;
		ID3DXInclude* noIncludes = NULL;
		const char* entryPoint = "main";
		const char* profile = "vs_3_0";
		const DWORD noFlags = 0;
		ID3DXBuffer* errorMessages = NULL;
		ID3DXConstantTable** noConstants = NULL;
		HRESULT result = D3DXCompileShaderFromFile( sourceCodeFileName, noMacros, noIncludes, entryPoint, profile, noFlags,
			&compiledShader, &errorMessages, noConstants );
		if ( SUCCEEDED( result ) )
		{
			if ( errorMessages )
			{
				errorMessages->Release();
			}
		}
		else
		{
			if ( errorMessages )
			{
				std::string errorMessage = std::string( "DirectX failed to compile the vertex shader from the file " ) +
					sourceCodeFileName + ":\n" +
					reinterpret_cast<char*>( errorMessages->GetBufferPointer() );
				MessageBox( m_mainWindow, errorMessage.c_str(), "No Vertex Shader", MB_OK | MB_ICONERROR );
				errorMessages->Release();
				return false;
			}
			else
			{
				std::string errorMessage = "DirectX failed to compile the vertex shader from the file ";
				errorMessage += sourceCodeFileName;
				MessageBox( m_mainWindow, errorMessage.c_str(), "No Vertex Shader", MB_OK | MB_ICONERROR );
				return false;
			}
		}
	}
	// Create the vertex shader object
	bool wereThereErrors = false;
	{
		HRESULT result = m_direct3dDevice->CreateVertexShader( reinterpret_cast<DWORD*>( compiledShader->GetBufferPointer() ),
			&m_vertexShader );
		if ( FAILED( result ) )
		{
			MessageBox( m_mainWindow, "DirectX failed to create the vertex shader", "No Vertex Shader", MB_OK | MB_ICONERROR );
			wereThereErrors = true;
		}
		compiledShader->Release();
	}
	return !wereThereErrors;
}
