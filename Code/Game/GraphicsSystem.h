/*
	This file contains the function declarations for graphics
*/

#ifndef __EAE2014_GRAPHICS_SYSTEM_H
#define __EAE2014_GRAPHICS_SYSTEM_H

// Header Files
//=============
#include "PreCompiled.h"

#include <d3d9.h>

namespace Engine
{
	class Material; //Forward Decleration

	class GraphicsSystem
	{
	private:
		const unsigned int m_windowWidth;
		const unsigned int m_windowHeight;
		const bool m_shouldRenderFullScreen;
		HWND m_mainWindow;
		IDirect3D9* m_direct3dInterface;
		IDirect3DDevice9* m_direct3dDevice;
		IDirect3DVertexDeclaration9* m_vertexDeclaration;
		// The index buffer describes how to make triangles with the vertices
		// (i.e. it defines the vertex connectivity)
		IDirect3DIndexBuffer9* m_indexBuffer;
		IDirect3DVertexBuffer9* m_vertexBuffer;

		Material *m_material;

		static GraphicsSystem *m_pInstance;
		bool mInitilized;

		// The vertex information that is sent to the display adaptor must match what the vertex shader expects
		struct sVertex
		{
			float x, y;
			D3DCOLOR color; // D3DCOLOR = 4 bytes, or 8 bits [0,255] per RGBA channel
		};

		GraphicsSystem(const HWND i_mainWindow,
			const std::string &i_VertexShaderPath,
			const std::string &i_FragmentShaderPath,
			const unsigned int i_windowWidth,
			const unsigned int i_windowHeight,
			const bool i_shouldRenderFullScreen); //Constructor
		GraphicsSystem(const GraphicsSystem & i_Other); //Copy Constructor
		GraphicsSystem & operator=(const GraphicsSystem & i_rhs); //Assignment Operators

		~GraphicsSystem();
		bool CreateDevice(const HWND i_mainWindow);
		bool CreateInterface(const HWND i_mainWindow);
		bool CreateVertexandIndexBuffer();
		bool CreateVertexBuffer(DWORD usage);
		bool CreateIndexBuffer(DWORD usage);
		bool CreateMaterial();

		bool Initialize();
		bool ShutDown();

	public:
		static bool CreateInstance(const HWND i_mainWindow,
			const std::string &i_VertexShaderPath,
			const std::string &i_FragmentShaderPath,
			const unsigned int i_windowWidth = 800,
			const unsigned int i_windowHeight = 600,
			const bool i_shouldRenderFullScreen = false);

		static GraphicsSystem * GetInstance();
		void Render();
		static void Destroy();
	};
}
#endif	// __EAE2014_GRAPHICS_SYSTEM_H
