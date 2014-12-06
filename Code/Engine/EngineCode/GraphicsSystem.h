/*
	This file contains the function declarations for graphics
*/

#ifndef __EAE2014_GRAPHICS_SYSTEM_H
#define __EAE2014_GRAPHICS_SYSTEM_H

// Header Files
//=============
#include "PreCompiled.h"

#ifdef EAE2014_GRAPHICS_SHOULDD3DDEBUGINFOBEENABLED
#define D3D_DEBUG_INFO
#endif

#include <d3d9.h>
#include <map>
#include <vector>
#include "MeshData.h"
#include "SharedPointer.h"

namespace Engine
{
	class Material; //Forward Decleration
	class Mesh;
	class Actor;
	class Sprite;
	struct sRectangle;
	struct SpriteDrawInfo;

	class GraphicsSystem
	{
	private:
		const unsigned int m_windowWidth;
		const unsigned int m_windowHeight;
		const bool m_shouldRenderFullScreen;
		const bool m_shouldEnableAntiAliasing;
		HWND m_mainWindow;
		IDirect3D9* m_direct3dInterface;
		IDirect3DDevice9* m_direct3dDevice;
		IDirect3DVertexDeclaration9* m_pvertexDeclaration;

		static std::map<unsigned int, SharedPointer<Material>> mMaterialCache; //Shared materials
		static std::map<unsigned int, SharedPointer<Mesh>> mMeshCache; //Shared meshes
		static std::map<unsigned int, SharedPointer<Sprite>> mSpriteCache; //Shared meshes

		static GraphicsSystem *m_pInstance;
		bool mInitilized;

		GraphicsSystem(const HWND i_mainWindow,
			const unsigned int i_windowWidth,
			const unsigned int i_windowHeight,
			const bool i_shouldRenderFullScreen,
			const bool i_shouldEnableAntiAliasing); //Constructor

		GraphicsSystem(const GraphicsSystem & i_Other); //Copy Constructor
		GraphicsSystem & operator=(const GraphicsSystem & i_rhs); //Assignment Operators

		~GraphicsSystem();
		bool CreateDevice(const HWND i_mainWindow);
		bool CreateInterface(const HWND i_mainWindow);
		bool CreateAndSetVertexDecleration();

		bool CreateVertexBuffer(DWORD i_usage, IDirect3DVertexBuffer9** i_ppvertexBuffer, const DrawInfo &i_DrawInfo);
		bool CreateIndexBuffer(DWORD i_usage, IDirect3DIndexBuffer9** i_ppindexBuffer, const DrawInfo &i_DrawInfo);
		bool CreateVertexBufferForSprite(IDirect3DVertexBuffer9** i_ppvertexBuffer, const SpriteDrawInfo &i_SpriteDrawInfo);
		
		bool Initialize();
		bool ShutDown();

		bool ComputeUsage(DWORD &o_usage);
		static bool s_bInFrame;
		
	public:
		bool CanSubmit(void);
		static bool CreateInstance(const HWND i_mainWindow);
		static D3DVERTEXELEMENT9 s_vertexElements[];
		static GraphicsSystem * GetInstance();
		bool BeingFrame(const ColorRGBA & i_ClearColor = ColorRGBA(0, 0, 0, 0));
		bool GraphicsSystem::Begin2D(void);
		bool GraphicsSystem::Begin3D(void);
		void Render(SharedPointer<Material> i_Material, SharedPointer<Mesh> i_Mesh, SharedPointer<Actor> ThisObject);
		void RenderSprite(SharedPointer<Sprite> i_Sprite);
		bool EndFrame(void);
		bool CreateDebugLineRenderer(const char *iName, unsigned int iMaxLines);
		SharedPointer<Mesh> CreateMesh(const char* i_MeshPath);
		SharedPointer<Material> CreateMaterial(const char *i_MaterialPath);
		SharedPointer<Sprite> CreateSprite(const char* i_TexturePath, const sRectangle *i_positionRect,
			const sRectangle *i_texcoordsRect, unsigned int i_MaxHorizontalCount = 1, unsigned int i_MaxVerticalCount = 1);

		static void Destroy();
	};
}
#endif	// __EAE2014_GRAPHICS_SYSTEM_H
