
#include "PreCompiled.h"

#include "Material.h"
#include "MeshData.h"
#include "RenderableObjectSystem.h"
#include "CameraSystem.h"
#include "GraphicsSystem.h"
#include "Vector3.h"
#include "HashedString.h"

namespace Engine
{
	MemoryPool *RenderableObjectSystem::Renderable3DObject::Renderer3DMemoryPool = NULL;
	unsigned int RenderableObjectSystem::MAX_3D_OBJECTS = 100;
	RenderableObjectSystem* RenderableObjectSystem::mInstance = NULL;

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
		SharedPointer<Mesh> &i_Mesh
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
		Return Value : SharedPointer<Mesh> 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	SharedPointer<Mesh> RenderableObjectSystem::Renderable3DObject::GetMesh(void) const
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
		const char *pcMaterialPath,
		const DrawInfo &i_DrawInfo)
	{
		assert(pcMaterialPath);

		SharedPointer<Material> NewMaterial;
		{
			//create new material and insert into material cache eventually
			NewMaterial = GraphicsSystem::GetInstance()->CreateMaterial(pcMaterialPath);

			//Set color constant from fragment shader
			//NewMaterial->SetFragmentShaderConstantValue(Vector3(3.0f, 6.0f, 2.0f));
		}

		assert(NewMaterial != NULL);

		SharedPointer<Mesh> NewMesh;
		{
			//create new mesh and insert into mesh cache eventually
			NewMesh = GraphicsSystem::GetInstance()->CreateMesh(i_DrawInfo);
		}

		assert( NewMesh != NULL);

		m3DRenderableObjects.push_back(new Renderable3DObject(i_Object, NewMaterial, NewMesh));
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
		Input        : 
		Output       : 
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void RenderableObjectSystem::Render()
	{
		Vector3  CurrentPosition;
		Vector3 CurrentVelocity;
		Vector3 CurrentAcceleration;
		Vector3 CurrentFriction;
		
		DeleteMarkedToDeathGameObjects();
		GraphicsSystem::GetInstance()->BeingFrame();
		
		//Render Logic
		for (unsigned long ulCount = 0; ulCount < m3DRenderableObjects.size(); ulCount++)
		{

			//Create a D3DMATRIX
			D3DXMATRIX LocalToWorld;

			//Transpose the stored matrix
			Matrix4x4 ObjectMatrix = m3DRenderableObjects[ulCount]->m_WorldObject->GetLocalToWorldMatrix().GetTranspose();
			LocalToWorld._11 = ObjectMatrix.At(1, 1);
			LocalToWorld._12 = ObjectMatrix.At(1, 2);
			LocalToWorld._13 = ObjectMatrix.At(1, 3);
			LocalToWorld._14 = ObjectMatrix.At(1, 4);
			LocalToWorld._21 = ObjectMatrix.At(2, 1);
			LocalToWorld._22 = ObjectMatrix.At(2, 2);
			LocalToWorld._23 = ObjectMatrix.At(2, 3);
			LocalToWorld._24 = ObjectMatrix.At(2, 4);
			LocalToWorld._31 = ObjectMatrix.At(3, 1);
			LocalToWorld._32 = ObjectMatrix.At(3, 2);
			LocalToWorld._33 = ObjectMatrix.At(3, 3);
			LocalToWorld._34 = ObjectMatrix.At(3, 4);
			LocalToWorld._41 = ObjectMatrix.At(4, 1);
			LocalToWorld._42 = ObjectMatrix.At(4, 2);
			LocalToWorld._43 = ObjectMatrix.At(4, 3);
			LocalToWorld._44 = ObjectMatrix.At(4, 4);

			unsigned int count = 1;

			if (!m3DRenderableObjects.at(ulCount)->GetMaterial()->SetPerInstanceConstantDataByName("g_transform_modelToWorld", &LocalToWorld, count))
			{
				assert(false);
			}

			assert(CameraSystem::GetInstance());
			//Set per-view constants
			if (!m3DRenderableObjects.at(ulCount)->GetMaterial()->SetPerViewConstantDataByName("g_transform_worldToView", &CameraSystem::GetInstance()->GetWorldToView(), count))
			{
				assert(false);
			}

			//Set per-view constants
			if (!m3DRenderableObjects.at(ulCount)->GetMaterial()->SetPerViewConstantDataByName("g_transform_viewToScreen", &CameraSystem::GetInstance()->GetViewToScreen(), count))
			{
				assert(false);
			}


			GraphicsSystem::GetInstance()->Render(m3DRenderableObjects.at(ulCount)->GetMaterial(), m3DRenderableObjects.at(ulCount)->GetMesh());
		}

		GraphicsSystem::GetInstance()->EndFrame();
		return;
	}

	RenderableObjectSystem::RenderableObjectSystem()
	{
		bool WereThereErrors = false;

		if (Renderable3DObject::Renderer3DMemoryPool == NULL)
		{
			Renderable3DObject::Renderer3DMemoryPool = MemoryPool::Create(sizeof(Renderable3DObject), MAX_3D_OBJECTS);
			if (Renderable3DObject::Renderer3DMemoryPool == NULL)
			{
				assert(false);
				WereThereErrors = false;
			}
		}

		mInitilized = !WereThereErrors;
	}

	RenderableObjectSystem::~RenderableObjectSystem()
	{
		DeleteAllGameObjects();
		m3DRenderableObjects.clear();

		if (Renderable3DObject::Renderer3DMemoryPool)
		{
			unsigned long ulOutLength;
			Renderable3DObject::Renderer3DMemoryPool->Destroy(&ulOutLength);
		}
	}

	bool RenderableObjectSystem::CreateInstance(const HWND i_mainWindow,
							const unsigned int i_windowWidth,
							const unsigned int i_windowHeight,
							const bool i_shouldRenderFullScreen)
	{
		if (mInstance == NULL)
		{
			if (false == GraphicsSystem::CreateInstance(i_mainWindow, i_windowWidth, i_windowHeight, i_shouldRenderFullScreen))
			{
				assert(false);
				return NULL;
			}

			mInstance = new RenderableObjectSystem();

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

	RenderableObjectSystem * RenderableObjectSystem::GetInstance()
	{
		if (mInstance != NULL)
		{
			return mInstance;
		}

		assert(false);

		return NULL;
	}

	void RenderableObjectSystem::Destroy()
	{
		if (mInstance)
		{
			GraphicsSystem::Destroy();
			delete mInstance;

			return;
		}

		assert(false);
	}
}