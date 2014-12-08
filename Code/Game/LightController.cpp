
#include "PreCompiled.h"

#include "Game.h"
#include "LightingSystem.h"
#include "LightController.h"
#include "RandomNumber.h"
#include "WorldSystem.h"
#include "UserInput.h"

using namespace std;
using namespace Engine;

namespace Light
{
	static LightController *mpLightController = NULL;

	/******************************************************************************
	Function     : LightController
	Description  : constructor
	Input        :
	Output       :
	Return Value :

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	LightController::LightController()
	{

	}

	/******************************************************************************
	Function     : ~LightController
	Description  : Destructor
	Input        :
	Output       :
	Return Value :

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	LightController:: ~LightController()
	{

	}

	/******************************************************************************
	Function     : UpdateActor
	Description  : LightController: Defines how light direction is controlled by update actor.
	Input        : Actor &i_Actor, const float i_DeltaTime
	Output       :
	Return Value : void
	Data Accessed:
	Data Updated :

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	void LightController::UpdateActor(Actor &i_Actor, const float i_DeltaTime)
	{
		i_Actor.SetDeltaTime(i_DeltaTime);

		Vector3 Offset(0.0f, 0.0f, 0.0f);

		char CharID = 0;

		if (UserInput::GetInstance()->IsKeyPressed('H'))
		{
			CharID = 'H';
		}
		else if (UserInput::GetInstance()->IsKeyPressed('K'))
		{
			CharID = 'K';
		}
		else if (UserInput::GetInstance()->IsKeyPressed('U'))
		{
			CharID = 'U';
		}
		else if (UserInput::GetInstance()->IsKeyPressed('J'))
		{
			CharID = 'J';
		}

		float singleAccelerationvalue = 0.001f;

		bool KeyPressed = false;

#if 1
		{
			static bool Toggle = true;
			Vector3 Direction = Engine::LightingSystem::GetInstance()->GetLightDirection();

			if (Direction.x() < -0.9f)
				Toggle = false;

			if (Direction.x() > 0.9f)
				Toggle = true;

			if (Toggle)
			{
				KeyPressed = true;
				CharID = 'H';
			}
			else if (!Toggle)
			{

				KeyPressed = true;
				CharID = 'K';
			}

			singleAccelerationvalue = singleAccelerationvalue / 2.0f;
		}
#endif

		switch (CharID)
		{
			case 'u':
			case 'U':
			{
				Offset = Vector3(0.0f, 0.0f, singleAccelerationvalue);
				KeyPressed = true;
				break;
			}

			case 'h':
			case 'H':
			{
				Offset = Vector3(-singleAccelerationvalue, 0.0f, 0.0f);
				KeyPressed = true;
				break;
			}

			case 'j':
			case 'J':
			{
				Offset = Vector3(0.0f, 0.0f, -singleAccelerationvalue);
				KeyPressed = true;
				break;
			}

			case 'k':
			case 'K':
			{
				Offset = Vector3(singleAccelerationvalue, 0.0f, 0.0f);
				KeyPressed = true;
				break;
			}

			default:
			{
				break;
			}
		}

		if (KeyPressed == true)
		{
			Vector3 CurrentXZPosition = Engine::LightingSystem::GetInstance()->m_WorldObject->GetPosition();

			CurrentXZPosition += Offset * i_DeltaTime;

			Engine::LightingSystem::GetInstance()->m_WorldObject->SetPosition(CurrentXZPosition);

			Vector3 Direction = Engine::LightingSystem::GetInstance()->GetLightDirection();

			Direction.SetCoordinates(CurrentXZPosition.x(), Direction.y(), CurrentXZPosition.z());
			Direction = Direction.Normalized();

			Engine::LightingSystem::GetInstance()->SetLightDirection(Direction);
			
			//CONSOLE_PRINT("Direction = %f, %f, %f", Direction.x(), Direction.y(), Direction.z());
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
		if (mpLightController == NULL)
			mpLightController = new LightController;
	}

	/******************************************************************************
	Function     : GetController
	Description  : Function to get controller
	Input        :
	Output       : light Controller pointer
	Return Value : mpLightController *

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	LightController * GetController(void)
	{
		assert(mpLightController);

		return mpLightController;
	}

	/******************************************************************************
	Function     : Shutdown
	Description  : Function to free controller
	Input        :
	Output       : light Controller pointer
	Return Value : mpLightController *

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	void ShutDown(void)
	{
		if (mpLightController)
			delete mpLightController;
	}
}