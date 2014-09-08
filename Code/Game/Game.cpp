// Game.cpp : Defines the entry point for the application.
//
// Header Files
//=============
#include "PreCompiled.h"

#include "Debug.h"
#include "Game.h"
#include "GraphicsSystem.h"
#include "Win32Management.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#include <stdlib.h>
#endif

static const unsigned int g_windowWidth = 800;
static const unsigned int g_windowHeight = 600;
static const bool g_shouldRenderFullScreen = false;

MainGame::MainGame():
	mInitilized(false)

{

}

bool MainGame::Initilize(const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState)
{
	std::string sMainWindowClassName = "Vinod's Main Window Class";
	std::string sWindowCaption = "EAE2014: Vinod's Game";
	
	mInitilized = Win32Management::WindowsManager::CreateInstance(i_thisInstanceOfTheProgram, i_initialWindowDisplayState, 
																	sMainWindowClassName, sWindowCaption, g_windowWidth, 
																	g_windowHeight, g_shouldRenderFullScreen);
	if (mInitilized == false)
	{
		Engine::DebugPrint("Failed to Create Windows Loop Instance");
		return mInitilized;
	}

	std::string VertexShaderPath = "data/vertexShader.hlsl";
	std::string FragmentShaderPath = "data/fragmentShader.hlsl";
	HWND mainWindowHandle = Win32Management::WindowsManager::GetInstance()->GetReferenceToMainWindowHandle();

	mInitilized = Engine::GraphicsSystem::CreateInstance(mainWindowHandle, VertexShaderPath, FragmentShaderPath);

	if (mInitilized == false)
	{
		Engine::DebugPrint("Failed to Create Graphics Engine Instance");
		return mInitilized;
	}

	return mInitilized;
}

int MainGame::Run(void)
{
	int exitCode = -1;
	
	//Game Loop
	if (mInitilized)
	{
		bool QuitRequested = false;
		do
		{
			Engine::GraphicsSystem::GetInstance()->Render();
			Win32Management::WindowsManager::GetInstance()->UpdateMainWindow(exitCode, QuitRequested);
			
		} while (QuitRequested == false);
	}

	Engine::DebugPrint("End of Game Loop");

	return exitCode;
}

void MainGame::Shutdown(const HINSTANCE i_thisInstanceOfTheProgram)
{
	if (mInitilized)
	{
		Engine::GraphicsSystem::Destroy();
		Win32Management::WindowsManager::Destroy();
	}

	Engine::DebugPrint("ShutDown");
}









// Entry Point
//============

int WINAPI WinMain(
	// When you see a type in capital letters that starts with an "H"
	// it is a "handle" to something
	// (for example, an HSOMETHING would be handle to a "something").
	// A "handle" is kind of like a pointer:
	// It's an object that is used to represent a different object.
	// This first parameter, then, is a handle to the specific instance
	// of the program that is being run currently.
	// If someone runs this program twice then WinMain() will be called twice,
	// each time with a different handle.
	// When you interact with Windows and want to let it know which specific
	// instance of the program you are referring to you can use this handle.
	HINSTANCE i_thisInstanceOfTheProgram,
	// This instance handle is here for legacy reasons and is unused
	HINSTANCE,
	char* i_commandLineArguments,
	int i_initialWindowDisplayState)
{

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
#endif

	MainGame Game;
	int exitCode = -1;
	
	if (true == Game.Initilize(i_thisInstanceOfTheProgram, i_initialWindowDisplayState))
	{
		exitCode = Game.Run();
		Game.Shutdown(i_thisInstanceOfTheProgram);
	}

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	// A Windows program doesn't actually need any windows at all
	// but in most cases there will be a single "main" window
	// and when it is closed the program will exit
	// Unlike standard C/C++ programs there is no standardized return value
	// to indicate that the program "succeeded".
	// Windows itself completely ignores the value that the program returns,
	// and so it is only useful if you have other programs that
	// are paying attention and expecting specific values.
	return exitCode;
}
