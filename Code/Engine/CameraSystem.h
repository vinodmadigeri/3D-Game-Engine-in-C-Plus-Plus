#ifndef __CAMERA_SYSTEM_H
#define __CAMERA_SYSTEM_H

#include "PreCompiled.h"
#include <d3dx9math.h>
#include "Actor.h"
#include "SharedPointer.h"

namespace Engine
{
	//class Actor; //Forward decleration

	class CameraSystem
	{
	private:
		float				mRotation;
		CameraSystem(SharedPointer<Actor> &i_WorldObject,
			const unsigned int i_windowWidth,
			const unsigned int i_windowHeight,
			const float i_YFOV,
			const float i_ZNear,
			const float i_ZFar,
			const Vector3 & i_LookAt,
			const Vector3 & i_Up);
		~CameraSystem();
		CameraSystem(const CameraSystem & i_Other);
		CameraSystem & operator=(const CameraSystem & i_rhs);

		bool CreateViewToScreen(const float i_YFOV, const float i_ZNear, const float i_ZFar);
		bool CreateWorldToView(const D3DXVECTOR3 & i_Eye,
								const D3DXVECTOR3 & i_LookAt,
								const D3DXVECTOR3 & i_Up);

		//float				mRotation;

		static D3DXVECTOR3 mLookAt, mUp;
		static D3DXMATRIXA16 s_worldToView; //View matrix
		static D3DXMATRIXA16 s_viewToScreen; //Projection matrix
		static CameraSystem * mInstance;
		bool mInitilized;

		unsigned int m_windowWidth;
		unsigned int m_windowHeight;
		float m_YFOV;
		float m_ZNear;
		float m_ZFar;
		D3DXVECTOR3 m_LookAt;
		D3DXVECTOR3 m_Up;
	public:
		SharedPointer<Actor> m_WorldObject;
		void Update();
		const D3DXMATRIXA16 GetWorldToView() const;
		const D3DXMATRIXA16 GetViewToScreen() const;

		static bool CreateInstance(const unsigned int i_windowWidth,
			const unsigned int i_windowHeight,
			const float i_YFOV, const float i_ZNear,
			const float i_ZFar,
			const Vector3 & i_Eye,
			const Vector3 & i_LookAt,
			const Vector3 & i_Up);
		
		
		static CameraSystem * GetInstance();
		static void Destroy();
	} ;
}
#endif //__CAMERA_SYSTEM_H