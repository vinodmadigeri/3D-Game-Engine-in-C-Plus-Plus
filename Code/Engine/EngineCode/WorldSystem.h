#ifndef __WORLD_SYSTEM_HEADER
#define __WORLD_SYSTEM_HEADER

#include "PreCompiled.h"

#include <vector>

#include "Actor.h"
#include "MemoryPool.h"
#include "SharedPointer.h"
#include "MeshData.h"

namespace Engine
{
	class WorldSystem
	{
	private:
		class WorldObject
		{

		public:
			SharedPointer<Actor> m_WorldObject;
			static MemoryPool* WorldMemoryPool;

			WorldObject(SharedPointer<Actor> &i_ActorObject);
			~WorldObject();

			inline void * operator new (size_t i_size)
			{
				assert(i_size == sizeof(WorldObject));

				return WorldMemoryPool->Allocate(i_size);
			}

				inline void operator delete(void * i_ptr)
			{
				if (i_ptr != NULL)
				{
					WorldMemoryPool->DeAllocate(i_ptr);
				}
			}
		};

		WorldSystem();
		~WorldSystem();
		WorldSystem(const WorldSystem & i_Other);
		WorldSystem & operator=(const WorldSystem & i_rhs);

		void DeleteMarkedToDeathGameObjects(void);
		void DeleteAllGameObjects(void);
		
		static unsigned int MAX_WORLD_OBJECTS;
		std::vector<WorldObject *> m_WorldObjectList;
		static WorldSystem * mInstance;
		bool mInitilized;
	public:

		void CreateActors(Vector3 i_vInitialPosition,
			Vector3 i_InitialVelocity,
			Vector3 i_vInitialAcceleration,
			const char *i_GameObjectName,
			const char *i_ActorType,
			const Vector3 & i_Size,
			const float i_Rotation,
			const char *pcMaterialPath,
			const char *pcMeshPath,
			const std::vector<std::string> &iCollidesWith);

		void AddActorGameObject(SharedPointer<Actor> &i_Object);
		std::vector< SharedPointer<Actor>> FindActorsByType(const char *i_ActorType);
		unsigned int FindActorCountByType(const char *i_ActorType);
		std::vector< SharedPointer<Actor>> FindAllActors(void);
		void ActorsUpdate(const float i_DeltaTime);

		static bool CreateInstance();
		static WorldSystem * GetInstance();
		static void Destroy();
	} ;
}
#endif //__WORLD_SYSTEM_HEADER