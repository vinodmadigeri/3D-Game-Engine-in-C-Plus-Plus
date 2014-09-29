#ifndef __PLAYER_CONTROLLER_H
#define __PLAYER_CONTROLLER_H

#include "PreCompiled.h"

#include "Actor.h"
#include "ActorController.h"

namespace Player
{
	class PlayerController :public Engine::IActorController
	{
	public:
		PlayerController();
		virtual ~PlayerController();
		virtual void UpdateActor(Engine::Actor &i_Actor, const float i_DeltaTime);
	} ;

	void CreateController(void);
	PlayerController * GetController(void);
	void ShutDown(void);
}

#endif //__PLAYER_CONTROLLER_H