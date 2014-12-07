#ifndef __COLLISION_SYSTEM_HEADER
#define __COLLISION_SYSTEM_HEADER

#include "Precompiled.h"
#include <vector>
#include "AABB.h"
#include "SharedPointer.h"
#include "MemoryPool.h"
#include "Matrix4x4.h"

#include "Vector3.h"

namespace Engine
{
	//Forward Decleration
	class Actor;

	class CollisionObject
	{
	public:
		SharedPointer<Actor> m_WorldObject;
		AABB				 m_WorldBox;
		CollisionObject		 *m_CollidedObject;
		float				 m_CollisionTime;
		Vector3				 m_CollisionResponseVector;

		static MemoryPool *CollisionMemoryPool;
		CollisionObject(SharedPointer<Actor> &i_WorldObject, AABB i_WorldBox);
		~CollisionObject();

		inline void * operator new(size_t i_size)
		{
			assert(i_size == sizeof(CollisionObject));

			return CollisionObject::CollisionMemoryPool->Allocate(i_size);
		}

		inline void operator delete(void *i_ptr)
		{
			if (i_ptr != NULL)
			{
				CollisionObject::CollisionMemoryPool->DeAllocate(i_ptr);
			}
		}
	} ;

	class CollisionSystem
	{
		static unsigned int MAX_COLLIDABLE_OBJECTS;
		std::vector<CollisionObject *> mCollisionObjects;
		static CollisionSystem * mInstance;
		bool mInitilized;

		CollisionSystem();

		~CollisionSystem();

		CollisionSystem(const CollisionSystem & i_Other);
		CollisionSystem & operator=(const CollisionSystem & i_rhs);
		void DeleteMarkedToDeathGameObjects(void);
		void DeleteAllGameObjects(void);
		bool CheckCollision(float i_DeltaTime, float &o_FirstCollisionTime);
		bool AxisRangeRayOverlap(float i_RangeStart, float i_RangeEnd, float i_RayStart, float i_RayLength, float & o_dEnter, float & o_dExit, Vector3 &SurfaceA, Vector3 &SurfaceB, float DeltaTime);
		bool CheckOOBBIntersection(const AABB & i_BoxA, const Vector3 & i_VelocityA, const Matrix4x4 & i_ObjAtoWorld,
			const AABB & i_BoxB, const Vector3 & i_VelocityB, const Matrix4x4 & i_ObjBtoWorld,
			Vector3 &SurfaceNormalA, Vector3 &SurfaceNormalB, float DeltaTime, float &CollisionTime);
		bool AxisCheck(float RelativeCentre, float Extent, float RelativeVelocity, float Centre, float i_DeltaTime, float &EnterTime, float &ExitTime, Vector3 & i_SurfaceNormal, Vector3 & o_SurfaceNormal);

	public:
		void AddActorGameObject(SharedPointer<Actor> &i_Object);

		void Update(float i_DeltaTime);

		static bool CreateInstance();
		static CollisionSystem * GetInstance();
		static void Destroy();
	};	
}

#endif //__COLLISION_SYSTEM_HEADER