/*
	This file contains the function declarations for graphics
*/

#ifndef __EAE2014_GRAPHICS_H
#define __EAE2014_GRAPHICS_H

// Header Files
//=============
#include "PreCompiled.h"

// Interface
//==========

bool Initialize( const HWND i_mainWindow );
void Render();
bool ShutDown();

// Helper Functions
//=================

// Initialize
//-----------

bool CreateDevice( const HWND i_mainWindow );
bool CreateInterface( const HWND i_mainWindow );
bool CreateVertexBuffer();
bool LoadFragmentShader();
bool LoadVertexShader();

#endif	// __EAE2014_GRAPHICS_H
