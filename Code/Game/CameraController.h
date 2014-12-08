#ifndef __CAMERA_CONTROLLER_H
#define __CAMERA_CONTROLLER_H

#include "PreCompiled.h"

#include "Actor.h"
#include "ActorReference.h"

namespace Camera
{
	class CameraController :public Engine::IActorControllerWithReference
	{
	public:
		CameraController(unsigned int i_UpdateFrequency = 1);
		virtual ~CameraController();
		virtual void UpdateActor(Engine::Actor &i_Actor, const float i_DeltaTime);
	};

	void CreateController(void);
	CameraController * GetController(void);
	void CameraShake(Engine::Actor &i_Actor, const float i_DeltaTime, float iShakeAmount);
	void ShutDown(void);
}

#endif //__CAMERA_CONTROLLER_H