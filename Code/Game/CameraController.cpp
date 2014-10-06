
#include "PreCompiled.h"

#include "Game.h"
#include "CameraController.h"
#include "RandomNumber.h"
#include "WorldSystem.h"
#include "UserInput.h"

using namespace std;
using namespace Engine;

namespace Camera
{
	static CameraController *mpCameraController = NULL;

	/******************************************************************************
	Function     : CameraController
	Description  : constructor
	Input        :
	Output       :
	Return Value :

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	CameraController::CameraController()
	{

	}

	/******************************************************************************
	Function     : ~CameraController
	Description  : Destructor
	Input        :
	Output       :
	Return Value :

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	CameraController:: ~CameraController()
	{

	}

	/******************************************************************************
	Function     : UpdateActor
	Description  : CameraController: Defines how camera is controlled by update actor.
	Input        : Actor &i_Actor, const float i_DeltaTime
	Output       :
	Return Value : void
	Data Accessed:
	Data Updated :

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	void CameraController::UpdateActor(Actor &i_Actor, const float i_DeltaTime)
	{
		unsigned char ucMove[4] = { 'w', 'a', 's', 'd' };

		Vector3 cPlayerPosition;
		Vector3 Friction = Vector3(0.0f, 0.0f, 0.0f);

		i_Actor.SetFriction(Friction);
		i_Actor.SetDeltaTime(i_DeltaTime);

		cPlayerPosition = i_Actor.GetPosition();

		char CharID = 0;

		if (UserInput::GetInstance()->IsKeyPressed('A'))
		{
			CharID = 'A';
		}
		else if (UserInput::GetInstance()->IsKeyPressed('D'))
		{
			CharID = 'D';
		}
		else if (UserInput::GetInstance()->IsKeyPressed('W'))
		{
			CharID = 'W';
		}
		else if (UserInput::GetInstance()->IsKeyPressed('S'))
		{
			CharID = 'S';
		}

		float singleAccelerationvalue = 0.00005f;

		switch (CharID)
		{
			case 'w':
			case 'W':
			{
				//Set velocity of the camera in Positive Y axis since 'W' is pressed
				i_Actor.SetAcceleration(Vector3(0.0f, singleAccelerationvalue, 0.0f));
				i_Actor.SetVelocity(Vector3(0.0f, i_Actor.GetVelocity().y(), 0.0f));
				CONSOLE_PRINT("Positive Y axis since 'W' is pressed");
				break;
			}

			case 'a':
			case 'A':
			{
				i_Actor.SetAcceleration(Vector3(-singleAccelerationvalue, 0.0f, 0.0f));
				i_Actor.SetVelocity(Vector3(i_Actor.GetVelocity().x(), 0.0f , 0.0f));
				CONSOLE_PRINT("Negative X axis since 'A' is pressed");
				break;
			}

			case 's':
			case 'S':
			{
				i_Actor.SetAcceleration(Vector3(0.0f, -singleAccelerationvalue, 0.0f));
				i_Actor.SetVelocity(Vector3(0.0f, i_Actor.GetVelocity().y(), 0.0f));
				CONSOLE_PRINT("Negative Y axis since 'S' is pressed");
				break;
			}

			case 'd':
			case 'D':
			{
				//Move the camera in Positive X axis since 'D' is pressed
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
		if (mpCameraController == NULL)
			mpCameraController = new CameraController;
	}

	/******************************************************************************
	Function     : GetController
	Description  : Function to get controller
	Input        :
	Output       : camera Controller pointer
	Return Value : mpCameraController *

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	CameraController * GetController(void)
	{
		assert(mpCameraController);

		return mpCameraController;
	}

	/******************************************************************************
	Function     : Shutdown
	Description  : Function to free controller
	Input        :
	Output       : camera Controller pointer
	Return Value : mpCameraController *

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	void ShutDown(void)
	{
		if (mpCameraController)
			delete mpCameraController;
	}
}