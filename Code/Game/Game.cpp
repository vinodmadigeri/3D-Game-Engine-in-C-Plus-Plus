// Game.cpp : Defines the entry point for the application.
//
// Header Files
//=============
#include "PreCompiled.h"

#include "Debug.h"
#include "Game.h"
#include "RenderableObjectSystem.h"
#include "MeshData.h"
#include "Vector3.h"
#include "HighResTime.h"
#include "WorldSystem.h"
#include "UserInput.h"
#include "PhysicsSystem.h"

#include "Win32Management.h"


#include "PlayerController.h"

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

	HWND mainWindowHandle = Win32Management::WindowsManager::GetInstance()->GetReferenceToMainWindowHandle();

	mInitilized = Engine::RenderableObjectSystem::CreateInstance(mainWindowHandle, g_windowWidth,
													g_windowHeight, g_shouldRenderFullScreen);

	if (mInitilized == false)
	{
		Engine::DebugPrint("Failed to Create Graphics Engine Instance");
		return mInitilized;
	}

	mInitilized = Engine::WorldSystem::CreateInstance();

	if (mInitilized == false)
	{
		Engine::DebugPrint("Failed to Create WorldSystem Instance");
		return mInitilized;
	}

	mInitilized = Engine::UserInput::CreateInstance();

	if (mInitilized == false)
	{
		Engine::DebugPrint("Failed to Create UserInput Instance");
		return mInitilized;
	}

	mInitilized = Engine::PhysicsSystem::CreateInstance();

	if (mInitilized == false)
	{
		Engine::DebugPrint("Failed to Create PhysicsSystem Instance");
		return mInitilized;
	}
	using namespace Engine;
	//------Input Actor Data and Create one-------------
	{
		Vector3 Position = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 Velocity = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 Acceleration = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 Size = Vector3(1.0f, 1.0f, 0.0f);
		float Rotation = 0.0f;

		DrawInfo DrawInfoData;
		DrawInfoData.m_PrimitiveType = D3DPT_TRIANGLELIST;
		DrawInfoData.m_PrimitiveCount = 12;
		DrawInfoData.m_NumOfVertices = 8;
		DrawInfoData.m_indexOfFirstVertexToRender = 0;
		DrawInfoData.m_indexOfFirstIndexToUse = 0;
		sVertexData vertexData[8]; //m_NumOfVertices
		vertexData[0].x = -0.2f;
		vertexData[0].y = -0.2f;
		vertexData[0].z = -0.2f;
		vertexData[0].color = D3DCOLOR_XRGB(20, 60, 92);

		vertexData[4].x = -0.2f;
		vertexData[4].y = -0.2f;
		vertexData[4].z = 0.2f;
		vertexData[4].color = D3DCOLOR_XRGB(20, 60, 92);

		vertexData[1].x = -0.2f;
		vertexData[1].y = 0.2f;
		vertexData[1].z = -0.2f;
		vertexData[1].color = D3DCOLOR_XRGB(15, 75, 220);


		vertexData[5].x = -0.2f;
		vertexData[5].y = 0.2f;
		vertexData[5].z = 0.2f;
		vertexData[5].color = D3DCOLOR_XRGB(15, 75, 220);


		vertexData[2].x = 0.2f;
		vertexData[2].y = 0.2f;
		vertexData[2].z = -0.2f;
		vertexData[2].color = D3DCOLOR_XRGB(92, 50, 38);


		vertexData[6].x = 0.2f;
		vertexData[6].y = 0.2f;
		vertexData[6].z = 0.2f;
		vertexData[6].color = D3DCOLOR_XRGB(92, 50, 38);

		vertexData[3].x = 0.2f;
		vertexData[3].y = -0.2f;
		vertexData[3].z = -0.2f;
		vertexData[3].color = D3DCOLOR_XRGB(20, 150, 80);
		
		vertexData[7].x = 0.2f;
		vertexData[7].y = -0.2f;
		vertexData[7].z = 0.2f;
		vertexData[7].color = D3DCOLOR_XRGB(20, 150, 80);

		DrawInfoData.m_pVerticesData = vertexData;
		DrawInfoData.m_VertexStride = sizeof(sVertexData);

		const unsigned int verticesPerTriangle = 3;
		const unsigned int trianglesPerRectangle = 2;
		const unsigned int NumberOfRectangles = 6;
		DrawInfoData.m_IndexCount = NumberOfRectangles * trianglesPerRectangle * verticesPerTriangle;
		DWORD32 indices[36] = 
		{
			0, 1, 3,
			3, 1, 2,
			3, 2, 7,
			7, 2, 6,
			0, 4 ,1,
			1, 4, 5,
			4, 7, 5,
			5, 7, 6,
			2, 1, 5,
			2, 5, 6,
			0, 3, 4,
			4, 3, 7
		};

		DrawInfoData.m_pIndices = indices;

		const char * pMaterialPath = "data/Simple.mat.lua";

		WorldSystem::GetInstance()->CreateActors(Position, Velocity, Acceleration, "Rectangle", "Rectangle", Size, Rotation, pMaterialPath, DrawInfoData);
	}

#if 0
	{
		Vector3 Position = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 Velocity = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 Acceleration = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 Size = Vector3(1.0f, 1.0f, 0.0f);
		float Rotation = 0.0f;

		DrawInfo DrawInfoDataSecondMesh;
		DrawInfoDataSecondMesh.m_PrimitiveType = D3DPT_TRIANGLELIST;
		DrawInfoDataSecondMesh.m_PrimitiveCount = 2;
		DrawInfoDataSecondMesh.m_NumOfVertices = 4;
		DrawInfoDataSecondMesh.m_indexOfFirstVertexToRender = 0;
		DrawInfoDataSecondMesh.m_indexOfFirstIndexToUse = 0;
		sVertexData vertexData[4]; //m_NumOfVertices
		vertexData[0].x = -0.4f;
		vertexData[0].y = -0.4f;
		vertexData[0].color = D3DCOLOR_XRGB(20, 60, 92);

		vertexData[1].x = -0.4f;
		vertexData[1].y = 0.4f;
		vertexData[1].color = D3DCOLOR_XRGB(15, 75, 220);

		vertexData[2].x = 0.4f;
		vertexData[2].y = 0.4f;
		vertexData[2].color = D3DCOLOR_XRGB(44, 99, 150);

		vertexData[3].x = 0.4f;
		vertexData[3].y = -0.4f;
		vertexData[3].color = D3DCOLOR_XRGB(52, 200, 20);

		DrawInfoDataSecondMesh.m_pVerticesData = vertexData;
		DrawInfoDataSecondMesh.m_VertexStride = sizeof(sVertexData);

		const unsigned int verticesPerTriangle = 3;
		const unsigned int trianglesPerRectangle = 2;
		DWORD32 indices[6];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 3;

		// A second triangle could be filled in like this:
		indices[3] = 1;
		indices[4] = 2;
		indices[5] = 3;
		DrawInfoDataSecondMesh.m_pIndices = indices;
		DrawInfoDataSecondMesh.m_IndexCount = trianglesPerRectangle * verticesPerTriangle;

		const char * pMaterialPath = "data/Simple.mat.lua";
		WorldSystem::GetInstance()->CreateActors(Position, Velocity, Acceleration, "Rectangle", "Rectangle", Size, Rotation, pMaterialPath, DrawInfoDataSecondMesh);
	}
#endif

	std::vector< SharedPointer<Actor>> ActorsList = WorldSystem::GetInstance()->FindActorsByType("Rectangle");

	Player::CreateController();

	for (unsigned int i = 0; i < ActorsList.size(); i++)
	{
		ActorsList.at(i)->SetController(Player::GetController());
		break;
	}

	return mInitilized;
}


int MainGame::Run(void)
{
	int exitCode = -1;
	
	Engine::HighResTimer GameTimer;
	GameTimer.Initilize();
	//Game Loop
	if (mInitilized)
	{
		bool QuitRequested = false;
		do
		{
			GameTimer.CalculateFrameTime();
			Engine::WorldSystem::GetInstance()->ActorsUpdate(static_cast<float>(GameTimer.GetLastFrameMS()));
			Engine::PhysicsSystem::GetInstance()->ApplyEulerPhysics(static_cast<float>(GameTimer.GetLastFrameMS()));
			Engine::RenderableObjectSystem::GetInstance()->Render();
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
		Engine::PhysicsSystem::Destroy();
		Engine::UserInput::Destroy();
		Engine::WorldSystem::Destroy();
		Engine::RenderableObjectSystem::Destroy();
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
