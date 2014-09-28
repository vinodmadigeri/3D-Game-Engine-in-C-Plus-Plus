#include "PreCompiled.h"

#include <vector>

#include "ActorController.h"
#include "Debug.h"
//#include "PhysicsSystem.h"
#include "WorldSystem.h"

namespace Engine
{

		static unsigned int MAX_WORLD_OBJECTS = 100;
		static WorldSystem * mInstance = NULL;
		MemoryPool * WorldSystem::WorldObject::WorldMemoryPool = NULL;
		
		/******************************************************************************
		 Function     : WorldObject
		 Description  : Constructor for world object
		 Input        : SharedPointer<Actor> &i_ActorObject
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		WorldSystem::WorldObject::WorldObject(SharedPointer<Actor> &i_ActorObject) :
			m_WorldObject(i_ActorObject)
		{

		}

		/******************************************************************************
		 Function     : ~WorldObject
		 Description  : Destructor for world object
		 Input        : 
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		WorldSystem::WorldObject::~WorldObject()
		{

		}

		/******************************************************************************
		 Function     : CreateActors
		 Description  : Function to create all initial actors from data file
		 Input        : 
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/	
		void WorldSystem::CreateActors(Vector3 i_vInitialPosition,
			Vector3 i_InitialVelocity, 
			Vector3 i_vInitialAcceleration,
			const char *i_GameObjectName,
			const char *i_ActorType,
			const Vector3 & i_Size,
			const float i_Rotation,
			const char *i_Type)
		{

			SharedPointer<Actor> NewActor = Actor::Create(i_vInitialPosition, i_InitialVelocity, i_vInitialAcceleration, i_GameObjectName, i_ActorType, i_Size, i_Rotation, i_Type);

			//Add actor in world system
			AddActorGameObject(NewActor);

			//Create Graphics Object and add

			//Create Physics object and add
			return;
		}


		/******************************************************************************
		 Function     : AddActorGameObject
		 Description  : Function to add actor to world system
		 Input        : SharedPointer<Actor> &i_Object
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		
		void WorldSystem::AddActorGameObject(SharedPointer<Actor> &i_Object)
		{
			m_WorldObjectList.push_back(new WorldObject(i_Object));
		}

		/******************************************************************************
		 Function     : DeleteMarkedToDeathGameObjects
		 Description  : Function to delete all marked to death actors
		 Input        : 
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		void WorldSystem::DeleteMarkedToDeathGameObjects(void)
		{
			for (unsigned long ulCount = 0; ulCount < m_WorldObjectList.size(); )
			{
				if( m_WorldObjectList.at(ulCount)->m_WorldObject->IsMarkedForDeath() )
				{
					delete m_WorldObjectList.at(ulCount);
					m_WorldObjectList.erase(m_WorldObjectList.begin() + ulCount);
				}
				else
				{
					ulCount++;
				}
			}
		}

		/******************************************************************************
		 Function     : DeleteAllGameObjects
		 Description  : Function to delete all actors in world system
		 Input        : 
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/	
		void WorldSystem::DeleteAllGameObjects(void)
		{
			for (unsigned long ulCount = 0; ulCount < m_WorldObjectList.size(); ulCount++)
			{
				delete m_WorldObjectList.at(ulCount);
			}

			m_WorldObjectList.clear();
		}

		/******************************************************************************
		 Function     : FindActorsByType
		 Description  : Function to find and return actors of input class
		 Input        : const char *i_ActorType
		 Output       : returns vector of objects of input actor type
		 Return Value : std::vector< SharedPointer<Actor>>

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/	
		std::vector< SharedPointer<Actor>> WorldSystem::FindActorsByType(const char *i_ActorType)
		{
			assert(i_ActorType);

			std::vector< SharedPointer<Actor>> ActorsList;

			for (unsigned long ulCount = 0; ulCount < m_WorldObjectList.size(); ulCount++)
			{
				if( true == m_WorldObjectList.at(ulCount)->m_WorldObject->IsA(i_ActorType) )
				{
					ActorsList.push_back(m_WorldObjectList.at(ulCount)->m_WorldObject);
				}
			}

			return ActorsList;
		}

		/******************************************************************************
		 Function     : FindActorCountByType
		 Description  : Function to count all actors of input type
		 Input        : const char *i_ActorType
		 Output       : returns count of objects of input actor type
		 Return Value : unsigned int

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/	
		unsigned int WorldSystem::FindActorCountByType(const char *i_ActorType)
		{
			assert(i_ActorType);

			unsigned int Count = 0;

			for (unsigned long ulCount = 0; ulCount < m_WorldObjectList.size(); ulCount++)
			{
				if( true == m_WorldObjectList.at(ulCount)->m_WorldObject->IsA(i_ActorType) )
				{
					Count++;
				}
			}

			return Count;
		}

		/******************************************************************************
		 Function     : FindAllActors
		 Description  : Function to find all actors
		 Input        : void
		 Output       : returns all objects in world system
		 Return Value : vector< SharedPointer<Actor>>

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/	
		std::vector< SharedPointer<Actor>> WorldSystem::FindAllActors(void)
		{
			std::vector< SharedPointer<Actor>> ActorsList;

			for (unsigned long ulCount = 0; ulCount < m_WorldObjectList.size(); ulCount++)
			{
				ActorsList.push_back(m_WorldObjectList.at(ulCount)->m_WorldObject);
			}

			return ActorsList;
		}

		/******************************************************************************
		 Function     : ActorsUpdate
		 Description  : Function to update all actors
		 Input        : const float i_DeltaTime
		 Output       : void
		 Return Value : void

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/	
		void WorldSystem::ActorsUpdate(const float i_DeltaTime)
		{
			DeleteMarkedToDeathGameObjects();

			for (unsigned long ulCount = 0; ulCount < m_WorldObjectList.size(); ulCount++)
			{
					m_WorldObjectList.at(ulCount)->m_WorldObject->Update(i_DeltaTime);
			}
		}

		WorldSystem::WorldSystem()
		{
			bool WereThereErrors = false;

			if (WorldObject::WorldMemoryPool == NULL)
			{
				WorldObject::WorldMemoryPool = MemoryPool::Create(sizeof(WorldObject), MAX_WORLD_OBJECTS);
				if (WorldObject::WorldMemoryPool)
				{
					assert(false);
					WereThereErrors = false;
				}
			}

			mInitilized = !WereThereErrors;
		}

		WorldSystem::~WorldSystem()
		{
			DeleteAllGameObjects();
			m_WorldObjectList.clear();

			if (WorldObject::WorldMemoryPool)
			{
				unsigned long ulOutLength;
				WorldObject::WorldMemoryPool->Destroy(&ulOutLength);
			}
		}

		bool WorldSystem::CreateInstance()
		{
			if (mInstance == NULL)
			{
				mInstance = new WorldSystem();

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

		WorldSystem * WorldSystem::GetInstance()
		{
			if (mInstance != NULL)
			{
				return mInstance;
			}

			assert(false);

			return NULL;
		}

		void WorldSystem::Destroy()
		{
			if (mInstance)
			{
				delete mInstance;
			}
		}
}