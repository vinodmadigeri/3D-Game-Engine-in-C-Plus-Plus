#include "PreCompiled.h"
#include "LightingSystem.h"

namespace Engine
{
	LightingSystem * LightingSystem::mInstance = NULL;

	LightingSystem::LightingSystem(SharedPointer<Actor> &i_WorldObject, const Vector3 & iAmbientLight, const Vector3 & iDiffuseLight, const Vector3 & iLightDirection):
		m_WorldObject(i_WorldObject),
		mAmbientLight(iAmbientLight),
		mDiffuseLight(iDiffuseLight),
		mLightDirection(iLightDirection)
	{
		bool WereThereErrors = false;

		mInitilized = !WereThereErrors;
	}

	LightingSystem::~LightingSystem()
	{

	}

	void LightingSystem::Update(float i_deltaTime)
	{
		m_WorldObject->Update(i_deltaTime);
	}

	bool LightingSystem::CreateInstance(const Vector3 & iAmbientLight, const Vector3 & iDiffuseLight, const Vector3 & iLightDirection)
	{
		if (mInstance == NULL)
		{
			Vector3 InitialPosition(0.0f, 0.0f, 0.0f);
			Vector3 InitialVelocity(0.0f, 0.0f, 0.0f);
			Vector3 InitialAccln(0.0f, 0.0f, 0.0f);
			Vector3 Size(0.4f, 0.4f, 0.4f);
			float Rotation = 0.0f;

			SharedPointer<Actor> NewActor = Actor::Create(InitialPosition, InitialVelocity, InitialAccln, "DiffuseLight", "Light", Size, Rotation);
			assert(NewActor != NULL);

			mInstance = new LightingSystem(NewActor, iAmbientLight, iDiffuseLight, iLightDirection);

			if (mInstance == NULL)
			{
				return false;
			}

			if (mInstance->mInitilized == false)
			{
				delete mInstance;
				return false;
			}

			//assert(LightingSystem::GetInstance());

			//PhysicsSystem::GetInstance()->AddActorGameObject(NewActor);
		}

		return true;
	}

	LightingSystem * LightingSystem::GetInstance()
	{
		if (mInstance != NULL)
		{
			return mInstance;
		}

		assert(false);

		return NULL;
	}

	void LightingSystem::Destroy()
	{
		if (mInstance)
		{
			delete mInstance;

			return;
		}

		assert(false);
	}
}
