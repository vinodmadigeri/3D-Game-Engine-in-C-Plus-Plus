// Header Files
//=============

#include "PreCompiled.h"
#include <d3dx9shader.h>
#include "Actor.h"
#include "GraphicsSystem.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshData.h"
#include "Sprite.h"
#include "../UserSettings/UserSettings.h"
#include "DebugLineRenderer.h"

// Static Data Initialization
//===========================
namespace Engine
{
	D3DVERTEXELEMENT9 GraphicsSystem::s_vertexElements[] =
	{
		// Stream 0
		//---------
		// POSITION, 3 floats == 12 bytes
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		// UV, 2 floats == 8 bytes
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		// UV, 3 floats == 12 bytes
		{ 0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		// UV, 3 floats == 12 bytes
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		// UV, 3 floats == 12 bytes
		{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		// COLOR0, D3DCOLOR == 4 bytes
		{ 0, 56, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};

	bool GraphicsSystem::s_bInFrame = false;
	GraphicsSystem *GraphicsSystem::m_pInstance = NULL;
	std::map<unsigned int, SharedPointer<Mesh>> GraphicsSystem::mMeshCache;
	std::map<unsigned int, SharedPointer<Material>> GraphicsSystem::mMaterialCache;
	std::map<unsigned int, SharedPointer<Sprite>> GraphicsSystem::mSpriteCache;

	// Interface
	//==========

	GraphicsSystem::GraphicsSystem(const HWND i_mainWindow,
		const unsigned int i_windowWidth,
		const unsigned int i_windowHeight,
		const bool i_shouldRenderFullScreen,
		const bool i_shouldEnableAntiAliasing) :
		m_windowWidth(i_windowWidth),
		m_windowHeight(i_windowHeight),
		m_shouldRenderFullScreen(i_shouldRenderFullScreen),
		m_shouldEnableAntiAliasing(i_shouldEnableAntiAliasing),
		m_mainWindow(i_mainWindow),
		m_direct3dInterface(NULL),
		m_direct3dDevice(NULL),
		m_pvertexDeclaration(NULL),
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
	bool GraphicsSystem::CreateInstance(const HWND i_mainWindow)
	{
		if (m_pInstance == NULL)
		{
			m_pInstance = new GraphicsSystem(i_mainWindow, UserSettings::GetWidth(), UserSettings::GetHeight(), UserSettings::IsFullScreenModeEnabled(), UserSettings::IsAntiAliasingEnabled());

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

		if (!CreateAndSetVertexDecleration())
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

	bool GraphicsSystem::Begin2D(void)
	{
		if (CanSubmit())
		{
			assert(m_direct3dDevice);

			//To enable standard alpha blending
			m_direct3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			m_direct3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_direct3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			//turn off depth writing and depth comparisons
			m_direct3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			m_direct3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

			return true;
		}

		return false;
	}

	bool GraphicsSystem::Begin3D(void)
	{
		if (CanSubmit())
		{
			assert(m_direct3dDevice);

			//To disable standard alpha blending
			m_direct3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

			//turn on depth writing and depth comparisons
			m_direct3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			m_direct3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			m_direct3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

			return true;
		}

		return false;
	}

	void GraphicsSystem::Render(SharedPointer<Material> i_Material, SharedPointer<Mesh> i_Mesh, SharedPointer<Actor> ThisObject)
	{
		assert(mInitilized == true);

		//Only render when in frame
		if (CanSubmit())
		{
			// Set the shaders
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				std::string errorMessage;
#endif
				HRESULT result = i_Material->Set(m_direct3dDevice, ThisObject
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
#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
			std::wstringstream EventMessage;
			EventMessage << "Draw Mesh " << (i_Mesh->GetName().c_str());
			D3DPERF_BeginEvent(0, EventMessage.str().c_str());
#endif
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
				
				const unsigned int primitiveCountToRender = i_Mesh->mDrawInfo.m_PrimitiveCount;
				const unsigned int vertexCountToRender = i_Mesh->mDrawInfo.m_NumOfVertices; 

				HRESULT result = m_direct3dDevice->DrawIndexedPrimitive(primitiveType, indexOfFirstVertexToRender, indexOfFirstVertexToRender, vertexCountToRender, indexOfFirstIndexToUse, primitiveCountToRender);
				assert(SUCCEEDED(result));
			}
#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
			D3DPERF_EndEvent();
#endif
		}
	}

	void GraphicsSystem::RenderSprite(SharedPointer<Sprite> i_Sprite)
	{
		assert(mInitilized == true);

		//Only render when in frame
		if (CanSubmit())
		{
			// Set the shaders
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				std::string errorMessage;
#endif
				HRESULT result = i_Sprite->Set(m_direct3dDevice
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

#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
			std::wstringstream EventMessage;
			EventMessage << "Draw Sprite " << (i_Sprite->GetName().c_str());
			D3DPERF_BeginEvent(0, EventMessage.str().c_str());
#endif
			// Bind a specific vertex buffer to the device as a data source
			{
				// There can be multiple streams of data feeding the display adaptor at the same time
				const unsigned int streamIndex = 0;
				// It's possible to start streaming data in the middle of a vertex buffer
				const unsigned int bufferOffset = 0;
				// The "stride" defines how large a single vertex is in the stream of data
				const unsigned int bufferStride = i_Sprite->m_spriteDrawInfo.m_VertexStride;
				HRESULT result = m_direct3dDevice->SetStreamSource(streamIndex, i_Sprite->GetVertexBuffer(), bufferOffset, bufferStride);
				assert(SUCCEEDED(result));
			}

			// Render objects from the current streams
			{
				// We are using triangles as the "primitive" type,
				// and we have defined the vertex buffer as a triangle list
				// (meaning that every triangle is defined by three vertices)
				const D3DPRIMITIVETYPE primitiveType = i_Sprite->m_spriteDrawInfo.m_PrimitiveType;
				// It's possible to start rendering primitives in the middle of the stream
				const unsigned int indexOfFirstVertexToRender = i_Sprite->m_spriteDrawInfo.m_indexOfFirstVertexToRender;
				
				const unsigned int primitiveCountToRender = i_Sprite->m_spriteDrawInfo.m_PrimitiveCount;
				const unsigned int vertexCountToRender = i_Sprite->m_spriteDrawInfo.m_NumOfVertices;

				HRESULT result = m_direct3dDevice->DrawPrimitive(primitiveType, indexOfFirstVertexToRender, primitiveCountToRender);
				
				assert(SUCCEEDED(result));
			}
#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
			D3DPERF_EndEvent();
#endif
		}
	}

	bool GraphicsSystem::ShutDown()
	{
		bool wereThereErrors = false;

		if (m_direct3dInterface)
		{
			if (m_direct3dDevice)
			{
				if (mMaterialCache.size() > 0)
				{
					mMaterialCache.clear();
				}

				if (mMeshCache.size() > 0)
				{
					mMeshCache.clear();
				}

				if (mSpriteCache.size() > 0)
				{
					mSpriteCache.clear();
				}

				if (m_pvertexDeclaration)
				{
					m_pvertexDeclaration->Release();
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
		bool AntiAliasingEnabled = false;
		D3DPRESENT_PARAMETERS presentationParameters = { 0 };
		{
			presentationParameters.BackBufferWidth = m_windowWidth;
			presentationParameters.BackBufferHeight = m_windowHeight;
			presentationParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
			presentationParameters.BackBufferCount = 1;
			presentationParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
			
			if (m_shouldEnableAntiAliasing && SUCCEEDED(m_direct3dInterface->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, FALSE,
				D3DMULTISAMPLE_16_SAMPLES, NULL)))
			{

				// Full-scene antialiasing is supported. Enable it here.
				AntiAliasingEnabled = true;
				presentationParameters.MultiSampleType = D3DMULTISAMPLE_16_SAMPLES;
			}

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
			if (AntiAliasingEnabled)
				m_direct3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

			//linear filtering for textures
			for (DWORD i = 0; i < 8; ++i)
			{
				m_direct3dDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
				m_direct3dDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
				m_direct3dDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			}

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

		std::map<unsigned int, SharedPointer<Material>>::iterator it;
		{

			it = mMaterialCache.find(HashedString::Hash(i_MaterialPath));

			if (it == mMaterialCache.end())
			{
				SharedPointer<Material> pMaterial = NULL;
				{
					pMaterial = new Material(i_MaterialPath, m_direct3dDevice);
					assert(pMaterial != NULL);

#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					std::string o_errorMessage;
#endif
					bool result = pMaterial->Load(i_MaterialPath, m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						, &o_errorMessage
#endif
						);

					if (!result)
					{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						std::stringstream errorMessage;
						errorMessage << "Couldn't load Material File : " << i_MaterialPath;
						MessageBox(m_mainWindow, o_errorMessage.c_str(), errorMessage.str().c_str(), MB_OK | MB_ICONERROR);
#endif
						return NULL;
					}
				}
				std::pair<unsigned int, SharedPointer<Material>> thisPair = std::pair<unsigned int, SharedPointer<Material>>(HashedString::Hash(i_MaterialPath), pMaterial);
				mMaterialCache.insert(thisPair);

				return pMaterial;
			}
			else
			{
				return it->second;
			}
		}
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

	bool GraphicsSystem::CreateDebugLineRenderer(const char *iName, unsigned int iMaxLines)
	{
		bool wereThereErrors = false;
#ifdef EAE2014_DEBUGLINE_SHOULDDRAW
		DebugLineRenderer::CreateInstance(iName, m_direct3dDevice, iMaxLines);

		if (DebugLineRenderer::GetInstance())
		{
			if (!DebugLineRenderer::GetInstance()->CreateVertexBuffer())
			{
				DebugPrint("Couldnot create Vertex Buffer for Line Renderer");

				wereThereErrors = true;
			}


			if (!DebugLineRenderer::GetInstance()->LoadShaders())
			{
				DebugPrint("Couldnot create Vertex Buffer for Line Renderer");

				wereThereErrors = true;
			}
		}
#endif
		return !wereThereErrors;
	}

	SharedPointer<Mesh> GraphicsSystem::CreateMesh(const char* i_MeshPath)
	{
		std::string errorMessage;

		DWORD usage = 0;
		{
			if (true == ComputeUsage(usage))
			{
				std::map<unsigned int, SharedPointer<Mesh>>::iterator it;
				{
					it = mMeshCache.find(HashedString::Hash(i_MeshPath));

					if (it != mMeshCache.end())
					{
						return it->second;
					}
					else
					{
						DrawInfo iO_DrawInfo;

						if (!Mesh::GetDrawInfoFromMeshFile(i_MeshPath, iO_DrawInfo))
						{
							goto OnError;
						}

						IDirect3DVertexBuffer9* vertexBuffer = NULL;
						{
							if (!CreateVertexBuffer(usage, &vertexBuffer, iO_DrawInfo))
							{
								errorMessage = "DirectX Failed to create Vertex Buffer";
								goto OnError;
							}
						}

						IDirect3DIndexBuffer9* indexBuffer = NULL;
						{
							if (!CreateIndexBuffer(usage, &indexBuffer, iO_DrawInfo))
							{
								errorMessage = "DirectX Failed to create Index Buffer";
								goto OnError;
							}
						}

						SharedPointer<Mesh> pMesh = NULL;
						{
							pMesh = new Mesh(i_MeshPath, iO_DrawInfo, vertexBuffer, indexBuffer);

							if (pMesh == NULL)
							{
								errorMessage = "Mesh Creation failed";
								goto OnError;
							}

							std::pair<unsigned int, SharedPointer<Mesh>> thisPair = std::pair<unsigned int, SharedPointer<Mesh>>(HashedString::Hash(i_MeshPath), pMesh);
							mMeshCache.insert(thisPair);

							delete iO_DrawInfo.m_pVerticesData;
							delete iO_DrawInfo.m_pIndices;

							return pMesh;
						}
					}
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

	bool GraphicsSystem::CreateAndSetVertexDecleration()
	{
		IDirect3DVertexDeclaration9* i_pvertexDeclaration;
		// Initialize the vertex format
		HRESULT result = m_direct3dDevice->CreateVertexDeclaration(s_vertexElements, &i_pvertexDeclaration);
		if (SUCCEEDED(result))
		{
			result = m_direct3dDevice->SetVertexDeclaration(i_pvertexDeclaration);
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

		return true;
	}

	bool GraphicsSystem::CreateVertexBuffer(DWORD i_usage, IDirect3DVertexBuffer9** i_ppvertexBuffer, const DrawInfo &i_DrawInfo)
	{
		HRESULT result;
		// Create a vertex buffer
		{
			// We are drawing a mesh
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
		
		// Fill the vertex buffer with the Mesh's vertices
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
		// Fill the index buffer with the Mesh's triangles' indices
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

	SharedPointer<Sprite> GraphicsSystem::CreateSprite(const char* i_TexturePath, const sRectangle &i_texcoordsRect, const sSprite &i_spriteDetails)
	{
		std::string errorMessage;
		std::map<unsigned int, SharedPointer<Sprite>>::iterator it;
		{
			it = mSpriteCache.find(HashedString::Hash(i_TexturePath));

			if (it != mSpriteCache.end())
			{
				return it->second;
			}
			else
			{
				IDirect3DVertexBuffer9* vertexBuffer = NULL;
				{
					if (!CreateVertexBufferForSprite(&vertexBuffer))
					{
						errorMessage = "DirectX Failed to create Vertex Buffer";
						goto OnError;
					}
				}

				SharedPointer<Sprite> pSprite = NULL;
				{
					pSprite = new Sprite(i_TexturePath, m_direct3dDevice, vertexBuffer);

					if (pSprite == NULL)
					{
						errorMessage = "Sprite Creation failed";
						goto OnError;
					}

					bool result = pSprite->Load(i_TexturePath, i_texcoordsRect, i_spriteDetails, m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						, &errorMessage
#endif
						);

					if (!result)
					{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
						std::stringstream errorMessagestream;
						errorMessagestream << "Couldn't load Material File : " << i_TexturePath;
						MessageBox(m_mainWindow, errorMessage.c_str(), errorMessagestream.str().c_str(), MB_OK | MB_ICONERROR);
#endif
						return NULL;
					}

					std::pair<unsigned int, SharedPointer<Sprite>> thisPair = std::pair<unsigned int, SharedPointer<Sprite>>(HashedString::Hash(i_TexturePath), pSprite);
					mSpriteCache.insert(thisPair);

					return pSprite;
				}
			}
		}

	OnError:
		MessageBox(m_mainWindow, errorMessage.c_str(), "Can not create Sprite", MB_OK | MB_ICONERROR);

		return NULL;
	}


	bool GraphicsSystem::CreateVertexBufferForSprite(IDirect3DVertexBuffer9** i_ppvertexBuffer)
	{
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
					MessageBox(m_mainWindow, "DirectX failed to get device creation parameters", "No Vertex Buffer", MB_OK | MB_ICONERROR);
					return false;
				}
			}
		}

		HRESULT result;
		// Create a vertex buffer
		{
			// We are drawing a mesh
			unsigned int numOfVerticesForSprite = 4;

			const unsigned int vertexCount = numOfVerticesForSprite;
			const unsigned int bufferSize = vertexCount * sizeof(sVertexData);

			// We will define our own vertex format
			const DWORD useSeparateVertexDeclaration = 0;
			// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
			const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
			HANDLE* const notUsed = NULL;

			result = m_direct3dDevice->CreateVertexBuffer(bufferSize, usage, useSeparateVertexDeclaration, useDefaultPool,
				i_ppvertexBuffer, notUsed);
			if (FAILED(result))
			{
				MessageBox(m_mainWindow, "DirectX failed to create a vertex buffer", "No Vertex Buffer", MB_OK | MB_ICONERROR);
				return false;
			}
		}

		return true;
	}
}