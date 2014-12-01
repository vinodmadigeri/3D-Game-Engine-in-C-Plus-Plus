
#include "PreCompiled.h"

#include "Actor.h"
#include "ActorController.h"
#include "Debug.h"
#include "Vector3.h"
#include "NamedBitSet.h"

namespace Engine
{
	MemoryPool *Actor::m_pActorMemoryPool = NULL;
	NamedBitSet<int> Actor::mActorTypeNamedBitSet;

	Actor::Actor
	(
		Vector3 i_Position,  
		Vector3	i_Size,
		Vector3 i_Velocity, 
		Vector3 i_Acceleration,
		float	i_Rotation,
		char *i_GameObjectName,
		const Matrix4x4 &i_LocalToWorld,
		const unsigned int i_ClassBitIndex,
		const char *i_Type
	):
		mPosition(i_Position),
		mSize(i_Size),
		mVelocity(i_Velocity),
		mAcceleration(i_Acceleration),
		mRotation(i_Rotation),
		mProjectedPosition(Vector3(0.0f, 0.0f, 0.0f)),
		mProjectedVelocity(Vector3(0.0f, 0.0f, 0.0f)),
		pGameObjectName(i_GameObjectName),
		bMarkForDeath(false),
		mFriction(Vector3(0.0f, 0.0f, 0.0f)),
		mDeltaTime(static_cast<float>(CONSTANT_TIME_FRAME)),
		mLocalToWorld(i_LocalToWorld),
		m_pController(NULL),
		mType(i_Type),
		mClassBitIndex(i_ClassBitIndex),
		mHashedName(i_GameObjectName)
	{

	}

	SharedPointer<Actor> Actor::Create
	(
		Vector3 i_Position,  
		Vector3 i_Velocity, 
		Vector3 i_Acceleration, 
		const char *i_GameObjectName, 
		const char *i_ActorType,
		const Vector3 & i_Size,
		const float i_Rotation
	)
	{
		assert(i_ActorType != NULL);
		assert(i_GameObjectName != NULL);
		char *pGameObjName = _strdup(i_GameObjectName);

		int ClassBitIndex = 0;
		if (false == mActorTypeNamedBitSet.FindBitMask(i_ActorType, ClassBitIndex))
		{
			CONSOLE_PRINT("Actor type is not present in global class types");
			//assert(false); ToDo Fix the camera parse
		}

		Matrix4x4 Translation, Rotation, LocalToWorld;

		Translation.CreateTranslation(i_Position);
		Rotation.CreateZRotation(i_Rotation);

		LocalToWorld = Translation * Rotation;

		return SharedPointer<Actor>(new Actor(i_Position, i_Size, i_Velocity, i_Acceleration, i_Rotation, pGameObjName, LocalToWorld, ClassBitIndex, i_ActorType));
	}

	Actor::~Actor()
	{
		if (pGameObjectName != NULL)
		{
			free(pGameObjectName);
		}
	}

	void Actor::MarkForDeath(void)
	{
		bMarkForDeath = true;
	}

	bool Actor::IsMarkedForDeath(void)
	{
		return bMarkForDeath;
	}

	void Actor::SetPosition(const Vector3 & i_Position)
	{
		mPosition = i_Position;
	}

	void Actor::SetVelocity(const Vector3 & i_Velocity)
	{
		mVelocity = i_Velocity;
	}

	void Actor::SetAcceleration(const Vector3 & i_Acceleration)
	{
		mAcceleration = i_Acceleration;
	}

	void Actor::SetRotation(const float i_Rotation)
	{
		mRotation = i_Rotation;
	}

	void Actor::SetProjectedPosition(const Vector3 & i_ProjectedPosition)
	{
		mProjectedPosition = i_ProjectedPosition;
	}

	void Actor::SetProjectedVelocity(const Vector3 & i_ProjectedVelocity)
	{
		mProjectedVelocity = i_ProjectedVelocity;
	}

	void Actor::SetFriction(const Vector3 & i_Friction)
	{
		mFriction = i_Friction;
	}

	void Actor::SetDeltaTime(const float i_DeltaTime)
	{
		mDeltaTime = i_DeltaTime;
	}

	void Actor::SetName(const char *i_Name)
	{
		//copying the i_Actorname to ActorName
		pGameObjectName = _strdup(i_Name);
		assert(pGameObjectName);
	}

	void Actor::SetLocalToWorldMatrix(const Matrix4x4 & i_Input)
	{
		mLocalToWorld = i_Input;
	}

	void Actor::AddGlobalClassTypes(const char * i_ActorType)
	{
		(void)mActorTypeNamedBitSet.GetBitMask(i_ActorType);
	}

	const Vector3 & Actor::GetPosition(void) const
	{
		return mPosition;
	}

	const Vector3 & Actor::GetSize(void) const
	{
		return mSize;
	}

	const Vector3 & Actor::GetVelocity(void) const
	{
		return mVelocity;
	}

	const Vector3 & Actor::GetAcceleration(void) const
	{
		return mAcceleration;
	}

	const float & Actor::GetRotation(void) const
	{
		return mRotation;
	}

	const Vector3 & Actor::GetProjectedPosition(void) const
	{
		return mProjectedPosition;
	}
	const Vector3 & Actor::GetProjectedVelocity(void) const
	{
		return mProjectedVelocity;
	}

	const Vector3 & Actor::GetFriction(void) const
	{
		return mFriction;
	}

	const float Actor::GetDeltaTime(void) const
	{
		return mDeltaTime;
	}

	char * Actor::GetName(void) const
	{
		assert(pGameObjectName);

		return pGameObjectName;
	}

	const Matrix4x4 & Actor::GetLocalToWorldMatrix(void) const
	{
		return mLocalToWorld;
	}

	bool Actor::IsA(const char * i_ActorType) const
	{

		int o_BitIndex = 0;

		if ((true == mActorTypeNamedBitSet.FindBitMask(i_ActorType, o_BitIndex)) && (mClassBitIndex == o_BitIndex))
		{
			return true;
		}

		return false;
	}

	void Actor::SetController(IActorController * i_pController)
	{
		m_pController = i_pController;
	}
	
	bool Actor::IsControllerSet(void) const
	{
		if (m_pController)
		{
			return true;
		}

		return false;
	}

	void Actor::Update(const float i_DeltaTime)
	{
		if (m_pController)
		{
			m_pController->UpdateActor(*this, i_DeltaTime);
		}
		
		Matrix4x4 Translation, Rotation;
		Translation.CreateTranslation(mPosition);
		Rotation.CreateZRotation(0.0f);

		Matrix4x4 ObjToWorld = Rotation * Translation;

		//Set Local to world matrix of actor for 3D rendering to use
		SetLocalToWorldMatrix(ObjToWorld);

	}

	void Actor::CreateActorMemoryPool()
	{
		if (NULL == m_pActorMemoryPool)
		{
			m_pActorMemoryPool =  MemoryPool::Create(sizeof(Actor), MAX_ACTOR_ALLOWED);
		}
	}

	void Actor::DeleteActorMemoryPool()
	{
		if (NULL != m_pActorMemoryPool)
		{
			unsigned long ulOutputLength = 0;
			m_pActorMemoryPool->Destroy(&ulOutputLength );
		}
	}

	void * Actor::operator new(const size_t i_size)
	{
		Actor::CreateActorMemoryPool();

		assert(m_pActorMemoryPool != NULL);

		return m_pActorMemoryPool->Allocate(i_size);
	}

	void Actor::operator delete(void * i_ptr)
	{
		assert(i_ptr != NULL);

		m_pActorMemoryPool->DeAllocate(i_ptr);
	}
}