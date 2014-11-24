#ifndef __LIGHTING_SYSTEM_H
#define __LIGHTING_SYSTEM_H
#include "Actor.h"
#include "SharedPointer.h"

namespace Engine
{
	class LightingSystem
	{
	private:
		LightingSystem(SharedPointer<Actor> &i_WorldObject, const Vector3 & iAmbientLight, const Vector3 & iDiffuseLight, const Vector3 & iLightDirection);
		~LightingSystem();
		LightingSystem(const LightingSystem & i_Other);
		LightingSystem & operator=(const LightingSystem & i_rhs);

		static LightingSystem * mInstance;

		bool mInitilized;
		Vector3 mAmbientLight;
		Vector3 mDiffuseLight;
		Vector3 mLightDirection;

	public:
		SharedPointer<Actor> m_WorldObject;
		void Update(float i_deltaTime);
		
		inline Vector3 GetAmbientLight() const { return mAmbientLight; }
		inline Vector3 GetDiffuseLight() const { return mDiffuseLight; }
		inline Vector3 GetLightDirection() const { return mLightDirection; }

		inline void SetAmbientLight(const Vector3 & iAmbientLight) { mAmbientLight = iAmbientLight; }
		inline void SetDiffuseLight(const Vector3 & iDiffuseLight) { mDiffuseLight = iDiffuseLight; }
		inline void SetLightDirection(const Vector3 & iLightDirection) { mLightDirection = iLightDirection; }

		static bool CreateInstance(const Vector3 & iAmbientLight, const Vector3 & iDiffuseLight, const Vector3 & iLightDirection);

		static LightingSystem * GetInstance();
		static void Destroy();
	};
}
#endif //__LIGHTING_SYSTEM_H