// Header Files
//=============


#include <sstream>


#include "cTextureBuilder.h"


#include "../BuilderHelper/cbBuilder.h"

// Static Data Initialization
//===========================

namespace Texture
{
	IDirect3D9* cTextureBuilder::s_direct3dInterface = NULL;
	IDirect3DDevice9* cTextureBuilder::s_direct3dDevice = NULL;
	IDirect3DBaseTexture9* cTextureBuilder::s_texture = NULL;

	// Interface
	//==========

	// Build
	//------

	bool cTextureBuilder::Build(const std::vector<const std::string>&)
	{
		bool wereThereErrors = false;


		if (!Initialize(m_path_source))
		{
			wereThereErrors = true;
			goto OnExit;
		}

		// Get information about the source image
		D3DXIMAGE_INFO imageInfo_source;
		{
			if (FAILED(D3DXGetImageInfoFromFile(m_path_source, &imageInfo_source)))
			{
				wereThereErrors = true;
				BuilderHelper::OutputErrorMessage("DirectX failed to get image info for the source image", m_path_source);
				goto OnExit;
			}
		}
		// Load the source image and do any necessary processing (compress, generate MIP maps, change resolution, etc.)
	{
		// A real game would have different compression options, but to keep things simple:
		const D3DFORMAT format = D3DFMT_DXT5;

		const unsigned int roundUpToAPowerOf2 = D3DX_DEFAULT;
		const unsigned int generateAllMipMaps = D3DX_DEFAULT;
		const DWORD staticTexture = 0;
		const D3DPOOL letD3dManageMemory = D3DPOOL_MANAGED;
		const DWORD useDefaultFiltering = D3DX_DEFAULT;
		const D3DCOLOR noColorKey = 0;
		PALETTEENTRY* noColorPalette = NULL;
		switch (imageInfo_source.ResourceType)
		{
		case D3DRTYPE_TEXTURE:
		{
			const HRESULT result = D3DXCreateTextureFromFileEx(s_direct3dDevice, m_path_source,
				roundUpToAPowerOf2, roundUpToAPowerOf2, generateAllMipMaps,
				staticTexture, format, letD3dManageMemory, useDefaultFiltering, useDefaultFiltering, noColorKey,
				&imageInfo_source, noColorPalette, reinterpret_cast<IDirect3DTexture9**>(&s_texture));
			if (FAILED(result))
			{
				wereThereErrors = true;
				BuilderHelper::OutputErrorMessage("DirectX failed to get image info for the source image", m_path_source);
				goto OnExit;
			}
		}
			break;
		case D3DRTYPE_CUBETEXTURE:
		{
			const HRESULT result = D3DXCreateCubeTextureFromFileEx(s_direct3dDevice, m_path_source,
				roundUpToAPowerOf2, generateAllMipMaps,
				staticTexture, format, letD3dManageMemory, useDefaultFiltering, useDefaultFiltering, noColorKey,
				&imageInfo_source, noColorPalette, reinterpret_cast<IDirect3DCubeTexture9**>(&s_texture));
			if (FAILED(result))
			{
				wereThereErrors = true;
				BuilderHelper::OutputErrorMessage("DirectX failed to get image info for the source image", m_path_source);
				goto OnExit;
			}
		}
			break;
		case D3DRTYPE_VOLUMETEXTURE:
		{
			const HRESULT result = D3DXCreateVolumeTextureFromFileEx(s_direct3dDevice, m_path_source,
				roundUpToAPowerOf2, roundUpToAPowerOf2, imageInfo_source.Depth, generateAllMipMaps,
				staticTexture, format, letD3dManageMemory, useDefaultFiltering, useDefaultFiltering, noColorKey,
				&imageInfo_source, noColorPalette, reinterpret_cast<IDirect3DVolumeTexture9**>(&s_texture));
			if (FAILED(result))
			{
				wereThereErrors = true;
				BuilderHelper::OutputErrorMessage("DirectX failed to get image info for the source image", m_path_source);
				goto OnExit;
			}
		}
			break;
		default:
		{
			wereThereErrors = true;
			std::stringstream errorMessage;
			errorMessage << "Unsupported texture resource type " << imageInfo_source.ResourceType;
			BuilderHelper::OutputErrorMessage(errorMessage.str().c_str(), m_path_source);
			goto OnExit;
		}
		}
	}
		// Save the texture
	{
		const D3DXIMAGE_FILEFORMAT ddsFormat = D3DXIFF_DDS;
		PALETTEENTRY* noColorPalette = NULL;
		HRESULT result = D3DXSaveTextureToFile(m_path_target, ddsFormat, s_texture, noColorPalette);
		if (FAILED(result))
		{
			wereThereErrors = true;
			BuilderHelper::OutputErrorMessage("DirectX failed to save the texture", m_path_target);
			goto OnExit;
		}
	}

	OnExit:

		if (!ShutDown())
		{
			wereThereErrors = true;
		}


		return !wereThereErrors;
	}


	bool cTextureBuilder::Initialize(const char* i_path_source)
	{
		// Create the D3D9 interface
		{
			s_direct3dInterface = Direct3DCreate9(D3D_SDK_VERSION);
			if (!s_direct3dInterface)
			{
				BuilderHelper::OutputErrorMessage("DirectX failed to create a Direct3D9 interface", i_path_source);
				return false;
			}
		}
		// Create the D3D9 device interface
		{
			HWND mainWindow = GetConsoleWindow();	// If this is NULL it's ok
			const UINT useDefaultDevice = D3DADAPTER_DEFAULT;
			const D3DDEVTYPE useHardwareRendering = D3DDEVTYPE_HAL;
			const DWORD useSoftwareVertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			D3DPRESENT_PARAMETERS presentationParameters = { 0 };
			{
				presentationParameters.BackBufferWidth = mainWindow == NULL ? 1 : 0;
				presentationParameters.BackBufferHeight = mainWindow == NULL ? 1 : 0;
				presentationParameters.BackBufferFormat = D3DFMT_UNKNOWN;
				presentationParameters.BackBufferCount = mainWindow == NULL ? 1 : 0;
				presentationParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
				presentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
				presentationParameters.hDeviceWindow = mainWindow;
				presentationParameters.Windowed = TRUE;
				presentationParameters.EnableAutoDepthStencil = FALSE;
			}
			HRESULT result = s_direct3dInterface->CreateDevice(useDefaultDevice, useHardwareRendering,
				mainWindow, useSoftwareVertexProcessing, &presentationParameters, &s_direct3dDevice);
			if (FAILED(result))
			{
				const D3DDEVTYPE useReferenceSoftwareImplementation = D3DDEVTYPE_REF;
				result = s_direct3dInterface->CreateDevice(useDefaultDevice, useReferenceSoftwareImplementation,
					mainWindow, useSoftwareVertexProcessing, &presentationParameters, &s_direct3dDevice);
				if (FAILED(result))
				{
					BuilderHelper::OutputErrorMessage("DirectX failed to create a Direct3D9 device", i_path_source);
					return false;
				}
			}
		}

		return true;
	}

	bool cTextureBuilder::ShutDown()
	{
		bool wereThereErrors = false;

		if (s_texture)
		{
			s_texture->Release();
			s_texture = NULL;
		}

		if (s_direct3dInterface)
		{
			if (s_direct3dDevice)
			{
				s_direct3dDevice->Release();
				s_direct3dDevice = NULL;
			}

			s_direct3dInterface->Release();
			s_direct3dInterface = NULL;
		}

		return !wereThereErrors;
	}

}