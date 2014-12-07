// Game.cpp : Defines the entry point for the application.
//
// Header Files
//=============
#include "PreCompiled.h"
#include "Resources\Resource.h"
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
#include "LightingSystem.h"
#include "CollisionSystem.h"
#include "Sprite.h"
#include "MathUtil.h"
#include "PlayerController.h"
#include "CameraController.h"
#include "LevelLoadHelper.h"
#include "LightController.h"
#include "Win32Management.h"
#include "DebugLineRenderer.h"
#include "../Engine/UserSettings/UserSettings.h"

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
																sMainWindowClassName, sWindowCaption, IDI_GAME, IDI_SMALL);
	if (mInitilized == false)
	{
		Engine::DebugPrint("Failed to Create Windows Loop Instance");
		return mInitilized;
	}

	HWND mainWindowHandle = Win32Management::WindowsManager::GetInstance()->GetReferenceToMainWindowHandle();

	mInitilized = Engine::RenderableObjectSystem::CreateInstance(mainWindowHandle);

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

	mInitilized = Engine::CollisionSystem::CreateInstance();

	if (mInitilized == false)
	{
		Engine::DebugPrint("Failed to Create CollisionSystem Instance");
		return mInitilized;
	}

	using namespace Engine;
	
	//Load the first level
	if (!LoadLevel("data/Start.level"))
	{
		return false;
	}

	std::vector< SharedPointer<Actor>> CubeActorsList = WorldSystem::GetInstance()->FindActorsByType("Cube");
	
	Player::CreateControllerAndCollisionHandler();

	for (unsigned int i = 0; i < CubeActorsList.size(); i++)
	{
		CubeActorsList.at(i)->SetController(Player::GetController());
		CubeActorsList.at(i)->SetCollisionHandler(Player::GetCollisionHandler());
	}

	Camera::CreateController();
	CameraSystem::GetInstance()->m_WorldObject->SetController(Camera::GetController());
	
	Light::CreateController();
	LightingSystem::GetInstance()->m_WorldObject->SetController(Light::GetController());

	const Engine::sRectangle Spriteposition = Engine::sRectangle(-1.0f, 0.0f, 1.0f, 0.0f);
	const Engine::sRectangle Spritetextcoord = Engine::sRectangle(0.0f, 1.0f, 0.0f, 1.0f);

	Engine::RenderableObjectSystem::GetInstance()->CreateSprite("data/logo.dds", &Spriteposition, &Spritetextcoord);

	const Engine::sRectangle position = Engine::sRectangle(0.5f, 1.0f, 1.0f, 0.5f);
	const Engine::sRectangle textcoord = Engine::sRectangle(0.0f, 1.0f, 0.0f, 1.0f);

	const unsigned int MaxHorizontalCount = 10;
	Engine::RenderableObjectSystem::GetInstance()->CreateSprite("data/numbers.dds", &position, &textcoord, MaxHorizontalCount);

	RenderableObjectSystem::GetInstance()->CreateDebugLines("ActorDebugLine", 30);

	return mInitilized;
}


int MainGame::Run(void)
{
	int exitCode = -1;
	
	Engine::HighResTimer GameTimer;
	GameTimer.Initilize();
	LONGLONG PreviousTimeStamp = GameTimer.GetCurrentTimeStamp();
	const unsigned int MaxHorizontalCount = 10;
	unsigned int SpriteCount = 0;
	
	using namespace Engine;
	std::vector< SharedPointer<Actor>> CubeActorsList = WorldSystem::GetInstance()->FindActorsByType("Cube");
	std::vector< SharedPointer<Actor>> TorusActorsList = WorldSystem::GetInstance()->FindActorsByType("Torus");
	std::vector< SharedPointer<Actor>> HelixActorsList = WorldSystem::GetInstance()->FindActorsByType("Helix");

	//Game Loop
	if (mInitilized)
	{
		bool QuitRequested = false;
		do
		{
			GameTimer.CalculateFrameTime();
			float DeltaTime = static_cast<float>(GameTimer.GetLastFrameMS());
			Engine::WorldSystem::GetInstance()->ActorsUpdate(DeltaTime);
			Engine::CollisionSystem::GetInstance()->Update(DeltaTime);
			Engine::PhysicsSystem::GetInstance()->ApplyEulerPhysics(DeltaTime);
			Engine::CameraSystem::GetInstance()->Update(DeltaTime);
			Engine::LightingSystem::GetInstance()->Update(DeltaTime);
			
			sLine MovingNewLine(CubeActorsList[0]->GetPosition(), CubeActorsList[1]->GetPosition());
			RenderableObjectSystem::GetInstance()->AddDebugLines(MovingNewLine);

			sLine OneEdgeMovingLine(CubeActorsList[0]->GetPosition(), TorusActorsList[0]->GetPosition());
			RenderableObjectSystem::GetInstance()->AddDebugLines(OneEdgeMovingLine);

			OneEdgeMovingLine = sLine(CubeActorsList[1]->GetPosition(), HelixActorsList[0]->GetPosition());
			RenderableObjectSystem::GetInstance()->AddDebugLines(OneEdgeMovingLine);

			Engine::RenderableObjectSystem::GetInstance()->Render();
			Win32Management::WindowsManager::GetInstance()->UpdateMainWindow(exitCode, QuitRequested);
			
			Engine::RenderableObjectSystem::RenderableSprites * NumbersSprite = Engine::RenderableObjectSystem::GetInstance()->FindSpriteGameObjectByName("data/numbers.dds");
			
			
			double TimePast = GameTimer.GetTimeDifferenceinMS(PreviousTimeStamp);

			if (TimePast > 2000)
			{
				PreviousTimeStamp = GameTimer.GetCurrentTimeStamp();
				NumbersSprite->GetSprite()->DrawFromSpriteSheet(SpriteCount);
				SpriteCount = (SpriteCount + 1) % MaxHorizontalCount;
			}

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
		Engine::LightingSystem::Destroy();
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
