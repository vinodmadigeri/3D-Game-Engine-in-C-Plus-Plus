#ifndef __PHYSICS_SYSTEM_HEADER
#define __PHYSICS_SYSTEM_HEADER

#include "PreCompiled.h"

#include <vector>
#include "Actor.h"
#include "MemoryPool.h"
#include "SharedPointer.h"

namespace Engine
{
	class PhysicsSystem
	{
	private:
		class PhysicsObject
		{
		public:
			SharedPointer<Actor> m_WorldObject;

			static MemoryPool *PhysicsMemoryPool;

			PhysicsObject(SharedPointer<Actor> &i_Object);

			~PhysicsObject();

			inline void * operator new(size_t i_size)
			{
				assert(i_size == sizeof(PhysicsObject));

				return PhysicsMemoryPool->Allocate(i_size);
			}

			inline void operator delete(void *i_ptr)
			{
				if (i_ptr != NULL)
				{
					PhysicsMemoryPool->DeAllocate(i_ptr);
				}
			}
		} ;

		std::vector<PhysicsObject *> m_PhysicsObjectList;
		static PhysicsSystem *mInstance;
		bool mInitilized;

		PhysicsSystem();
		~PhysicsSystem();
		PhysicsSystem & operator=(const PhysicsSystem & i_rhs);
		PhysicsSystem(const PhysicsSystem & i_Other);
		static const unsigned int  MAX_PHYSICS_OBJECTS;
	public:

		void AddActorGameObject(SharedPointer<Actor> &i_Object);
		void DeleteMarkedToDeathGameObjects(void);
		void DeleteAllGameObjects(void);
		void ApplyEulerPhysics(float i_DeltaTime);
		void ApplyEulerPhysics(SharedPointer<Actor> i_Object, float i_DeltaTime);
		static bool CreateInstance();
		static PhysicsSystem * GetInstance();
		static void Destroy();
	} ;
}
#endif //__PHYSICS_SYSTEM_HEADER