// Header Files
//=============

#include "PreCompiled.h"
#include <d3dx9shader.h>

#include "GraphicsSystem.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshData.h"

// Static Data Initialization
//===========================
namespace Engine
{
	static D3DVERTEXELEMENT9 s_vertexElements[] =
	{
		// Stream 0
		//---------
		// POSITION, 3 floats == 12 bytes
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		// COLOR0, D3DCOLOR == 4 bytes
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};

	bool GraphicsSystem::s_bInFrame = false;
	GraphicsSystem *GraphicsSystem::m_pInstance = NULL;

	// Interface
	//==========

	GraphicsSystem::GraphicsSystem(const HWND i_mainWindow,
		const unsigned int i_windowWidth,
		const unsigned int i_windowHeight,
		const bool i_shouldRenderFullScreen) :
		m_windowWidth(i_windowWidth),
		m_windowHeight(i_windowHeight),
		m_shouldRenderFullScreen(i_shouldRenderFullScreen),
		m_mainWindow(i_mainWindow),
		m_direct3dInterface(NULL),
		m_direct3dDevice(NULL),
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
		const unsigned int i_windowWidth,
		const unsigned int i_windowHeight,
		const bool i_shouldRenderFullScreen)
	{
		if (m_pInstance == NULL)
		{
			m_pInstance = new GraphicsSystem(i_mainWindow, i_windowWidth, i_windowHeight, i_shouldRenderFullScreen);

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

	bool GraphicsSystem::Initialize()
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

		return true;

	OnError:

		ShutDown();
		return false;
	}
	
	bool GraphicsSystem::CanSubmit(void)
	{
		return s_bInFrame;
	}

	bool GraphicsSystem::BeingFrame(const ColorRGBA & i_ClearColor)
	{
		assert(mInitilized);

		if (m_direct3dDevice)
		{
			// Every frame an entirely new image will be created.
			// Before drawing anything, then, the previous image will be erased
			// by "clearing" the image buffer (filling it with a solid color)
			// and any other associated buffers (filling them with whatever values make sense)
			{
				const D3DRECT* subRectanglesToClear = NULL;
				const DWORD subRectangleCount = 0;
				const DWORD clearTheRenderTarget = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
				D3DCOLOR clearColor;
				{
					// Black is usually used:
					clearColor = D3DCOLOR_XRGB(i_ClearColor.r, i_ClearColor.g, i_ClearColor.b);
				}
				const float depthToClear = 1.0f; //Clear the depth buffer
				const DWORD noStencilBuffer = 0;
				HRESULT result = m_direct3dDevice->Clear(subRectangleCount, subRectanglesToClear,
					clearTheRenderTarget, clearColor, depthToClear, noStencilBuffer);
				assert(SUCCEEDED(result));
			}

			HRESULT result = m_direct3dDevice->BeginScene();
			assert(SUCCEEDED(result));


			s_bInFrame = true;
		}

		return true;
	}
	
	bool GraphicsSystem::EndFrame(void)
	{
		HRESULT result = m_direct3dDevice->EndScene();
		assert(SUCCEEDED(result));

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

		s_bInFrame = false;

		return true;
	}

	void GraphicsSystem::Render(SharedPointer<Material> i_Material, SharedPointer<Mesh> i_Mesh)
	{
		assert(mInitilized == true);

		//Only render when in frame
		assert(s_bInFrame);
		{
			// Set the shaders
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				std::string errorMessage;
#endif
				HRESULT result = i_Material->Set(m_direct3dDevice
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
				const unsigned int bufferStride = i_Mesh->mDrawInfo.m_VertexStride;
				HRESULT result = m_direct3dDevice->SetStreamSource(streamIndex, i_Mesh->GetVertexBuffer(), bufferOffset, bufferStride);
				assert(SUCCEEDED(result));
			}

			// Set the indices to use
			{
				HRESULT result = m_direct3dDevice->SetIndices(i_Mesh->GetIndexBuffer());
				assert(SUCCEEDED(result));
			}

			// Render objects from the current streams
			{
				// We are using triangles as the "primitive" type,
				// and we have defined the vertex buffer as a triangle list
				// (meaning that every triangle is defined by three vertices)
				const D3DPRIMITIVETYPE primitiveType = i_Mesh->mDrawInfo.m_PrimitiveType;
				// It's possible to start rendering primitives in the middle of the stream
				const unsigned int indexOfFirstVertexToRender = i_Mesh->mDrawInfo.m_indexOfFirstVertexToRender;
				const unsigned int indexOfFirstIndexToUse = i_Mesh->mDrawInfo.m_indexOfFirstIndexToUse;
				// We are drawing a single triangle
				const unsigned int primitiveCountToRender = i_Mesh->mDrawInfo.m_PrimitiveCount;
				const unsigned int vertexCountToRender = i_Mesh->mDrawInfo.m_NumOfVertices; //Rectangle contains 4 vertices

				HRESULT result = m_direct3dDevice->DrawIndexedPrimitive(primitiveType, indexOfFirstVertexToRender, indexOfFirstVertexToRender, vertexCountToRender, indexOfFirstIndexToUse, primitiveCountToRender);
				assert(SUCCEEDED(result));
			}
		}
	}

	bool GraphicsSystem::ShutDown()
	{
		bool wereThereErrors = false;

		if (m_direct3dInterface)
		{
			if (m_direct3dDevice)
			{
				if (m_Materials.size() > 0)
				{
					m_Materials.clear();
				}

				if (m_Meshes.size() > 0)
				{
					m_Meshes.clear();
				}

				m_direct3dDevice->SetVertexDeclaration(NULL);
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
			presentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

			//This tells Direct3D to create a depth/stencil buffer automatically and manage its lifetime
			presentationParameters.EnableAutoDepthStencil = TRUE;

			//When the automatic depth/stencil buffer is enabled we need to tell Direct3D which format to use. 
			//There are other formats available, but we are choosing to ask for 16 bits for depth in the hopes 
			//that it will be the most compatible and will work on any graphics hardware out there
			presentationParameters.AutoDepthStencilFormat = D3DFMT_D16;

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

	SharedPointer<Material> GraphicsSystem::CreateMaterial(const char *i_MaterialPath)
	{
		assert(m_direct3dDevice);

		SharedPointer<Material> OutMaterial = NULL;
		{
			OutMaterial = new Material(m_direct3dDevice);
			assert(OutMaterial != NULL);

#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			std::string o_errorMessage;
#endif
			bool result = OutMaterial->Load(i_MaterialPath, m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, &o_errorMessage
#endif
				);

			if (!result)
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				MessageBox(m_mainWindow, o_errorMessage.c_str(), "No Shader File", MB_OK | MB_ICONERROR);
#endif
				return NULL;
			}
		}

		m_Materials.push_back(OutMaterial);

		return OutMaterial;
	}


	bool GraphicsSystem::ComputeUsage(DWORD &o_usage)
	{
		// The usage tells Direct3D how this vertex buffer will be used
		{
			// Our code will only ever write to the buffer
			o_usage |= D3DUSAGE_WRITEONLY;
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			{
				D3DDEVICE_CREATION_PARAMETERS deviceCreationParameters;
				const HRESULT result = m_direct3dDevice->GetCreationParameters(&deviceCreationParameters);
				if (SUCCEEDED(result))
				{
					DWORD vertexProcessingType = deviceCreationParameters.BehaviorFlags &
						(D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING);
					o_usage |= (vertexProcessingType != D3DCREATE_SOFTWARE_VERTEXPROCESSING) ? 0 : D3DUSAGE_SOFTWAREPROCESSING;
				}
				else
				{
					MessageBox(m_mainWindow, "DirectX failed to get device creation parameters", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}
		}

		return true;
	}


	SharedPointer<Mesh> GraphicsSystem::CreateMesh(const DrawInfo &i_DrawInfo)
	{
		std::string errorMessage;

		DWORD usage = 0;
		{
			if (true == ComputeUsage(usage))
			{
				IDirect3DVertexDeclaration9* vertexDeclaration = NULL;
				IDirect3DVertexBuffer9* vertexBuffer = NULL;
				{
					if (!CreateVertexBuffer(usage, &vertexDeclaration, &vertexBuffer, i_DrawInfo))
					{
						errorMessage = "DirectX Failed to create Vertex Buffer";
						goto OnError;
					}
				}

				IDirect3DIndexBuffer9* indexBuffer = NULL;
				{
					if (!CreateIndexBuffer(usage, &indexBuffer, i_DrawInfo))
					{
						errorMessage = "DirectX Failed to create Index Buffer";
						goto OnError;
					}
				}

				SharedPointer<Mesh> pMesh = NULL;
				{
					pMesh = new Mesh(i_DrawInfo, vertexDeclaration, vertexBuffer, indexBuffer);

					if (pMesh == NULL)
					{
						errorMessage = "Mesh Creation failed";
						goto OnError;
					}

					m_Meshes.push_back(pMesh);

					return pMesh;
				}
			}
			else
			{
				errorMessage =  "Failed";
				goto OnError;
			}
		}

	OnError:
		MessageBox(m_mainWindow, errorMessage.c_str(), "Can not create Mesh", MB_OK | MB_ICONERROR);

		return NULL;

	}

	bool GraphicsSystem::CreateVertexBuffer(DWORD i_usage, IDirect3DVertexDeclaration9** i_ppvertexDeclaration, IDirect3DVertexBuffer9** i_ppvertexBuffer, const DrawInfo &i_DrawInfo)
	{
		// Initialize the vertex format
		HRESULT result = m_direct3dDevice->CreateVertexDeclaration(s_vertexElements, i_ppvertexDeclaration);
		if (SUCCEEDED(result))
		{
			result = m_direct3dDevice->SetVertexDeclaration(*i_ppvertexDeclaration);
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
			const unsigned int vertexCount = i_DrawInfo.m_NumOfVertices;
			const unsigned int bufferSize = vertexCount * i_DrawInfo.m_VertexStride;

			// We will define our own vertex format
			const DWORD useSeparateVertexDeclaration = 0;
			// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
			const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
			HANDLE* const notUsed = NULL;

			result = m_direct3dDevice->CreateVertexBuffer(bufferSize, i_usage, useSeparateVertexDeclaration, useDefaultPool,
															i_ppvertexBuffer, notUsed);
			if (FAILED(result))
			{
				MessageBox(m_mainWindow, "DirectX failed to create a vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR);
				return false;
			}
		}

		// Fill the vertex buffer with the rectangle's vertices
		{
			// Before the vertex buffer can be changed it must be "locked"
			sVertexData* vertexData;
			{
				const unsigned int lockEntireBuffer = 0;
				const DWORD useDefaultLockingBehavior = 0;
				result = (*i_ppvertexBuffer)->Lock(lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>(&vertexData), useDefaultLockingBehavior);
				if (FAILED(result))
				{
					MessageBox(m_mainWindow, "DirectX failed to lock the vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}

			// Fill the buffer
			{
				memcpy(vertexData, i_DrawInfo.m_pVerticesData, i_DrawInfo.m_VertexStride * i_DrawInfo.m_NumOfVertices);
			}

			// The buffer must be "unlocked" before it can be used
			{
				result = (*i_ppvertexBuffer)->Unlock();
				if (FAILED(result))
				{
					MessageBox(m_mainWindow, "DirectX failed to unlock the vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}
		}

		return true;
	}

	bool GraphicsSystem::CreateIndexBuffer(DWORD i_usage, IDirect3DIndexBuffer9** i_ppindexBuffer, const DrawInfo &i_DrawInfo)
	{
		// Create an index buffer
		{
			// We'll use 32-bit indices in this class to keep things simple
			D3DFORMAT format = D3DFMT_INDEX32;
			unsigned int bufferLength;
			{
				bufferLength = i_DrawInfo.m_IndexCount * sizeof(DWORD32);
			}
			D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
			HANDLE* notUsed = NULL;

			HRESULT result = m_direct3dDevice->CreateIndexBuffer(bufferLength, i_usage, format, useDefaultPool,
																i_ppindexBuffer, notUsed);
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
				const HRESULT result = (*i_ppindexBuffer)->Lock(lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>(&indices), useDefaultLockingBehavior);
				if (FAILED(result))
				{
					MessageBox(m_mainWindow, "DirectX failed to lock the index buffer", "No Index Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}
			// Fill the buffer
			{
				memcpy(indices, i_DrawInfo.m_pIndices, i_DrawInfo.m_IndexCount * sizeof(DWORD32));
				// And so on for all of the required triangles
			}
			// The buffer must be "unlocked" before it can be used
			{
				const HRESULT result = (*i_ppindexBuffer)->Unlock();
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