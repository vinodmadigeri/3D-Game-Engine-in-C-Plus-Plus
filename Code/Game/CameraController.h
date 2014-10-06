#ifndef __CAMERA_CONTROLLER_H
#define __CAMERA_CONTROLLER_H

#include "PreCompiled.h"

#include "Actor.h"
#include "ActorController.h"

namespace Camera
{
	class CameraController :public Engine::IActorController
	{
	public:
		CameraController();
		virtual ~CameraController();
		virtual void UpdateActor(Engine::Actor &i_Actor, const float i_DeltaTime);
	};

	void CreateController(void);
	CameraController * GetController(void);
	void ShutDown(void);
}

#endif //__CAMERA_CONTROLLER_H