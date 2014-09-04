/*
	This file contains the function declarations for graphics
*/

#ifndef __EAE2014_GRAPHICS_SYSTEM_H
#define __EAE2014_GRAPHICS_SYSTEM_H

// Header Files
//=============
#include "PreCompiled.h"

#include <d3d9.h>

class GraphicsSystem
{
private:
	bool mInitilized;
	static GraphicsSystem *m_pInstance;
	HWND m_mainWindow;
	IDirect3D9* m_direct3dInterface;
	IDirect3DDevice9* m_direct3dDevice;
	IDirect3DVertexDeclaration9* m_vertexDeclaration;
	IDirect3DVertexBuffer9* m_vertexBuffer;
	IDirect3DVertexShader9* m_vertexShader;
	IDirect3DPixelShader9* m_fragmentShader;
	std::string m_VertexShaderFilePath;
	std::string m_FragmentShaderFilePath;
	
	// The vertex information that is sent to the display adaptor must match what the vertex shader expects
	struct sVertex
	{
		float x, y;
	};

	GraphicsSystem(const HWND i_mainWindow, const std::string &i_VertexShaderPath, const std::string &i_FragmentShaderPath); //Constructor
	GraphicsSystem(const GraphicsSystem & i_Other); //Copy Constructor
	GraphicsSystem & operator=(const GraphicsSystem & i_rhs); //Assignment Operators

	~GraphicsSystem();
	bool CreateDevice(const HWND i_mainWindow);
	bool CreateInterface(const HWND i_mainWindow);
	bool CreateVertexBuffer();
	bool LoadFragmentShader();
	bool LoadVertexShader();

	bool Initialize();
	bool ShutDown();

public:
	static GraphicsSystem * CreateInstance(const HWND i_mainWindow, const std::string &i_VertexShaderPath, const std::string &i_FragmentShaderPath);
	static GraphicsSystem * GetInstance();
	void Render();
	static void Destroy();
};

#endif	// __EAE2014_GRAPHICS_SYSTEM_H
