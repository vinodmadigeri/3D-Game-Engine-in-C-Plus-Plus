
#include "PreCompiled.h"
#include "CameraSystem.h"
#include "Actor.h"
#include "PhysicsSystem.h"

namespace Engine
{
	D3DXMATRIXA16 CameraSystem::s_worldToView; //View matrix
	D3DXMATRIXA16 CameraSystem::s_viewToScreen; //Projection matrix
	D3DXVECTOR3 CameraSystem::mLookAt;
	D3DXVECTOR3 CameraSystem::mUp;

	CameraSystem * CameraSystem ::mInstance = NULL;

	CameraSystem::CameraSystem(SharedPointer<Actor> &i_WorldObject,
		const unsigned int i_windowWidth,
		const unsigned int i_windowHeight,
		const float i_YFOV, 
		const float i_ZNear,
		const float i_ZFar,
		const Vector3 & i_LookAt,
		const Vector3 & i_Up) :
		m_WorldObject(i_WorldObject),
		m_windowWidth(i_windowWidth),
		m_windowHeight(i_windowHeight),
		m_YFOV(i_YFOV),
		m_ZNear(i_ZNear),
		m_ZFar(i_ZFar)
	{
		bool WereThereErrors = false;

		m_LookAt = D3DXVECTOR3(i_LookAt.x(), i_LookAt.y(), i_LookAt.z());
		m_Up = D3DXVECTOR3(i_Up.x(), i_Up.y(), i_Up.z());

		mInitilized = !WereThereErrors;
	}

	CameraSystem::~CameraSystem()
	{

	}

	void CameraSystem::Update()
	{
		D3DXVECTOR3 CurrentPosition = D3DXVECTOR3(m_WorldObject->GetPosition().x(), m_WorldObject->GetPosition().y(), m_WorldObject->GetPosition().z());

		if (!CreateWorldToView(CurrentPosition, m_LookAt, m_Up))
		{
			assert(false);
		}

		if (!CreateViewToScreen(m_YFOV, m_ZNear, m_ZFar))
		{
			assert(false);
		}
	}

	bool CameraSystem::CreateViewToScreen(const float i_YFOV, const float i_ZNear, const float i_ZFar)
	{
		assert(m_windowHeight > 0.0f);
		assert(m_windowWidth > 0.0f);

		float aspect = static_cast<float>(m_windowWidth / m_windowHeight);

		D3DXMatrixPerspectiveFovLH(&s_viewToScreen, i_YFOV, aspect, i_ZNear, i_ZFar);
		return true;
	}

	bool CameraSystem::CreateWorldToView(const D3DXVECTOR3 & i_position,
		const D3DXVECTOR3 & i_LookAt,
		const D3DXVECTOR3 & i_Up)
	{

		D3DXMatrixLookAtLH(&s_worldToView, &i_position, &i_LookAt, &i_Up);

		return true;
	}

	const D3DXMATRIXA16 CameraSystem::GetWorldToView() const
	{
		return s_worldToView;
	}

	const D3DXMATRIXA16 CameraSystem::GetViewToScreen() const
	{
		return s_viewToScreen;
	}

	bool CameraSystem::CreateInstance(const unsigned int i_windowWidth,
									const unsigned int i_windowHeight,
									const float i_YFOV, const float i_ZNear,
									const float i_ZFar,
									const Vector3 & i_Eye,
									const Vector3 & i_LookAt,
									const Vector3 & i_Up)
	{
		if (mInstance == NULL)
		{
			Vector3 InitialVelocity(0.0f, 0.0f, 0.0f);
			Vector3 InitialAccln(0.0f, 0.0f, 0.0f);
			Vector3 Size(0.4f, 0.4f, 0.4f);
			float Rotation = 0.0f;

			SharedPointer<Actor> NewActor = Actor::Create(i_Eye, InitialVelocity, InitialAccln, "Camera", "Camera", Size, Rotation);
			assert(NewActor != NULL);

			mInstance = new CameraSystem(NewActor, i_windowWidth, i_windowHeight, i_YFOV, i_ZFar, i_ZNear, i_LookAt, i_Up);

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

	CameraSystem * CameraSystem::GetInstance()
	{
		if (mInstance != NULL)
		{
			return mInstance;
		}

		assert(false);

		return NULL;
	}

	void CameraSystem::Destroy()
	{
		if (mInstance)
		{
			delete mInstance;

			return;
		}

		assert(false);
	}
}