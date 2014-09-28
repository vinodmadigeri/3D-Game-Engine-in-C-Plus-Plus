
#include "PreCompiled.h"

#include "Material.h"
#include "GraphicsObjects.h"
#include "Vector3.h"
#include "HashedString.h"

namespace Engine
{
	MemoryPool *RenderableObjectSystem::Renderable3DObject::Renderer3DMemoryPool = NULL;

		/***************************3D Rendering **************************************/
		/******************************************************************************
		 Function     : Renderable3DObject
		 Description  : Constructor of 3D Rendering object 
		 Input        : SharedPointer<Actor> &i_WorldObject, 
						SharedPointer<Cheesy::Material> &i_Material,
						SharedPointer<Cheesy::Mesh> &i_Mesh
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/	
		RenderableObjectSystem::Renderable3DObject::Renderable3DObject
		(
			SharedPointer<Actor> &i_WorldObject, 
			SharedPointer<Material> &i_Material,
			SharedPointer<MeshData> &i_Mesh
		):
		m_WorldObject(i_WorldObject),
		mMaterial(i_Material),
		mMesh(i_Mesh)
		{
			
		}

		/******************************************************************************
		 Function     : ~Renderable3DObject
		 Description  : Destructor of 3D Rendering object 
		 Input        : 
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		RenderableObjectSystem::Renderable3DObject::~Renderable3DObject()
		{

		}

		/******************************************************************************
		 Function     : GetMaterial
		 Description  : Function to get material of 3D object for rendering
		 Input        : void
		 Output       : shared material pointer for rendering
		 Return Value : SharedPointer<Material> 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		SharedPointer<Material> RenderableObjectSystem::Renderable3DObject::GetMaterial(void) const
		{
			return mMaterial;
		}

		/******************************************************************************
		 Function     : GetMesh
		 Description  : Function to get mesh of 3D object for rendering
		 Input        : void
		 Output       : shared mesh pointer for rendering
		 Return Value : SharedPointer<MeshData> 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		SharedPointer<MeshData> RenderableObjectSystem::Renderable3DObject::GetMesh(void) const
		{
			return mMesh;
		}

		/******************************************************************************
		 Function     : Add3DActorGameObject
		 Description  : Function to add 3D Actor object for Rendering, Only creates 
						Material and Mesh if not in cache
		 Input        : void
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		void RenderableObjectSystem::Add3DActorGameObject(
			SharedPointer<Actor> &i_Object,
			const char *pcShaderLocation, 
			const char *pcTextureArray,
			const unsigned int Count)
		{
			assert(pcTextureArray);
			assert(pcShaderLocation);

			SharedPointer<Material> NewMatrial;
			{
				//Else create new material and insert into material cache
				//NewMatrial = Cheesy::Material::Create( pcShaderLocation, &pcTextureArray, Count );
			}

			assert(NewMatrial != NULL);

			SharedPointer<MeshData> NewMesh;
			{
				//Else create new mesh and insert into mesh cache
				//NewMesh = Cheesy::Mesh::Create( i_MeshData->data );
			}

			assert( NewMesh != NULL);

			m3DRenderableObjects.push_back(new Renderable3DObject(i_Object, NewMatrial, NewMesh));
		}


		/***************************Global Rendering***********************************/
		/******************************************************************************
		 Function     : DeleteMarkedToDeathGameObjects
		 Description  : Function to delete marked to death actor game object from 
						rendering system
		 Input        : void
		 Output       : void
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		void RenderableObjectSystem::DeleteMarkedToDeathGameObjects(void)
		{
			for (unsigned long ulCount = 0; ulCount < m3DRenderableObjects.size(); )
			{
				if( m3DRenderableObjects.at(ulCount)->m_WorldObject->IsMarkedForDeath() )
				{
					delete (m3DRenderableObjects.at(ulCount));
					m3DRenderableObjects.erase(m3DRenderableObjects.begin() + ulCount);
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
						rendering system
		 Input        : void
		 Output       : void
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		void RenderableObjectSystem::DeleteAllGameObjects(void)
		{
			//Delete 3D objects
			for (unsigned long ulCount = 0; ulCount < m3DRenderableObjects.size(); ulCount++)
			{
				delete m3DRenderableObjects.at(ulCount);
			}

			m3DRenderableObjects.clear();

		}

		/******************************************************************************
		 Function     : Render
		 Description  : Function to render #D, 2D, Text objects
		 Input        : bool & bQuit
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		void RenderableObjectSystem::Render(bool & bQuit)
		{
			Vector3  CurrentPosition;
			Vector3 CurrentVelocity;
			Vector3 CurrentAcceleration;
			Vector3 CurrentFriction;
		
			DeleteMarkedToDeathGameObjects();
		

			return;
		}

		/******************************************************************************
		 Function     : Initilize
		 Description  : Function to initilize rendering system, creates memory pool for 3D, 2D, Text
		 Input        : const char * WindowsName, const unsigned int WindowX, const unsigned int WindowY,
						const D3DXVECTOR3 &Eye, const D3DXVECTOR3 &LookAt, const D3DXVECTOR3 &Up
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		void RenderableObjectSystem::Initilize(void)
		{
			if (Renderable3DObject::Renderer3DMemoryPool == NULL)
			{
				Renderable3DObject::Renderer3DMemoryPool = MemoryPool::Create(sizeof(Renderable3DObject), 100);
				assert(Renderable3DObject::Renderer3DMemoryPool != NULL);
			}
		}

		/******************************************************************************
		 Function     : Shutdown
		 Description  : Function to Shutdown and destroy memory pool of 3D, 2D, Text objects,
						Also clears cache
		 Input        : 
		 Output       : 
		 Return Value : 

		 History      :
		 Author       : Vinod VM
		 Modification : Created function
		******************************************************************************/
		void RenderableObjectSystem::Shutdown(void)
		{
			unsigned long o_ulOutPutLen;
			DeleteAllGameObjects();

			m3DRenderableObjects.clear();

			if (Renderable3DObject::Renderer3DMemoryPool != NULL)
			{
				Renderable3DObject::Renderer3DMemoryPool->Destroy(&o_ulOutPutLen);
			}
		}
}