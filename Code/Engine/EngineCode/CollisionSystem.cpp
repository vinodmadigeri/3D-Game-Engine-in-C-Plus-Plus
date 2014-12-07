
#include "PreCompiled.h"

#include "CollisionSystem.h"
#include "Actor.h"
#include "Vector4.h"
#include "HighResTime.h"
#include "MathUtil.h"
#include "Debug.h"
#include "PhysicsSystem.h"
#include "Profiling.h"


namespace Engine
{
	unsigned int CollisionSystem::MAX_COLLIDABLE_OBJECTS = 100;
	CollisionSystem * CollisionSystem::mInstance = NULL;
	MemoryPool *CollisionObject::CollisionMemoryPool = NULL;
		
	/******************************************************************************
		Function     : CollisionObject
		Description  : CollisionObject Constructor
		Input        : SharedPointer<Actor> &i_WorldObject, AABB i_WorldBox
		Output       : 
		Return Value :   

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	CollisionObject::CollisionObject(SharedPointer<Actor> &i_WorldObject, AABB i_WorldBox):
		m_WorldObject(i_WorldObject),
		m_WorldBox(i_WorldBox),
		m_CollidedObject(NULL),
		m_CollisionTime(0xffff),
		m_CollisionResponseVector(Vector3(0.0f, 0.0f, 0.0f))
	{
		Matrix4x4 Translation, Rotation;
		Translation.CreateTranslation(m_WorldObject->GetPosition());
		Rotation.CreateZRotation(0.0f);

		Matrix4x4 ObjToWorld = Rotation * Translation;
			
		//Set Local to world matrix of actor for 3D rendering to use
		i_WorldObject->SetLocalToWorldMatrix(ObjToWorld);
	}

	CollisionObject::~CollisionObject()
	{
			
	}

	/******************************************************************************
	Function     : Initilize
	Description  : Function to initilize collision system - Creates Memorypool
	Input        : void
	Output       : void
	Return Value :   

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	CollisionSystem::CollisionSystem()
	{
		bool WereThereErrors = false;

		if (CollisionObject::CollisionMemoryPool == NULL)
		{
			CollisionObject::CollisionMemoryPool = MemoryPool::Create(sizeof(CollisionObject), MAX_COLLIDABLE_OBJECTS);
			if (CollisionObject::CollisionMemoryPool == NULL)
			{
				assert(false);
				WereThereErrors = false;
			}
		}

		mInitilized = !WereThereErrors;
	}

	/******************************************************************************
	Function     : Shutdown
	Description  : Function to Shutdown collision system - destroys Memorypool
	Input        : void
	Output       : void
	Return Value :   

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	CollisionSystem::~CollisionSystem()
	{
		DeleteAllGameObjects();
		mCollisionObjects.clear();

		if (CollisionObject::CollisionMemoryPool != NULL)
		{
			unsigned long o_ulOutPutLen;
			CollisionObject::CollisionMemoryPool->Destroy(&o_ulOutPutLen);
		}
	}

	/******************************************************************************
		Function     : AddActorGameObject
		Description  : Function to add actor game object to collision system
		Input        : SharedPointer<Actor> &i_Object
		Output       : void
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void CollisionSystem::AddActorGameObject(SharedPointer<Actor> &i_Object)
	{
		AABB WorldBox(Vector3(0.0f, 0.0f, 0.0f), i_Object->GetSize().x() * 0.5f, i_Object->GetSize().y() * 0.5f, i_Object->GetSize().z() * 0.5f);

		CollisionObject *NewObject = new CollisionObject(i_Object, WorldBox);

		mCollisionObjects.push_back(NewObject);
	}

	/******************************************************************************
		Function     : DeleteMarkedToDeathGameObjects
		Description  : Function to delete marked to death actor game object from 
					collision system
		Input        : void
		Output       : void
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void CollisionSystem::DeleteMarkedToDeathGameObjects(void)
	{
		for(unsigned int i = 0; i < mCollisionObjects.size(); )
		{
			if (mCollisionObjects[i]->m_WorldObject->IsMarkedForDeath() == true)
			{
				delete mCollisionObjects[i];
				mCollisionObjects.erase(mCollisionObjects.begin() + i);
				continue;
			}

			i++;
		}
	}

	/******************************************************************************
		Function     : DeleteAllGameObjects
		Description  : Function to delete all actor game object from 
					collision system
		Input        : void
		Output       : void
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void CollisionSystem::DeleteAllGameObjects(void)
	{
		for (unsigned long ulCount = 0; ulCount < mCollisionObjects.size(); ulCount++)
		{
			delete mCollisionObjects.at(ulCount);
		}

		mCollisionObjects.clear();
	}

	void CollisionSystem::Update(float i_DeltaTime)
	{
		const long MAX_COLLISION_CHECK_PER_FRAME = 2;

		long CollisionCheckPerFrame = MAX_COLLISION_CHECK_PER_FRAME;

		DeleteMarkedToDeathGameObjects();

		float FirstCollision_DeltaTime = static_cast<float>(MAXDWORD);

		bool bFound = CheckCollision(i_DeltaTime, FirstCollision_DeltaTime);

#if 0
		while (bFound && (CollisionCheckPerFrame > 0))
		{
			for (unsigned int i = 0; i < mCollisionObjects.size(); i++)
			{
				if ((mCollisionObjects[i]->m_CollidedObject != NULL) && (AlmostEqualRelative(FirstCollision_DeltaTime,  mCollisionObjects[i]->m_CollisionTime)))
				{
					CollisionObject *ObjectA = mCollisionObjects[i];
					CollisionObject *ObjectB = mCollisionObjects[i]->m_CollidedObject;

					ObjectA->m_WorldObject->HandleCollision(ObjectA, ObjectB);
					ObjectA->m_CollidedObject = NULL;
					ObjectA->m_CollisionTime = 0xffff;
				}
			}

			//Apply Physics
			PhysicsSystem::ApplyEulerPhysics(FirstCollision_DeltaTime);
			bool bQuit = false;
			RenderingSystem::Render(bQuit);
			i_DeltaTime -= FirstCollision_DeltaTime;
				
			FirstCollision_DeltaTime = static_cast<float>(MAXDWORD);

			bFound = CheckCollision(i_DeltaTime, FirstCollision_DeltaTime);
			CollisionCheckPerFrame--;
		}

		if (i_DeltaTime > 0)
		{
			PhysicsSystem::GetInstance()->ApplyEulerPhysics(i_DeltaTime);
		}
#endif
	}

	/******************************************************************************
		Function     : CheckCollision
		Description  : Function to check collision 
		Input        : float i_DeltaTime, float &o_FirstCollisionTime
		Output       : void
		Return Value : bool

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	bool CollisionSystem::CheckCollision(float i_DeltaTime, float &o_FirstCollisionTime)
	{
		PROFILE_UNSCOPED("Collision")

		bool bFoundCollision = false;

		for(unsigned int i = 0; i < mCollisionObjects.size(); i++)
		{
			mCollisionObjects[i]->m_CollisionTime = 0xffff;
				
			Matrix4x4 Translation, Rotation;
			Translation.CreateTranslation(mCollisionObjects[i]->m_WorldObject->GetPosition());
			Rotation.CreateZRotation(mCollisionObjects[i]->m_WorldObject->GetRotation());

			mCollisionObjects[i]->m_WorldObject->SetLocalToWorldMatrix( Translation * Rotation );
	
		}

		for(unsigned int i = 0; i < mCollisionObjects.size(); i++)
		{
			for(unsigned int j = i + 1; j < mCollisionObjects.size(); j++)
			{
				Vector3 SurfaceNormalA = Vector3(0.0f, 0.0f, 0.0f);
				Vector3 SurfaceNormalB = Vector3(0.0f, 0.0f, 0.0f);
				float CollisionTime = 0.0f;
				bool ACollidesWithB = ((mCollisionObjects[i]->m_WorldObject->mCollidesWithBitIndex & mCollisionObjects[j]->m_WorldObject->mClassBitIndex) != 0);
				bool BCollidesWithA = ((mCollisionObjects[j]->m_WorldObject->mCollidesWithBitIndex & mCollisionObjects[i]->m_WorldObject->mClassBitIndex) != 0);

				if (ACollidesWithB || BCollidesWithA)
				{
					if ( true == CheckOOBBIntersection(mCollisionObjects[i]->m_WorldBox, (mCollisionObjects[i]->m_WorldObject->GetVelocity()), (mCollisionObjects[i]->m_WorldObject->GetLocalToWorldMatrix()), 
									mCollisionObjects[j]->m_WorldBox, (mCollisionObjects[j]->m_WorldObject->GetVelocity()), (mCollisionObjects[j]->m_WorldObject->GetLocalToWorldMatrix()), SurfaceNormalA, SurfaceNormalB, i_DeltaTime, CollisionTime))
					{
						if (o_FirstCollisionTime > CollisionTime)
						{
							o_FirstCollisionTime = CollisionTime;
						}
	
						if (ACollidesWithB)
						{
							mCollisionObjects[i]->m_CollisionResponseVector = SurfaceNormalA;
							mCollisionObjects[i]->m_CollisionTime = CollisionTime;
							mCollisionObjects[i]->m_CollidedObject = mCollisionObjects[j];
							mCollisionObjects[i]->m_WorldObject->HandleCollision(mCollisionObjects[i], mCollisionObjects[j]);
						}

						if (BCollidesWithA)
						{
							mCollisionObjects[j]->m_CollisionResponseVector = SurfaceNormalB;
							mCollisionObjects[j]->m_CollisionTime = CollisionTime;
							mCollisionObjects[j]->m_CollidedObject = mCollisionObjects[i];
							mCollisionObjects[j]->m_WorldObject->HandleCollision(mCollisionObjects[j], mCollisionObjects[i]);
						}
					}
				}
			}
		}

		return bFoundCollision;
	}
	/******************************************************************************
		Function     : AxisRangeRayOverlap
		Description  : Function to check overlap in input axis
		Input        : float i_RangeStart, float i_RangeEnd, float i_RayStart, 
					float i_RayLength, float & o_dEnter, float & o_dExit, 
					Vector3 &SurfaceA, Vector3 &SurfaceB, float DeltaTime
		Output       : 
		Return Value : bool

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	bool CollisionSystem::AxisRangeRayOverlap(float i_RangeStart, float i_RangeEnd, float i_RayStart, float i_RayLength, float & o_dEnter, float & o_dExit, Vector3 &SurfaceA, Vector3 &SurfaceB, float DeltaTime)
	{
		// Make sure i_RangeStart < i_RangeEnd
		if( i_RangeStart > i_RangeEnd )
		{
			float Temp = i_RangeStart;
			i_RangeStart = i_RangeEnd;
			i_RangeEnd = Temp;
		}

		// Handle ray=point case (i.e. no velocity)
		if( i_RayLength == 0.0f )
		{	
			if( ( i_RayStart < i_RangeStart )  ||  (i_RayStart > i_RangeEnd ) )
				return false;
			else
			{
				o_dEnter = 0.0f;
				o_dExit = DeltaTime;

				return true;
			}
		}

		// Calculate overlaps
		o_dEnter = ( i_RangeStart - i_RayStart ) / i_RayLength;
		o_dExit = ( i_RangeEnd - i_RayStart ) / i_RayLength;

		// Make sure o_dEnter < o_dExit
		// This takes care of case of negative ray length (i.e. negative velocity)d
		if( o_dEnter > o_dExit )
		{
			float Temp = o_dEnter;
			o_dEnter = o_dExit;
			o_dExit = Temp;

				
			Vector3 TempVector = SurfaceA;
			SurfaceA = SurfaceB;
			SurfaceB = TempVector;
		}
	
		return !( (o_dEnter >= DeltaTime)  ||  (o_dExit <= 0.0f) );
	}

	/******************************************************************************
		Function     : CheckOOBBIntersection
		Description  : Function to check OOBB intersection of two bounded boxes
		Input        : const AABB & i_BoxA, const Vector3 & i_VelocityA, const Matrix4x4 & i_ObjAtoWorld, 
					const AABB & i_BoxB, const Vector3 & i_VelocityB, const Matrix4x4 & i_ObjBtoWorld,  
					Vector3 &SurfaceNormalA, Vector3 &SurfaceNormalB, float DeltaTime, float &CollisionTime
		Output       : 
		Return Value : bool

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	bool CollisionSystem::CheckOOBBIntersection(const AABB & i_BoxA, const Vector3 & i_VelocityA, const Matrix4x4 & i_ObjAtoWorld,
								const AABB & i_BoxB, const Vector3 & i_VelocityB, const Matrix4x4 & i_ObjBtoWorld,  
								Vector3 &SurfaceNormalA, Vector3 &SurfaceNormalB, float DeltaTime, float &CollisionTime)
	{

		Matrix4x4 WorldToObjA = i_ObjAtoWorld.GetInverse();
		Matrix4x4 WorldToObjB = i_ObjBtoWorld.GetInverse();
		Matrix4x4 ObjAtoObjB = i_ObjAtoWorld * WorldToObjB;
		Matrix4x4 ObjBtoObjA = i_ObjBtoWorld * WorldToObjA;

		float	fLastEnter = 0.0f;
		float	fFirstExit = DeltaTime;

		// A In B
		{
			// Transform Velocities from World CS to ObjB CS
			Vector3	VelAInB = (WorldToObjB * Vector4(i_VelocityA, 0.0f)).GetAsVector3();
			Vector3	VelBInB = (WorldToObjB * Vector4(i_VelocityB, 0.0f)).GetAsVector3();

			// Project ObjA BB extents onto ObjB axis
			float ExtentsX = fabs( i_BoxA.HalfX() * ObjAtoObjB.At(1,1) ) + fabs( i_BoxA.HalfY() * ObjAtoObjB.At(2,1) ) + fabs( i_BoxA.HalfZ() * ObjAtoObjB.At(3,1) );
			float ExtentsY = fabs( i_BoxA.HalfX() * ObjAtoObjB.At(1,2) ) + fabs( i_BoxA.HalfY() * ObjAtoObjB.At(2,2) ) + fabs( i_BoxA.HalfZ() * ObjAtoObjB.At(3,2) );
			float ExtentsZ = fabs( i_BoxA.HalfX() * ObjAtoObjB.At(1,3) ) + fabs( i_BoxA.HalfY() * ObjAtoObjB.At(2,3) ) + fabs( i_BoxA.HalfZ() * ObjAtoObjB.At(3,3) );

			// Move ObjA BB Center to Obj CS
			Vector3 CenterAInB = (Vector4(i_BoxA.Center(), 1.0f) * ObjAtoObjB).GetAsVector3();

			// Create our expanded BB
			AABB	MasterBox = i_BoxB;

			float MasterHalfX = MasterBox.HalfX() + ExtentsX;
			float MasterHalfY = MasterBox.HalfY() + ExtentsY;
			float MasterHalfZ = MasterBox.HalfZ() + ExtentsZ;

			Vector3 MasterVelocity = VelAInB - VelBInB;

			Vector3 SurfaceA(-1.0f, 0.0f, 0.0f);
			Vector3 SurfaceB(1.0f, 0.0f, 0.0f);
			float	fEnter = 0.0f;
			float	fExit = DeltaTime;

			if( !AxisRangeRayOverlap( MasterBox.Center().x() - MasterHalfX, MasterBox.Center().x() + MasterHalfX, CenterAInB.x(), MasterVelocity.x(), fEnter, fExit, SurfaceA, SurfaceB, DeltaTime) )
				return false;

			if( fEnter > fLastEnter )
			{
				SurfaceNormalA = SurfaceA;
				SurfaceNormalB = SurfaceB;
				fLastEnter = fEnter;
			}

			if( fExit < fFirstExit )
			{
				fFirstExit = fExit;
			}

			SurfaceA = Vector3(0.0f, -1.0f, 0.0f);
			SurfaceB = Vector3(0.0f,1.0f, 0.0f);
			if( !AxisRangeRayOverlap( MasterBox.Center().y() - MasterHalfY, MasterBox.Center().y() + MasterHalfY, CenterAInB.y(), MasterVelocity.y(), fEnter, fExit, SurfaceA, SurfaceB, DeltaTime ) )
				return false;

			if( fEnter > fLastEnter )
			{
				SurfaceNormalA = SurfaceA;
				SurfaceNormalB = SurfaceB;
				fLastEnter = fEnter;
			}

			if( fExit < fFirstExit )
			{
				fFirstExit = fExit;
			}

			SurfaceA = Vector3(0.0f, 0.0f, -1.0f);
			SurfaceB = Vector3(0.0f, 0.0f, 1.0f);
			if( !AxisRangeRayOverlap( MasterBox.Center().z() - MasterHalfZ, MasterBox.Center().z() + MasterHalfZ, CenterAInB.z(), MasterVelocity.z(), fEnter, fExit, SurfaceA, SurfaceB, DeltaTime  ) )
				return false;

			if( fEnter > fLastEnter )
			{
				fLastEnter = fEnter;
			}

			if( fExit < fFirstExit )
			{
				fFirstExit = fExit;
			}
		}

		// B In A
		{
			// Transform Velocities from World CS to ObjA CS
			Vector3	VelBInA = (WorldToObjA * Vector4(i_VelocityB, 0.0f)).GetAsVector3();
			Vector3	VelAInA = (WorldToObjA * Vector4(i_VelocityA, 0.0f)).GetAsVector3();

			// Project ObjA BB extents onto ObjB axis
			float ExtentsX = fabs( i_BoxB.HalfX() * ObjAtoObjB.At(1,1) ) + fabs( i_BoxB.HalfY() * ObjAtoObjB.At(2,1) ) + fabs( i_BoxB.HalfZ() * ObjAtoObjB.At(3,1) );
			float ExtentsY = fabs( i_BoxB.HalfX() * ObjAtoObjB.At(1,2) ) + fabs( i_BoxB.HalfY() * ObjAtoObjB.At(2,2) ) + fabs( i_BoxB.HalfZ() * ObjAtoObjB.At(3,2) );
			float ExtentsZ = fabs( i_BoxB.HalfX() * ObjAtoObjB.At(1,3) ) + fabs( i_BoxB.HalfY() * ObjAtoObjB.At(2,3) ) + fabs( i_BoxB.HalfZ() * ObjAtoObjB.At(3,3) );

			// Move ObjB BB Center to ObjA CS
			Vector3 CenterBInA = (Vector4(i_BoxB.Center(), 1.0f) * ObjBtoObjA).GetAsVector3();

			// Create our expanded BB
			AABB MasterBox = i_BoxA;

			float MasterHalfX = MasterBox.HalfX() + ExtentsX;
			float MasterHalfY = MasterBox.HalfY() + ExtentsY;
			float MasterHalfZ = MasterBox.HalfZ() + ExtentsZ;

			Vector3 MasterVelocity = VelBInA - VelAInA;
			Vector3 SurfaceA(-1.0f, 0.0f, 0.0f);
			Vector3 SurfaceB(1.0f, 0.0f, 0.0f);
				
			float fEnter = 0.0f;
			float fExit = DeltaTime;

			if( !AxisRangeRayOverlap( MasterBox.Center().x() - MasterHalfX, MasterBox.Center().x() + MasterHalfX, CenterBInA.x(), MasterVelocity.x(), fEnter, fExit, SurfaceA, SurfaceB, DeltaTime ) )
				return false;
				
			if( fEnter > fLastEnter )
			{
				SurfaceNormalA = SurfaceA;
				SurfaceNormalB = SurfaceB;
				fLastEnter = fEnter;
			}

			if( fExit < fFirstExit )
			{
				fFirstExit = fExit;
			}
				
			SurfaceA = Vector3(0.0f, -1.0f, 0.0f);
			SurfaceB = Vector3(0.0f, 1.0f, 0.0f);
			if( !AxisRangeRayOverlap( MasterBox.Center().y() - MasterHalfY, MasterBox.Center().y() + MasterHalfY, CenterBInA.y(), MasterVelocity.y(), fEnter, fExit, SurfaceA, SurfaceB, DeltaTime ) )
				return false;

			if( fEnter > fLastEnter )
			{
				SurfaceNormalA = SurfaceA;
				SurfaceNormalB = SurfaceB;
				fLastEnter = fEnter;
			}

			if( fExit < fFirstExit )
			{
				fFirstExit = fExit;
			}

			SurfaceA = Vector3(0.0f, 0.0f, -1.0f);
			SurfaceB = Vector3(0.0f, 0.0f, 1.0f);
			if( !AxisRangeRayOverlap( MasterBox.Center().z() - MasterHalfZ, MasterBox.Center().z() + MasterHalfZ, CenterBInA.z(), MasterVelocity.z(), fEnter, fExit, SurfaceA, SurfaceB, DeltaTime ) )
				return false;

			if( fEnter > fLastEnter )
			{
				fLastEnter = fEnter;
			}

			if( fExit < fFirstExit )
			{
				fFirstExit = fExit;
			}

			if (fFirstExit > fLastEnter)
			{
				CollisionTime = fLastEnter;
				return true;
			}

			return false;
		}
	}


	bool CollisionSystem::CreateInstance()
	{
		if (mInstance == NULL)
		{
			mInstance = new CollisionSystem();

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

	CollisionSystem * CollisionSystem::GetInstance()
	{
		if (mInstance != NULL)
		{
			return mInstance;
		}

		assert(false);

		return NULL;
	}

	void CollisionSystem::Destroy()
	{
		if (mInstance)
		{
			delete mInstance;
		}
	}
}