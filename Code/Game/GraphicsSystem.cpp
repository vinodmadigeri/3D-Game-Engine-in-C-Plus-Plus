// Header Files
//=============

#include "PreCompiled.h"
#include <d3dx9shader.h>

#include "GraphicsSystem.h"
#include "Material.h"


// Static Data Initialization
//===========================
namespace Engine
{
	static D3DVERTEXELEMENT9 s_vertexElements[] =
	{
		// Stream 0
		//---------
		// POSITION, 2 floats == 8 bytes
		{ 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		// COLOR0, D3DCOLOR == 4 bytes
		{ 0, 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};

	GraphicsSystem *GraphicsSystem::m_pInstance = NULL;

	// Interface
	//==========

	GraphicsSystem::GraphicsSystem(const HWND i_mainWindow,
		const std::string &i_VertexShaderPath,
		const std::string &i_FragmentShaderPath,
		const unsigned int i_windowWidth,
		const unsigned int i_windowHeight,
		const bool i_shouldRenderFullScreen) :
		m_windowWidth(i_windowWidth),
		m_windowHeight(i_windowHeight),
		m_shouldRenderFullScreen(i_shouldRenderFullScreen),
		m_mainWindow(i_mainWindow),
		m_direct3dInterface(NULL),
		m_direct3dDevice(NULL),
		m_vertexDeclaration(NULL),
		m_indexBuffer(NULL),
		m_vertexBuffer(NULL),
		m_material(NULL),
		mInitilized(false)
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
	bool GraphicsSystem::CreateInstance(const HWND i_mainWindow,
		const std::string &i_VertexShaderPath,
		const std::string &i_FragmentShaderPath,
		const unsigned int i_windowWidth,
		const unsigned int i_windowHeight,
		const bool i_shouldRenderFullScreen)
	{
		if (m_pInstance == NULL)
		{
			
			m_pInstance = new GraphicsSystem(i_mainWindow, i_VertexShaderPath, i_FragmentShaderPath, i_windowWidth, i_windowHeight, i_shouldRenderFullScreen);

			//Handle crash
			if (m_pInstance == NULL)
			{
				return false;
			}
			
			//Delete the memory if not initilized
			if (m_pInstance->mInitilized == false)
			{
				delete m_pInstance;
				m_pInstance = NULL;
			}
		}

		return m_pInstance != NULL;
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

		if (!CreateMaterial())
		{
			goto OnError;
		}

		if (!CreateVertexandIndexBuffer())
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
				clearColor = D3DCOLOR_XRGB(0, 0, 0);
			}
			const float noZBuffer = 0.0f;
			const DWORD noStencilBuffer = 0;
			HRESULT result = m_direct3dDevice->Clear(subRectangleCount, subRectanglesToClear,
				clearTheRenderTarget, clearColor, noZBuffer, noStencilBuffer);
			assert(SUCCEEDED(result));
		}

		// The actual function calls that draw geometry must be made between paired calls to
		// BeginScene() and EndScene()
		{
			HRESULT result = m_direct3dDevice->BeginScene();
			assert(SUCCEEDED(result));
			{
				// Set the shaders
				{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					std::string errorMessage;
#endif
					result = m_material->Set(m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						, &errorMessage
#endif
						);
					assert(SUCCEEDED(result));
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					if (FAILED(result))
					{
						MessageBox(m_mainWindow, errorMessage.c_str(), "Error Setting Material", MB_OK | MB_ICONERROR);
					}
#endif
				}

				// Bind a specific vertex buffer to the device as a data source
				{
					// There can be multiple streams of data feeding the display adaptor at the same time
					const unsigned int streamIndex = 0;
					// It's possible to start streaming data in the middle of a vertex buffer
					const unsigned int bufferOffset = 0;
					// The "stride" defines how large a single vertex is in the stream of data
					const unsigned int bufferStride = sizeof(sVertex);
					result = m_direct3dDevice->SetStreamSource(streamIndex, m_vertexBuffer, bufferOffset, bufferStride);
					assert(SUCCEEDED(result));
				}

				// Set the indices to use
				{
					HRESULT result = m_direct3dDevice->SetIndices(m_indexBuffer);
					assert(SUCCEEDED(result));
				}

				// Render objects from the current streams
				{
					// We are using triangles as the "primitive" type,
					// and we have defined the vertex buffer as a triangle list
					// (meaning that every triangle is defined by three vertices)
					const D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST;
					// It's possible to start rendering primitives in the middle of the stream
					const unsigned int indexOfFirstVertexToRender = 0;
					const unsigned int indexOfFirstIndexToUse = 0;
					// We are drawing a single triangle
					const unsigned int primitiveCountToRender = 2;
					const unsigned int vertexCountToRender = 4; //Rectangle contains 4 vertices

					result = m_direct3dDevice->DrawIndexedPrimitive(primitiveType, indexOfFirstVertexToRender, indexOfFirstVertexToRender, vertexCountToRender, indexOfFirstIndexToUse, primitiveCountToRender);
					assert(SUCCEEDED(result));
				}
			}

			result = m_direct3dDevice->EndScene();
			assert(SUCCEEDED(result));
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
			assert(SUCCEEDED(result));
		}
	}

	bool GraphicsSystem::ShutDown()
	{
		bool wereThereErrors = false;

		if (m_direct3dInterface)
		{
			if (m_direct3dDevice)
			{
				if (m_material)
				{
					delete m_material;
					m_material = NULL;
				}

				if (m_vertexBuffer)
				{
					m_vertexBuffer->Release();
					m_vertexBuffer = NULL;
				}
				if (m_vertexDeclaration)
				{
					m_direct3dDevice->SetVertexDeclaration(NULL);
					m_vertexDeclaration->Release();
					m_vertexDeclaration = NULL;
				}
				if (m_indexBuffer)
				{
					m_indexBuffer->Release();
					m_indexBuffer = NULL;
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
			presentationParameters.BackBufferWidth = m_windowWidth;
			presentationParameters.BackBufferHeight = m_windowHeight;
			presentationParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
			presentationParameters.BackBufferCount = 1;
			presentationParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
			presentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
			presentationParameters.hDeviceWindow = i_mainWindow;
			presentationParameters.Windowed = m_shouldRenderFullScreen ? FALSE : TRUE;
			presentationParameters.EnableAutoDepthStencil = FALSE;
			presentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		}

		HRESULT result = m_direct3dInterface->CreateDevice(useDefaultDevice, useHardwareRendering,
			i_mainWindow, useHardwareVertexProcessing, &presentationParameters, &m_direct3dDevice);
		if (SUCCEEDED(result))
		{
			return true;
		}
		else
		{
			MessageBox(i_mainWindow, "DirectX failed to create a Direct3D9 device", "No D3D9 Device", MB_OK | MB_ICONERROR);
			return false;
		}
	}

	bool GraphicsSystem::CreateInterface(const HWND i_mainWindow)
	{
		// D3D_SDK_VERSION is #defined by the Direct3D header files,
		// and is just a way to make sure that everything is up-to-date
		m_direct3dInterface = Direct3DCreate9(D3D_SDK_VERSION);
		if (m_direct3dInterface)
		{
			return true;
		}
		else
		{
			MessageBox(i_mainWindow, "DirectX failed to create a Direct3D9 interface", "No D3D9 Interface", MB_OK | MB_ICONERROR);
			return false;
		}
	}

	bool GraphicsSystem::CreateMaterial()
	{
		if (m_material == NULL)
		{
			m_material = new Material();
			assert(m_material);

#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			std::string o_errorMessage;
#endif
			HRESULT result = m_material->Load("", m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, &o_errorMessage);
#endif

			if (FAILED(result))
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				MessageBox(m_mainWindow, o_errorMessage.c_str(), "No Shader File", MB_OK | MB_ICONERROR);
#endif
				return false;
			}
		}

		return true;
	}

	bool GraphicsSystem::CreateVertexandIndexBuffer()
	{
		// The usage tells Direct3D how this vertex buffer will be used
		DWORD usage = 0;
		{
			// Our code will only ever write to the buffer
			usage |= D3DUSAGE_WRITEONLY;
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
					MessageBox(m_mainWindow, "DirectX failed to get device creation parameters", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}
		}

		if (!CreateVertexBuffer(usage))
		{
			return false;
		}

		if (!CreateIndexBuffer(usage))
		{
			return false;
		}

		return true;
	}

	bool GraphicsSystem::CreateVertexBuffer(DWORD i_usage)
	{
		// Initialize the vertex format
		HRESULT result = m_direct3dDevice->CreateVertexDeclaration(s_vertexElements, &m_vertexDeclaration);
		if (SUCCEEDED(result))
		{
			result = m_direct3dDevice->SetVertexDeclaration(m_vertexDeclaration);
			if (FAILED(result))
			{
				MessageBox(m_mainWindow, "DirectX failed to set the vertex declaration", "No Vertex Declaration", MB_OK | MB_ICONERROR);
				return false;
			}
		}
		else
		{
			MessageBox(m_mainWindow, "DirectX failed to create a Direct3D9 vertex declaration", "No Vertex Declaration", MB_OK | MB_ICONERROR);
			return false;
		}

		// Create a vertex buffer
		{
			// We are drawing a rectangle
			const unsigned int vertexCount = 4;
			const unsigned int bufferSize = vertexCount * sizeof(sVertex);

			// We will define our own vertex format
			const DWORD useSeparateVertexDeclaration = 0;
			// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
			const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
			HANDLE* const notUsed = NULL;

			result = m_direct3dDevice->CreateVertexBuffer(bufferSize, i_usage, useSeparateVertexDeclaration, useDefaultPool,
				&m_vertexBuffer, notUsed);
			if (FAILED(result))
			{
				MessageBox(m_mainWindow, "DirectX failed to create a vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR);
				return false;
			}
		}
		// Fill the vertex buffer with the rectangle's vertices
		{
			// Before the vertex buffer can be changed it must be "locked"
			sVertex* vertexData;
			{
				const unsigned int lockEntireBuffer = 0;
				const DWORD useDefaultLockingBehavior = 0;
				result = m_vertexBuffer->Lock(lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>(&vertexData), useDefaultLockingBehavior);
				if (FAILED(result))
				{
					MessageBox(m_mainWindow, "DirectX failed to lock the vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}

			// Fill the buffer
			{
				vertexData[0].x = 0.0f;
				vertexData[0].y = 0.0f;
				vertexData[0].color = D3DCOLOR_XRGB(255, 0, 0);

				vertexData[1].x = 0.0f;
				vertexData[1].y = 1.0f;
				vertexData[1].color = D3DCOLOR_XRGB(0, 255, 0);

				vertexData[2].x = 1.0f;
				vertexData[2].y = 1.0f;
				vertexData[2].color = D3DCOLOR_XRGB(0, 0, 255);

				vertexData[3].x = 1.0f;
				vertexData[3].y = 0.0f;
				vertexData[3].color = D3DCOLOR_XRGB(255, 255, 255);
			}

			// The buffer must be "unlocked" before it can be used
			{
				result = m_vertexBuffer->Unlock();
				if (FAILED(result))
				{
					MessageBox(m_mainWindow, "DirectX failed to unlock the vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}
		}

		return true;
	}

	bool GraphicsSystem::CreateIndexBuffer(DWORD i_usage)
	{
		// Create an index buffer
		{
			// We'll use 32-bit indices in this class to keep things simple
			D3DFORMAT format = D3DFMT_INDEX32;
			unsigned int bufferLength;
			{
				// EAE6320_TODO: How many triangles in a rectangle?
				const unsigned int verticesPerTriangle = 3;
				const unsigned int trianglesPerRectangle = 2;
				bufferLength = verticesPerTriangle * trianglesPerRectangle * sizeof(DWORD32);
			}
			D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
			HANDLE* notUsed = NULL;

			HRESULT result = m_direct3dDevice->CreateIndexBuffer(bufferLength, i_usage, format, useDefaultPool,
																	&m_indexBuffer, notUsed);
			if (FAILED(result))
			{
				MessageBox(m_mainWindow, "DirectX failed to create an index buffer", "No Index Buffer", MB_OK | MB_ICONERROR);
				return false;
			}
		}
		// Fill the index buffer with the rectangle's triangles' indices
		{
			// Before the index buffer can be changed it must be "locked"
			DWORD32* indices;
			{
				const unsigned int lockEntireBuffer = 0;
				const DWORD useDefaultLockingBehavior = 0;
				const HRESULT result = m_indexBuffer->Lock(lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>(&indices), useDefaultLockingBehavior);
				if (FAILED(result))
				{
					MessageBox(m_mainWindow, "DirectX failed to lock the index buffer", "No Index Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}
			// Fill the buffer
			{
				// EAE6320_TODO: What should the indices be
				// in order to draw the required number of triangles
				// using a left-handed winding order?

				// The first triangle can be filled in like this:
				indices[0] = 0;
				indices[1] = 1;
				indices[2] = 3;

				// A second triangle could be filled in like this:
				indices[3] = 1;
				indices[4] = 2;
				indices[5] = 3;

				// And so on for all of the required triangles
			}
			// The buffer must be "unlocked" before it can be used
			{
				const HRESULT result = m_indexBuffer->Unlock();
				if (FAILED(result))
				{
					MessageBox(m_mainWindow, "DirectX failed to unlock the index buffer", "No Index Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}
		}

		return true;
	}
}