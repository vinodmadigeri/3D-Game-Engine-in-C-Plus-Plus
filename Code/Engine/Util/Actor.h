#ifndef __ACTOR_H
#define __ACTOR_H

#include "PreCompiled.h"

#include <vector>
#include "MemoryPool.h"

#include "Vector3.h"
#include "Matrix4x4.h"
#include "SharedPointer.h"
#include "HashedString.h"

const int MAX_ACTOR_ALLOWED = 101;
static const double CONSTANT_TIME_FRAME = 1000.0f / 60.0f;

namespace Engine
{
	class IActorController;
	
	template<class T> 
	class NamedBitSet;

	class ICollisionHandlerInterface;
	class CollisionObject;
}

namespace Engine
{
	class Actor
	{
		Vector3				mPosition;
		Vector3				mSize;
		Vector3				mVelocity;
		Vector3				mAcceleration;
		Vector3				mProjectedPosition;
		Vector3				mProjectedVelocity;
		Vector3				mFriction;
		float				mRotation;
		float				mDeltaTime;
		char				*pGameObjectName;
		bool				bMarkForDeath;
		Matrix4x4			mLocalToWorld;
		static MemoryPool	*m_pActorMemoryPool;
		static NamedBitSet<int>	mActorTypeNamedBitSet;
		ICollisionHandlerInterface *m_pCollisionHandler;

		IActorController	*m_pController;
		HashedString		mType;

		Actor(Vector3 i_Position,
			Vector3	i_Size,
			Vector3 i_Velocity, 
			Vector3 i_Acceleration,
			float	i_Rotation,
			char *i_GameObjectName,
			const Matrix4x4 &i_LocalToWorld,
			const unsigned int i_ClassBitIndex,
			const unsigned int i_CollidesWithBitIndex,
			const char * i_Type);

		static void CreateActorMemoryPool();

	public:
		unsigned int		mClassBitIndex;
		unsigned int		mCollidesWithBitIndex;
		HashedString		mHashedName;
		~Actor();
		static SharedPointer<Actor> Create
		(
			Vector3 i_Position,
			Vector3 i_Velocity,
			Vector3 i_Acceleration,
			const char *i_GameObjectName, 
			const char *i_ActorType,
			const Vector3 & i_Size,
			const float i_Rotation,
			const std::vector<std::string> &iCollidesWith
		);

		static void DeleteActorMemoryPool();
		void MarkForDeath(void);
		bool IsMarkedForDeath(void);

		void SetPosition(const Vector3 & i_Position);
		void SetVelocity(const Vector3 & i_Velocity);
		void SetAcceleration(const Vector3 & i_Acceleration);
		void SetRotation(const float i_Rotation);
		void SetProjectedPosition(const Vector3 & i_Velocity);
		void SetProjectedVelocity(const Vector3 & i_Velocity);
		void SetFriction(const Vector3 & i_Friction);
		void SetDeltaTime(const float i_DeltaTime);
		void SetName(const char *i_Name);
		void SetLocalToWorldMatrix(const Matrix4x4 & i_Input);

		const Vector3 & GetPosition(void) const;
		const Vector3 & GetSize(void) const;
		const Vector3 & GetVelocity(void) const;
		const Vector3 & GetAcceleration(void) const;
		const float & GetRotation(void) const;
		const Vector3 & GetProjectedPosition(void) const;
		const Vector3 & GetProjectedVelocity(void) const;
		const Vector3 & GetFriction(void) const;
		const float GetDeltaTime(void) const;
		char *GetName(void) const;
		const Matrix4x4 & GetLocalToWorldMatrix(void) const;
		static void AddGlobalClassTypes(const char * i_ActorType);
		bool IsA(const char * i_ActorType) const;

		void SetController(IActorController * i_pController);
		bool IsControllerSet(void) const;
		void Update(const float i_DeltaTime);
		void SetCollisionHandler(ICollisionHandlerInterface *i_pCollisionHandler);
		bool IsCollisionHandlerSet(void) const;
		void HandleCollision(CollisionObject *ThisCollisionObject, CollisionObject *OtherCollisionObject);
		void * operator new(const size_t i_size);
		void operator delete(void * i_ptr);
	} ;
}
#endif //__ACTOR_H
