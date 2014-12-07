#ifndef __COLLISION_HANDLER_H
#define __COLLISION_HANDLER_H

#include "PreCompiled.h"

namespace Engine
{

	class CollisionObject;
}

namespace Engine
{
	class ICollisionHandlerInterface
	{
	public:
		virtual void Handler(CollisionObject *ThisCollisionObject, CollisionObject *OtherCollisionObject) = 0;
		virtual ~ICollisionHandlerInterface(){}
	} ;
}
#endif //__COLLISION_HANDLER_H