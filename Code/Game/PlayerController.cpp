
#include "PreCompiled.h"

#include "Game.h"
#include "PlayerController.h"
#include "RandomNumber.h"
#include "WorldSystem.h"
#include "UserInput.h"

using namespace std;
using namespace Engine;

namespace Player
{
	static PlayerController *mpPlayerController = NULL;
	
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
		unsigned char ucMove[4] = {'w', 'a', 's', 'd'};

		Vector3 cPlayerPosition;
		Vector3 Friction = Vector3(0.0f, 0.0f, 0.0f);

		i_Actor.SetFriction(Friction);
		i_Actor.SetDeltaTime(i_DeltaTime);

		cPlayerPosition = i_Actor.GetPosition();
		
		char CharID = 0;

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

		float singleAccelerationvalue = 0.00005f;

		switch (CharID)
		{
			case 'w':
			case 'W':
			{
				//Set velocity of the Player in Positive Y axis since 'W' is pressed
				i_Actor.SetAcceleration(Vector3(0.0f, 0.0f, singleAccelerationvalue));
				i_Actor.SetVelocity(Vector3(0.0f, 0.0f, i_Actor.GetVelocity().z()));
				CONSOLE_PRINT("Positive Z axis since 'W' is pressed");
				break;
			}

			case 'a':
			case 'A':
			{
				i_Actor.SetAcceleration(Vector3(-singleAccelerationvalue, 0.0f, 0.0f));
				i_Actor.SetVelocity(Vector3(i_Actor.GetVelocity().x(), 0.0f, 0.0f));
				CONSOLE_PRINT("negative X axis since 'A' is pressed");
				break;
			}

			case 's':
			case 'S':
			{
				i_Actor.SetAcceleration(Vector3(0.0f, 0.0f, -singleAccelerationvalue));
				i_Actor.SetVelocity(Vector3(0.0f, 0.0f, i_Actor.GetVelocity().z()));
				CONSOLE_PRINT("Negative Z axis since 'S' is pressed");
				break;
			}

			case 'd':
			case 'D':
			{
				//Move the Player in Positive X axis since 'D' is pressed
				i_Actor.SetAcceleration(Vector3(singleAccelerationvalue, 0.0f, 0.0f));
				i_Actor.SetVelocity(Vector3(i_Actor.GetVelocity().x(), 0.0f, 0.0f));
				CONSOLE_PRINT("Postive X axis since 'D' is pressed");
				break;
			}

			default: 
			{
				//Set acceleration to zero
				i_Actor.SetAcceleration(Vector3(0.0f, 0.0f, 0.0f));
				i_Actor.SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
			}
		}

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
	void CreateController(void)
	{
		if(mpPlayerController == NULL)
			mpPlayerController = new PlayerController;
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
	}
}