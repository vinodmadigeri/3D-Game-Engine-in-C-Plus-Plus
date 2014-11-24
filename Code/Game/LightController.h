#ifndef __LIGHT_CONTROLLER_H
#define __LIGHT_CONTROLLER_H

#include "PreCompiled.h"

#include "Actor.h"
#include "ActorController.h"

namespace Light
{
	class LightController :public Engine::IActorController
	{
	public:
		LightController();
		virtual ~LightController();
		virtual void UpdateActor(Engine::Actor &i_Actor, const float i_DeltaTime);
	};

	void CreateController(void);
	LightController * GetController(void);
	void ShutDown(void);
}

#endif //__LIGHT_CONTROLLER_H