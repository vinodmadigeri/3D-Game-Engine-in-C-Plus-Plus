
#include "PreCompiled.h"

#include "Game.h"
#include "CameraController.h"
#include "CameraSystem.h"
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
	CameraController::CameraController(unsigned int i_UpdateFrequency):
		IActorControllerWithReference(i_UpdateFrequency)
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
		if (++m_TicksSinceUpdate < m_UpdateFrequency)
			return;

		m_TicksSinceUpdate = 0;


		if (UserInput::GetInstance()->IsKeyPressed('C'))
		{
			CameraShake(i_Actor, i_DeltaTime, 0.01f);
		}
#if 0
		else
		{
			if (m_pOtherActor != NULL && !m_pOtherActor->IsMarkedForDeath())
			{
				float OtherActorsXLocation = m_pOtherActor->GetPosition().x();
				float OtherActorsZLocation = m_pOtherActor->GetPosition().z();
				// I'm looking at someone
				if (Engine::CameraSystem::GetInstance())
				{
					const D3DXVECTOR3 CurrentLookAt = Engine::CameraSystem::GetInstance()->GetLookAt();
					Engine::CameraSystem::GetInstance()->SetLookAt(D3DXVECTOR3(OtherActorsXLocation, CurrentLookAt.y, OtherActorsZLocation));
					i_Actor.SetPosition(OtherActorsXLocation, i_Actor.GetPosition().y(), OtherActorsZLocation);
				}
			}
		}

#else
		Vector3 Friction = Vector3(0.0f, 0.0f, 0.0f);

		i_Actor.SetFriction(Friction);
		i_Actor.SetDeltaTime(i_DeltaTime);

		char CharID = 0;
#if 0
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
#endif
		float singleAccelerationvalue = 0.00005f;

		switch (CharID)
		{
			case 'w':
			case 'W':
			{
				if (i_Actor.GetAcceleration().y() < 0.0f)
				{
					i_Actor.SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
				}

				//Set velocity of the camera in Positive Y axis since 'W' is pressed
				i_Actor.SetAcceleration(Vector3(0.0f, singleAccelerationvalue, 0.0f));
				i_Actor.SetVelocity(Vector3(0.0f, i_Actor.GetVelocity().y(), 0.0f));
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
				i_Actor.SetVelocity(Vector3(i_Actor.GetVelocity().x(), 0.0f , 0.0f));
				break;
			}

			case 's':
			case 'S':
			{
				if (i_Actor.GetAcceleration().y() > 0.0f)
				{
					i_Actor.SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
				}

				i_Actor.SetAcceleration(Vector3(0.0f, -singleAccelerationvalue, 0.0f));
				i_Actor.SetVelocity(Vector3(0.0f, i_Actor.GetVelocity().y(), 0.0f));
				break;
			}

			case 'd':
			case 'D':
			{
				if (i_Actor.GetAcceleration().x() < 0.0f)
				{
					i_Actor.SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
				}

				//Move the camera in Positive X axis since 'D' is pressed
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
	Function     : CameraShake
	Description  :
	Input        :
	Output       :
	Return Value :

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	void Camera::CameraShake(Engine::Actor &i_Actor, const float i_DeltaTime, float iShakeAmount)
	{
		using namespace Engine;

		float xRandomUnit = Engine::GenerateRandomNumber(-1.0f, 1.0f) * iShakeAmount * i_DeltaTime;
		float zRandomUnit = Engine::GenerateRandomNumber(-1.0f, 1.0f) * iShakeAmount * i_DeltaTime;

		float XCurrentPosition = i_Actor.GetPosition().x();
		float ZCurrentPosition = i_Actor.GetPosition().z();
		float YCurrrentPosition = i_Actor.GetPosition().y();
		i_Actor.SetPosition(XCurrentPosition + xRandomUnit, YCurrrentPosition, ZCurrentPosition + zRandomUnit);

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