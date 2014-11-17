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
#include "CameraSystem.h"
#include "Win32Management.h"
#include "MathUtil.h"
#include "PlayerController.h"
#include "CameraController.h"

#include "../Tools/UserSettings/UserSettings.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#include <stdlib.h>
#endif

MainGame::MainGame():
	mInitilized(false)

{

}

bool MainGame::Initilize(const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState)
{
	std::string sMainWindowClassName = "Vinod's Main Window Class";
	std::string sWindowCaption = "EAE2014: Vinod's Game";
	
	mInitilized = Win32Management::WindowsManager::CreateInstance(i_thisInstanceOfTheProgram, i_initialWindowDisplayState, 
																	sMainWindowClassName, sWindowCaption, UserSettings::GetWidth(), 
																	UserSettings::GetHeight(), UserSettings::IsFullScreenModeEnabled());
	if (mInitilized == false)
	{
		Engine::DebugPrint("Failed to Create Windows Loop Instance");
		return mInitilized;
	}

	HWND mainWindowHandle = Win32Management::WindowsManager::GetInstance()->GetReferenceToMainWindowHandle();

	mInitilized = Engine::RenderableObjectSystem::CreateInstance(mainWindowHandle, UserSettings::GetWidth(),
													UserSettings::GetHeight(), UserSettings::IsFullScreenModeEnabled());

	if (mInitilized == false)
	{
		Engine::DebugPrint("Failed to Create Graphics Engine Instance");
		return mInitilized;
	}
	
	//Order of physics system and camera system is important
	mInitilized = Engine::PhysicsSystem::CreateInstance();

	if (mInitilized == false)
	{
		Engine::DebugPrint("Failed to Create PhysicsSystem Instance");
		return mInitilized;
	}

	Engine::Vector3 eyeLocation(0.0f, 0.0f, -11.0f);
	Engine::Vector3 LookAtLocation(0.0f, 0.0f, 1.0f);
	Engine::Vector3 Up(0.0f, 1.0f, 0.0f);
	float FieldOfView = static_cast<float>(Engine::Get_PI_Value() / 3); //60 degrees
	float NearPlane = 0.1f;
	float FarPlane = 100.0f;
	
	mInitilized = Engine::CameraSystem::CreateInstance(UserSettings::GetWidth(),UserSettings::GetHeight(), FieldOfView, 
														NearPlane, FarPlane, eyeLocation, LookAtLocation, Up);

	if (mInitilized == false)
	{
		Engine::DebugPrint("Failed to Create WorldSystem Instance");
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

	using namespace Engine;
	//------Input Actor Data and Create one-------------

	//Create DirectX Cube
	{
		Vector3 Position = Vector3(-3.0f, 0.0f, -2.0f);
		Vector3 Velocity = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 Acceleration = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 Size = Vector3(1.0f, 1.0f, 0.0f);
		float Rotation = 0.0f;

		const char * pMaterialPath = "data/cubeAMDMaterial.mat";
		const char * pMeshPath = "data/cube.dat";

		WorldSystem::GetInstance()->CreateActors(Position, Velocity, Acceleration, "CubeDirectX", "Cube", Size, Rotation, pMaterialPath, pMeshPath);
	}

	//Create Nvidia Cube
	{
		Vector3 Position = Vector3(3.0f, 0.0f, -2.0f);
		Vector3 Velocity = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 Acceleration = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 Size = Vector3(1.0f, 1.0f, 0.0f);
		float Rotation = 0.0f;

		const char * pMaterialPath = "data/cubeNvidiaMaterial.mat";
		const char * pMeshPath = "data/cubeNvidia.dat";

		WorldSystem::GetInstance()->CreateActors(Position, Velocity, Acceleration, "CubeNvidia1", "Cube", Size, Rotation, pMaterialPath, pMeshPath);
	}


	//Create Plane
	{
		Vector3 Position = Vector3(0.0f, -1.0f, 0.0f);
		Vector3 Velocity = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 Acceleration = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 Size = Vector3(1.0f, 1.0f, 0.0f);
		float Rotation = 0.0f;

		const char * pMaterialPath = "data/planeMaterial.mat";
		const char * pMeshPath = "data/plane.dat";

		WorldSystem::GetInstance()->CreateActors(Position, Velocity, Acceleration, "BaseQuad", "BaseQuad", Size, Rotation, pMaterialPath, pMeshPath);
	}

	std::vector< SharedPointer<Actor>> ActorsList = WorldSystem::GetInstance()->FindActorsByType("Cube");

	Player::CreateController();

	for (unsigned int i = 0; i < ActorsList.size(); i++)
	{
		ActorsList.at(i)->SetController(Player::GetController());
	}

	Camera::CreateController();
	CameraSystem::GetInstance()->m_WorldObject->SetController(Camera::GetController());

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
			Engine::CameraSystem::GetInstance()->Update(static_cast<float>(GameTimer.GetLastFrameMS()));
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
		Player::ShutDown();
		Camera::ShutDown();
		Engine::PhysicsSystem::Destroy();
		Engine::UserInput::Destroy();
		Engine::WorldSystem::Destroy();
		Engine::CameraSystem::Destroy();
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
	//_CrtSetBreakAlloc(148);
#endif

	MainGame Game;
	int exitCode = -1;
	
	if (true == Game.Initilize(i_thisInstanceOfTheProgram, i_initialWindowDisplayState))
	{
		exitCode = Game.Run();
		Game.Shutdown(i_thisInstanceOfTheProgram);
	}

#ifdef _DEBUG
	//_CrtDumpMemoryLeaks();
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
