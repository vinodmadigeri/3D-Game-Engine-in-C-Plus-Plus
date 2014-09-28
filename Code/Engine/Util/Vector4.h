#ifndef __VECTOR4_HEADER
#define __VECTOR4_HEADER

#include "PreCompiled.h"
#include "Vector3.h"
#include "MathUtil.h"

namespace Engine
{
	class Vector4
	{
	public:
		Vector4(void);
		Vector4(const Vector3 & i_Vector3, float i_w);
		Vector4(const float i_x, const float i_y, const float i_z);
		Vector4(const float i_x, const float i_y, const float i_z, const float i_w);
		Vector4(const Vector4 & i_Vector4);
		~Vector4();

		void x(const float fXvalue);
		void y(const float fYvalue);
		void z(const float fZvalue);
		void w(const float fWvalue);

		float x() const;
		float y() const;
		float z() const;
		float w() const;
		
		Vector3 GetAsVector3(void) const;

	private:
		float fX, fY, fZ, fW;
	} ;
}

#include "Vector4.inl"

#endif //__VECTOR4_HEADER