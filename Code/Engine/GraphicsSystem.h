/*
	This file contains the function declarations for graphics
*/

#ifndef __EAE2014_GRAPHICS_SYSTEM_H
#define __EAE2014_GRAPHICS_SYSTEM_H

// Header Files
//=============
#include "PreCompiled.h"


#include <d3d9.h>
#include <vector>
#include "MeshData.h"
#include "SharedPointer.h"

namespace Engine
{
	class Material; //Forward Decleration
	class Mesh;

	class GraphicsSystem
	{
	private:
		const unsigned int m_windowWidth;
		const unsigned int m_windowHeight;
		const bool m_shouldRenderFullScreen;
		HWND m_mainWindow;
		IDirect3D9* m_direct3dInterface;
		IDirect3DDevice9* m_direct3dDevice;
		std::vector<SharedPointer<Material>> m_Materials; //Shared materials
		std::vector<SharedPointer<Mesh>> m_Meshes; //Shared meshes

		static GraphicsSystem *m_pInstance;
		bool mInitilized;

		GraphicsSystem(const HWND i_mainWindow,
			const unsigned int i_windowWidth,
			const unsigned int i_windowHeight,
			const bool i_shouldRenderFullScreen); //Constructor
		GraphicsSystem(const GraphicsSystem & i_Other); //Copy Constructor
		GraphicsSystem & operator=(const GraphicsSystem & i_rhs); //Assignment Operators

		~GraphicsSystem();
		bool CreateDevice(const HWND i_mainWindow);
		bool CreateInterface(const HWND i_mainWindow);
		
		bool CreateVertexBuffer(DWORD i_usage, IDirect3DVertexDeclaration9** i_ppvertexDeclaration, IDirect3DVertexBuffer9** i_ppvertexBuffer, const DrawInfo &i_DrawInfo);
		bool CreateIndexBuffer(DWORD i_usage, IDirect3DIndexBuffer9** i_ppindexBuffer, const DrawInfo &i_DrawInfo);

		

		bool Initialize();
		bool ShutDown();


		bool ComputeUsage(DWORD &o_usage);
		static bool s_bInFrame;
		bool CanSubmit(void);
		

	public:
		static bool CreateInstance(const HWND i_mainWindow,
			const unsigned int i_windowWidth = 800,
			const unsigned int i_windowHeight = 600,
			const bool i_shouldRenderFullScreen = false);

		static GraphicsSystem * GetInstance();
		bool BeingFrame(const ColorRGBA & i_ClearColor = ColorRGBA(0, 0, 0, 0));
		void Render(SharedPointer<Material> i_Material, SharedPointer<Mesh> i_Mesh);
		bool EndFrame(void);
		SharedPointer<Mesh> CreateMesh(const DrawInfo &i_DrawInfo);
		SharedPointer<Material> CreateMaterial(const char *i_MaterialPath);

		static void Destroy();
	};
}
#endif	// __EAE2014_GRAPHICS_SYSTEM_H
