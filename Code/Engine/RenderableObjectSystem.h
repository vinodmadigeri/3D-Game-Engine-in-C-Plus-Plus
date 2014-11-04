#ifndef __RENDERABLE_OBJECT_SYSTEM_HEADER
#define __RENDERABLE_OBJECT_SYSTEM_HEADER

#include "PreCompiled.h"

#include <vector>
#include <map>
#include "Mesh.h"
#include "Material.h"
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
			SharedPointer<Mesh> mMesh;

		public:

			static MemoryPool *Renderer3DMemoryPool;
			SharedPointer<Actor> m_WorldObject;

			Renderable3DObject(SharedPointer<Actor> &i_WorldObject,
				SharedPointer<Material>	&i_Material,
				SharedPointer<Mesh> &i_Mesh);

			~Renderable3DObject();

			SharedPointer<Material> GetMaterial(void) const;
			SharedPointer<Mesh> GetMesh(void) const;

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

		RenderableObjectSystem();
		~RenderableObjectSystem();
		RenderableObjectSystem(const RenderableObjectSystem & i_Other);
		RenderableObjectSystem & operator=(const RenderableObjectSystem & i_rhs);

		void DeleteMarkedToDeathGameObjects(void);
		void DeleteAllGameObjects(void);

		std::vector<Renderable3DObject *> m3DRenderableObjects;
		static unsigned int MAX_3D_OBJECTS;
		static RenderableObjectSystem * mInstance;
		bool mInitilized;
	public:

		void Add3DActorGameObject(
			SharedPointer<Actor> &i_Object,
			const char *pcMaterialPath,
			const char *pcMeshPath);

		void Render();

		static bool CreateInstance(const HWND i_mainWindow,
			const unsigned int i_windowWidth,
			const unsigned int i_windowHeight,
			const bool i_shouldRenderFullScreen);

		static RenderableObjectSystem * GetInstance();
		static void Destroy();
	} ;
}
#endif //__RENDERABLE_OBJECT_SYSTEM_HEADER