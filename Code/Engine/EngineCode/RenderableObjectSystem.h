#ifndef __RENDERABLE_OBJECT_SYSTEM_HEADER
#define __RENDERABLE_OBJECT_SYSTEM_HEADER

#include "PreCompiled.h"

#include <vector>
#include <map>
#include "Mesh.h"
#include "Material.h"
#include "MeshData.h"
#include "Sprite.h"
#include "Actor.h"
#include "SharedPointer.h"

namespace Engine
{
	struct sLine;

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

	public:
		class RenderableSprites
		{
			//float				mRotation;
			SharedPointer<Sprite>	mSprite;

		public:

			static MemoryPool *SpriteMemoryPool;

			RenderableSprites(SharedPointer<Sprite> &i_Sprite);

			~RenderableSprites();

			SharedPointer<Sprite> GetSprite(void) const;

			inline void * operator new(size_t i_size)
			{
				assert(i_size == sizeof(RenderableSprites));

				return RenderableSprites::SpriteMemoryPool->Allocate(i_size);
			}

				inline void operator delete(void *i_ptr)
			{
				if (i_ptr != NULL)
				{
					RenderableSprites::SpriteMemoryPool->DeAllocate(i_ptr);
				}
			}
		};

	private:
		RenderableObjectSystem();
		~RenderableObjectSystem();
		RenderableObjectSystem(const RenderableObjectSystem & i_Other);
		RenderableObjectSystem & operator=(const RenderableObjectSystem & i_rhs);

		std::vector<Renderable3DObject *> m3DRenderableObjects;
		std::vector<RenderableSprites *> mSpriteRenderableObjects;

		static unsigned int MAX_3D_OBJECTS;
		static unsigned int MAX_2D_OBJECTS;

		static RenderableObjectSystem * mInstance;
		bool mInitilized;
	public:

		void Add3DActorGameObject(
			SharedPointer<Actor> &i_Object,
			const char *pcMaterialPath,
			const char *pcMeshPath);

		void CreateSprite(const char* i_TexturePath, const sRectangle *i_positionRect,
			const sRectangle *i_texcoordsRect, unsigned int i_MaxHorizontalCount = 1, unsigned int i_MaxVerticalCount = 1);

		bool CreateDebugLines(const char * iName, const unsigned int iMaxlines);
		void AddDebugLines(const sLine &iLine);
		
		void Render();

		bool Delete3DGameObjectByName(const char * iName);
		bool DeleteSpriteGameObjectByName(const char * iName);
		RenderableSprites * FindSpriteGameObjectByName(const char * iName);
		void DeleteMarkedToDeathGameObjects(void);
		void DeleteAllRenderableObjects(void);

		static bool CreateInstance(const HWND i_mainWindow);

		static RenderableObjectSystem * GetInstance();
		static void Destroy();
	} ;
}
#endif //__RENDERABLE_OBJECT_SYSTEM_HEADER