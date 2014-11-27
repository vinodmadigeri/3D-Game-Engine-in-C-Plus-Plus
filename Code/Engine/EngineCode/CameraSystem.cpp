
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

		m_LookAt = i_LookAt.GetAsD3DXVECTOR3();
		m_Up = i_Up.GetAsD3DXVECTOR3();

		if (!CreateViewToScreen(m_YFOV, m_ZNear, m_ZFar))
		{
			assert(false);
		}

		mInitilized = !WereThereErrors;
	}

	CameraSystem::~CameraSystem()
	{

	}

	void CameraSystem::Update(float i_deltaTime)
	{
		if (!CreateWorldToView(m_WorldObject->GetPosition().GetAsD3DXVECTOR3(), m_LookAt, m_Up))
		{
			assert(false);
		}

		m_WorldObject->Update(i_deltaTime);
	}

	bool CameraSystem::CreateViewToScreen(const float i_YFOV, const float i_ZNear, const float i_ZFar)
	{
		assert(m_windowHeight > 0.0f);
		assert(m_windowWidth > 0.0f);

		float aspect = (static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight));

		D3DXMatrixPerspectiveFovLH(&s_viewToScreen, i_YFOV, aspect, i_ZNear, i_ZFar);
		return true;
	}

	bool CameraSystem::CreateWorldToView(const D3DXVECTOR3 & i_position,
		const D3DXVECTOR3 & i_LookAt,
		const D3DXVECTOR3 & i_Up)
	{
		D3DXMATRIX transform_viewToWorld;
		{
			D3DXMatrixTransformation(&transform_viewToWorld,
				NULL, NULL, NULL, NULL,
				NULL, &(i_position));
		}

		// D3DX can calculate the inverse of any matrix:
		//D3DXMATRIX transform_worldToView;
		//D3DXMatrixInverse( &transform_worldToView, NULL, &transform_viewToWorld );
		// But it can be calculated more cheaply since we know a camera can only have
		// rotation and translation:
#if 1
		s_worldToView = D3DXMATRIX(
			transform_viewToWorld._11, transform_viewToWorld._21, transform_viewToWorld._31, 0.0f,
			transform_viewToWorld._12, transform_viewToWorld._22, transform_viewToWorld._32, 0.0f,
			transform_viewToWorld._13, transform_viewToWorld._23, transform_viewToWorld._33, 0.0f,
			-(transform_viewToWorld._41 * transform_viewToWorld._11) - (transform_viewToWorld._42 * transform_viewToWorld._12) - (transform_viewToWorld._43 * transform_viewToWorld._13),
			-(transform_viewToWorld._41 * transform_viewToWorld._21) - (transform_viewToWorld._42 * transform_viewToWorld._22) - (transform_viewToWorld._43 * transform_viewToWorld._23),
			-(transform_viewToWorld._41 * transform_viewToWorld._31) - (transform_viewToWorld._42 * transform_viewToWorld._32) - (transform_viewToWorld._43 * transform_viewToWorld._33),
			1.0f);
#else
		D3DXMatrixLookAtLH(&s_worldToView, &i_position, &i_LookAt, &i_Up);
#endif
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

			mInstance = new CameraSystem(NewActor, i_windowWidth, i_windowHeight, i_YFOV, i_ZNear, i_ZFar, i_LookAt, i_Up);

			if (mInstance == NULL)
			{
				return false;
			}

			if (mInstance->mInitilized == false)
			{
				delete mInstance;
				return false;
			}

			assert(PhysicsSystem::GetInstance());

			PhysicsSystem::GetInstance()->AddActorGameObject(NewActor);
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