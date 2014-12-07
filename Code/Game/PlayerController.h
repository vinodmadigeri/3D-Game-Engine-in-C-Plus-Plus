#ifndef __PLAYER_CONTROLLER_H
#define __PLAYER_CONTROLLER_H

#include "PreCompiled.h"

#include "Actor.h"
#include "ActorController.h"
#include "CollisionHandler.h"
#include "CollisionSystem.h"
#include "Debug.h"

namespace Player
{
	class PlayerController :public Engine::IActorController
	{
	public:
		PlayerController();
		virtual ~PlayerController();
		virtual void UpdateActor(Engine::Actor &i_Actor, const float i_DeltaTime);
	} ;

	class PlayerCollisionHandler :public Engine::ICollisionHandlerInterface
	{
	public:
		PlayerCollisionHandler(){};
		virtual ~PlayerCollisionHandler(){};
		virtual void Handler(Engine::CollisionObject *ThisCollisionObject, Engine::CollisionObject *OtherCollisionObject)
		{
			assert(ThisCollisionObject && OtherCollisionObject);

			unsigned int MyCollisionBit = ThisCollisionObject->m_WorldObject->mCollidesWithBitIndex;
			unsigned int OtherObjectCollisionBit = OtherCollisionObject->m_WorldObject->mClassBitIndex;

			if (MyCollisionBit & OtherObjectCollisionBit)
			{
				Engine::DebugPrint("%s Collided with %s", ThisCollisionObject->m_WorldObject->GetName(), OtherCollisionObject->m_WorldObject->GetName());
			}
		}
	};

	void CreateControllerAndCollisionHandler(void);
	PlayerController * GetController(void);
	PlayerCollisionHandler * GetCollisionHandler(void);
	void ShutDown(void);
}

#endif //__PLAYER_CONTROLLER_H