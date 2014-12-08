
#include "PreCompiled.h"

#include "Game.h"
#include "PlayerController.h"
#include "RandomNumber.h"
#include "WorldSystem.h"
#include "UserInput.h"
#include "RenderableObjectSystem.h"
#include "ILine.h"

using namespace std;
using namespace Engine;

namespace Player
{
	static PlayerController *mpPlayerController = NULL;
	static PlayerCollisionHandler *mpPlayerCollisionHandler = NULL;
	
	/******************************************************************************
	Function     : PlayerController
	Description  : constructor, sets can shoot to true and lives to 5
	Input        : 
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	PlayerController:: PlayerController()
	{

	}

	/******************************************************************************
	Function     : ~PlayerController
	Description  : Destructor
	Input        : 
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	PlayerController:: ~PlayerController()
	{

	}

	/******************************************************************************
	 Function     : UpdateActor
	 Description  : PlayerController: Defines how actor is controlled by update actor.
	 Input        : Actor &i_Actor, const float i_DeltaTime
	 Output       : 
	 Return Value : void  
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	void PlayerController::UpdateActor(Actor &i_Actor, const float i_DeltaTime)
	{
		Vector3 Friction = Vector3(0.0f, 0.0f, 0.0f);

		i_Actor.SetFriction(Friction);
		i_Actor.SetDeltaTime(i_DeltaTime);
		
		char CharID = 0;
		const float Gravity = -0.00002f;
		//float ConstantDeceleration = Gravity;
		float AccelerationIncrementvalue = 0.0002f;

		const float MaxVelocity = 0.01f;
		
		//CONSOLE_PRINT("Acceleration: %f & Velocity: %f", i_Actor.GetAcceleration().z(), i_Actor.GetVelocity().z());
#if 1
		if (UserInput::GetInstance()->IsKeyPressed(VK_SPACE))
		{
			if (i_Actor.GetAcceleration().z() < 0)
			{
				i_Actor.SetVelocity(i_Actor.GetVelocity().x(), i_Actor.GetVelocity().y(), 0.0f);
			}

			i_Actor.SetAcceleration(Vector3(0.0f, 0.0f, AccelerationIncrementvalue));
			i_Actor.SetVelocity(i_Actor.GetVelocity().Truncated(MaxVelocity));
			
			Vector3 EndLine = i_Actor.GetPosition() + i_Actor.GetVelocity() * 200;
			//CONSOLE_PRINT("EndLIne %f, %f, %f", EndLine.x(), EndLine.y(), EndLine.z());
			sLine NewLine(i_Actor.GetPosition(), EndLine);

			RenderableObjectSystem::GetInstance()->AddDebugLines(NewLine);
		}
		else
		{
			Vector3 EndLine = i_Actor.GetPosition() + i_Actor.GetVelocity() * 200;
			//CONSOLE_PRINT("EndLIne %f, %f, %f", EndLine.x(), EndLine.y(), EndLine.z());
			sLine NewLine(i_Actor.GetPosition(), EndLine);

			RenderableObjectSystem::GetInstance()->AddDebugLines(NewLine);
			i_Actor.SetAcceleration(Vector3(0.0f, 0.0f, Gravity));
			i_Actor.SetVelocity(i_Actor.GetVelocity().Truncated(MaxVelocity));
		}
#else
		if (UserInput::GetInstance()->IsKeyPressed(VK_LEFT))
		{
			CharID = 'A';
		}
		else if (UserInput::GetInstance()->IsKeyPressed(VK_RIGHT))
		{
			CharID = 'D';
		}
		else if (UserInput::GetInstance()->IsKeyPressed(VK_UP))
		{
			CharID = 'W';
		}
		else if (UserInput::GetInstance()->IsKeyPressed(VK_DOWN))
		{
			CharID = 'S';
		}

	
		switch (CharID)
		{
			case 'w':
			case 'W':
			{
				if (i_Actor.GetAcceleration().z() < 0.0f)
				{
					i_Actor.SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
				}

				//Set velocity of the Player in Positive Y axis since 'W' is pressed
				i_Actor.SetAcceleration(Vector3(0.0f, 0.0f, singleAccelerationvalue));
				i_Actor.SetVelocity(Vector3(0.0f, 0.0f, i_Actor.GetVelocity().z()));
				break;
			}

			case 'a':
			case 'A':
			{
				if (i_Actor.GetAcceleration().x() > 0.0f)
				{
					i_Actor.SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
				}

				i_Actor.SetAcceleration(Vector3(-singleAccelerationvalue, 0.0f, 0.0f));
				i_Actor.SetVelocity(Vector3(i_Actor.GetVelocity().x(), 0.0f, 0.0f));
				break;
			}

			case 's':
			case 'S':
			{
				if (i_Actor.GetAcceleration().z() > 0.0f)
				{
					i_Actor.SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
				}

				i_Actor.SetAcceleration(Vector3(0.0f, 0.0f, -singleAccelerationvalue));
				i_Actor.SetVelocity(Vector3(0.0f, 0.0f, i_Actor.GetVelocity().z()));
				break;
			}

			case 'd':
			case 'D':
			{
				if (i_Actor.GetAcceleration().x() < 0.0f)
				{
					i_Actor.SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
				}

				//Move the Player in Positive X axis since 'D' is pressed
				i_Actor.SetAcceleration(Vector3(singleAccelerationvalue, 0.0f, 0.0f));
				i_Actor.SetVelocity(Vector3(i_Actor.GetVelocity().x(), 0.0f, 0.0f));
				break;
			}

			default: 
			{
				//Set acceleration to zero
				i_Actor.SetAcceleration(Vector3(0.0f, 0.0f, 0.0f));
				i_Actor.SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
			}
		}
#endif
		return;
	}

	/******************************************************************************
	Function     : CreateController
	Description  : Function to create controller
	Input        : 
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	void CreateControllerAndCollisionHandler(void)
	{
		if(mpPlayerController == NULL)
			mpPlayerController = new PlayerController;

		if (!mpPlayerCollisionHandler)
		{
			mpPlayerCollisionHandler = new PlayerCollisionHandler;
		}
	}

	/******************************************************************************
	Function     : GetController
	Description  : Function to get controller
	Input        : 
	Output       : Player Controller pointer
	Return Value : PlayerController *

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	PlayerController * GetController(void)
	{
		assert(mpPlayerController);

		return mpPlayerController;
	}

	/******************************************************************************
	Function     : GetCollisionHandler
	Description  : Function to get controller
	Input        :
	Output       : Player Controller pointer
	Return Value : PlayerController *

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	PlayerCollisionHandler * GetCollisionHandler(void)
	{
		assert(mpPlayerCollisionHandler);

		return mpPlayerCollisionHandler;
	}


	/******************************************************************************
	Function     : Shutdown
	Description  : Function to free player controller
	Input        : 
	Output       : Player Controller pointer
	Return Value : PlayerController *

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/	
	void ShutDown(void)
	{
		if(mpPlayerController)
			delete mpPlayerController;

		if (mpPlayerCollisionHandler)
			delete mpPlayerCollisionHandler;
	}
}