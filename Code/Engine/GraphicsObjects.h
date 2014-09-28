#ifndef __GRAPHICS_OBJECTS_HEADER
#define __GRAPHICS_OBJECTS_HEADER

#include "PreCompiled.h"

#include <vector>
#include <map>
#include "MeshData.h"
#include "Actor.h"
#include "SharedPointer.h"

namespace Engine
{
	class RenderableObjectSystem
	{
	private:
		class Renderable3DObject
		{
			float				mRotation;
			SharedPointer<Material>	mMaterial;
			SharedPointer<MeshData> mMesh;

		public:

			static MemoryPool *Renderer3DMemoryPool;
			SharedPointer<Actor> m_WorldObject;

			Renderable3DObject(SharedPointer<Actor> &i_WorldObject,
				SharedPointer<Material>	&i_Material,
				SharedPointer<MeshData> &i_Mesh);

			~Renderable3DObject();

			SharedPointer<Material> GetMaterial(void) const;
			SharedPointer<MeshData> GetMesh(void) const;

			inline void * operator new(size_t i_size)
			{
				assert(i_size == sizeof(Renderable3DObject));

				return Renderable3DObject::Renderer3DMemoryPool->Allocate(i_size);
			}

				inline void operator delete(void *i_ptr)
			{
				if (i_ptr != NULL)
				{
					Renderable3DObject::Renderer3DMemoryPool->DeAllocate(i_ptr);
				}
			}
		};

		void Initilize(void);
		void Shutdown(void);

		void DeleteMarkedToDeathGameObjects(void);
		void DeleteAllGameObjects(void);

		std::vector<Renderable3DObject *> m3DRenderableObjects;
	public:

		void Add3DActorGameObject(
			SharedPointer<Actor> &i_Object,
			const char * pcShaderLocation,
			const char *pcTextureArray,
			const unsigned int Count);
		void Render(bool & bQuit);
	} ;
}
#endif //__RENDERING_SYSTEM_HEADER