#ifndef __AABB_HEADER
#define __AABB_HEADER

#include "PreCompiled.h"
#include "Vector3.h"

namespace Engine
{
	class AABB
	{
		Vector3				m_Center;
	
		float				m_HalfX;
		float				m_HalfY;
		float				m_HalfZ;

	public:
		AABB(const Vector3 &i_Center, float i_halfX, float i_halfY, float i_halfZ);

		inline float HalfX(void) const
		{
			return m_HalfX;
		}

		inline float HalfY(void) const
		{
			return m_HalfY;
		}

		inline float HalfZ(void) const
		{
			return m_HalfZ;
		}

		inline Vector3 Center(void) const
		{
			return m_Center;
		}

	} ;
}

#endif //__AABB_HEADER