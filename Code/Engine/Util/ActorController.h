#ifndef __ACTOR_CONTROLLER_H
#define __ACTOR_CONTROLLER_H

#include "PreCompiled.h"

namespace Engine
{
	class Actor;
}

namespace Engine
{
	class IActorController
	{

	public:
		virtual ~IActorController()
		{

		}

		virtual void UpdateActor(Actor &i_Actor, const float i_DeltaTime) = 0;
	};
}
#endif //__ACTOR_CONTROLLER_H
