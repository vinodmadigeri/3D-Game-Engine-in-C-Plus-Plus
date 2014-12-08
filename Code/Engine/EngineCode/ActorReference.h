#ifndef __ACTOR_REFERENCE_H
#define __ACTOR_REFERENCE_H

#include "PreCompiled.h"
#include "ActorController.h"
#include "SharedPointer.h"

namespace Engine
{
	class Actor;

	// Implementation of ActorControllerWithReference
	class IActorControllerWithReference : public IActorController
	{
	public:
		IActorControllerWithReference(unsigned int i_UpdateFrequency) :
			m_UpdateFrequency(i_UpdateFrequency),
			m_TicksSinceUpdate(0),
			m_pOtherActor(NULL)
		{
		}

		~IActorControllerWithReference(){}
		
		void SetOtherActorReference(const SharedPointer<Actor> & i_pOtherActor) { m_pOtherActor = i_pOtherActor; }

		virtual void UpdateActor(Actor &i_Actor, const float i_DeltaTime) = 0;

	protected:
		unsigned int 			m_TicksSinceUpdate;
		unsigned int			m_UpdateFrequency;

		SharedPointer<Actor>	m_pOtherActor;
	};

}
#endif //__ACTOR_REFERENCE_H