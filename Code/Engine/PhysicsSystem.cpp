
#include "PreCompiled.h"
#include "PhysicsSystem.h"
#include "Vector3.h"
#include "Debug.h"

namespace Engine
{

	const unsigned int  PhysicsSystem::MAX_PHYSICS_OBJECTS = 100;
	PhysicsSystem *PhysicsSystem ::mInstance = NULL;
	MemoryPool * PhysicsSystem::PhysicsObject::PhysicsMemoryPool = NULL;
		

	/******************************************************************************
		Function     : PhysicsObject
		Description  : Constructor of Physics object 
		Input        : SharedPointer<Actor> &i_Object
		Output       : 
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/		
	PhysicsSystem::PhysicsObject::PhysicsObject(SharedPointer<Actor> &i_Object) :
		m_WorldObject(i_Object)
	{

	}

	/******************************************************************************
		Function     : ~PhysicsObject
		Description  : Destructor of Physics object 
		Input        : 
		Output       : 
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	PhysicsSystem::PhysicsObject::~PhysicsObject()
	{

	}

	/******************************************************************************
		Function     : AddActorGameObject
		Description  : Function to add actor game object to physics system
		Input        : SharedPointer<Actor> &i_Object
		Output       : void
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void PhysicsSystem::AddActorGameObject(SharedPointer<Actor> &i_Object)
	{
		m_PhysicsObjectList.push_back(new PhysicsObject(i_Object));
	}

	/******************************************************************************
		Function     : DeleteMarkedToDeathGameObjects
		Description  : Function to delete marked to death actor game object from 
					physics system
		Input        : void
		Output       : void
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void PhysicsSystem::DeleteMarkedToDeathGameObjects(void)
	{
		for (unsigned long ulCount = 0; ulCount < m_PhysicsObjectList.size(); )
		{
			if( m_PhysicsObjectList.at(ulCount)->m_WorldObject->IsMarkedForDeath() )
			{
				delete m_PhysicsObjectList.at(ulCount);
				m_PhysicsObjectList.erase(m_PhysicsObjectList.begin() + ulCount);
			}
			else
			{
				ulCount++;
			}
		}
	}

	/******************************************************************************
		Function     : DeleteAllGameObjects
		Description  : Function to delete all actor game object from 
					physics system
		Input        : void
		Output       : void
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void PhysicsSystem::DeleteAllGameObjects(void)
	{
		for (unsigned long ulCount = 0; ulCount < m_PhysicsObjectList.size(); ulCount++)
		{
			delete m_PhysicsObjectList.at(ulCount);
		}

		m_PhysicsObjectList.clear();
	}

	/******************************************************************************
		Function     : ApplyEulerPhysics
		Description  : Function to apply Euler equation on physics objects
		Input        : float i_DeltaTime
		Output       : 
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void PhysicsSystem::ApplyEulerPhysics(float i_DeltaTime)
	{
			
		{
			Vector3  CurrentPosition;
			Vector3 CurrentVelocity;
			Vector3 CurrentAcceleration;
			Vector3 CurrentFriction;

			DeleteMarkedToDeathGameObjects();

			for (unsigned long ulCount = 0; ulCount< m_PhysicsObjectList.size(); ulCount++)
			{
				CurrentPosition = m_PhysicsObjectList.at(ulCount)->m_WorldObject->GetPosition();
				CurrentVelocity = m_PhysicsObjectList.at(ulCount)->m_WorldObject->GetVelocity();
				CurrentAcceleration = m_PhysicsObjectList.at(ulCount)->m_WorldObject->GetAcceleration();
				CurrentFriction = m_PhysicsObjectList.at(ulCount)->m_WorldObject->GetFriction();

				m_PhysicsObjectList.at(ulCount)->m_WorldObject->SetVelocity(CurrentVelocity +  Vector3(CurrentAcceleration.x() * i_DeltaTime, CurrentAcceleration.y() * i_DeltaTime, CurrentAcceleration.z() * i_DeltaTime ));
				CurrentVelocity = m_PhysicsObjectList.at(ulCount)->m_WorldObject->GetVelocity();

				//Update the Player position based on velocity
				m_PhysicsObjectList.at(ulCount)->m_WorldObject->SetPosition(CurrentPosition + Vector3(CurrentVelocity.x() * i_DeltaTime, CurrentVelocity.y() * i_DeltaTime, CurrentVelocity.z() * i_DeltaTime));

				Vector3 CPosition = m_PhysicsObjectList.at(ulCount)->m_WorldObject->GetPosition();
				//CONSOLE_PRINT("X=%f, Y=%f, Z=%f", CPosition.x(), CPosition.y(), CPosition.z());
				//Apply Friction if present
				m_PhysicsObjectList.at(ulCount)->m_WorldObject->SetVelocity(CurrentVelocity +  Vector3(CurrentFriction.x() * i_DeltaTime, CurrentFriction.y() * i_DeltaTime, CurrentFriction.z() * i_DeltaTime ));
			}
		}
		return;
	}

	/******************************************************************************
		Function     : ApplyEulerPhysics
		Description  : Function to apply Euler equation on input physics object
		Input        : SharedPointer<Actor> i_Object, float i_DeltaTime
		Output       : 
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void PhysicsSystem::ApplyEulerPhysics(SharedPointer<Actor> i_Object, float i_DeltaTime)
	{
		Vector3  CurrentPosition;
		Vector3 CurrentVelocity;
		Vector3 CurrentAcceleration;
		Vector3 CurrentFriction;

		DeleteMarkedToDeathGameObjects();

		
		CurrentPosition = i_Object->GetPosition();
		CurrentVelocity = i_Object->GetVelocity();

		CurrentAcceleration = i_Object->GetAcceleration();
		CurrentFriction = i_Object->GetFriction();

		i_Object->SetVelocity(CurrentVelocity +  Vector3(CurrentAcceleration.x() * i_DeltaTime, CurrentAcceleration.y() * i_DeltaTime, CurrentAcceleration.z() * i_DeltaTime ));
		CurrentVelocity = i_Object->GetVelocity();

		//Update the Player position based on velocity
		i_Object->SetPosition(CurrentPosition + Vector3(CurrentVelocity.x() * i_DeltaTime, CurrentVelocity.y() * i_DeltaTime, CurrentVelocity.z() * i_DeltaTime));

		//Apply Friction if present
		i_Object->SetVelocity(CurrentVelocity -  Vector3(CurrentFriction.x() * i_DeltaTime, CurrentFriction.y() * i_DeltaTime, CurrentFriction.z() * i_DeltaTime ));

		return;
	}

	PhysicsSystem::PhysicsSystem()
	{
		bool WereThereErrors = false;

		if (PhysicsObject::PhysicsMemoryPool == NULL)
		{
			PhysicsObject::PhysicsMemoryPool = MemoryPool::Create(sizeof(PhysicsObject), MAX_PHYSICS_OBJECTS);
			if (PhysicsObject::PhysicsMemoryPool == NULL)
			{
				assert(false);
				WereThereErrors = false;
			}
		}

		mInitilized = !WereThereErrors;
	}

	PhysicsSystem::~PhysicsSystem()
	{
		DeleteAllGameObjects();
		m_PhysicsObjectList.clear();

		if (PhysicsObject::PhysicsMemoryPool)
		{
			unsigned long ulOutLength;
			PhysicsObject::PhysicsMemoryPool->Destroy(&ulOutLength);
		}
	}

	bool PhysicsSystem::CreateInstance()
	{
		if (mInstance == NULL)
		{
			mInstance = new PhysicsSystem();

			if (mInstance == NULL)
			{
				return false;
			}

			if (mInstance->mInitilized == false)
			{
				delete mInstance;
				return false;
			}
		}

		return true;
	}

	PhysicsSystem * PhysicsSystem::GetInstance()
	{
		if (mInstance != NULL)
		{
			return mInstance;
		}

		assert(false);

		return NULL;
	}

	void PhysicsSystem::Destroy()
	{
		if (mInstance)
		{
			delete mInstance;
		}
	}
}