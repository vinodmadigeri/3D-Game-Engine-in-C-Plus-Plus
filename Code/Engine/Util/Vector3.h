#ifndef __VECTOR3_H
#define __VECTOR3_H

#include "PreCompiled.h"
#include "MathUtil.h"
#include <math.h>

namespace Engine
{

	class Vector3
	{
	private:
		float fX, fY, fZ;
	public:
		
		Vector3();
		Vector3(const float fXvalue, const float fYvalue, const float fZvalue);

		inline void x(const float fXvalue);
		inline void y(const float fYvalue);
		inline void z(const float fZvalue);

		inline float x() const;
		inline float y() const;
		inline float z() const;
	
		inline void Vector3::GetAsFloatArray(float(&o_Array)[3], int &o_Count) const;

		inline void SetCoordinates(const float fX, const float fY, const float fZ);

		//Binary operators
		inline Vector3 operator+(const Vector3 &RightVector) const;
		inline Vector3 operator-(const Vector3 &RightVector) const;
		inline Vector3 operator*(const Vector3 &RightVector) const;
		inline Vector3 operator/(const Vector3 &RightVector) const;

		inline Vector3 operator+(const float fValue) const;
		inline Vector3 operator-(const float fValue) const;
		inline Vector3 operator*(const float fValue) const;
		inline Vector3 operator/(const float fValue) const;

		//Assignment operators
		inline void operator+=(const Vector3 &RightVector);
		inline void operator-=(const Vector3 &RightVector);
		inline void operator*=(const Vector3 &RightVector);
		inline void operator/=(const Vector3 &RightVector);

	
		inline void operator+=(const float fValue);
		inline void operator-=(const float fValue);
		inline void operator*=(const float fValue);
		inline void operator/=(const float fValue);

		//Increment Operators
		inline void operator++(void);
		inline void operator--(void);

		inline const Vector3 Negated() const;
		inline float Length() const;
		inline const Vector3 Normalized() const;
		inline Vector3 Reflected( const Vector3 & i_in ) const;

 	} ;

	inline bool operator==(const Vector3 &LeftVector, const Vector3 &RightVector);
	inline bool operator!=(const Vector3 &LeftVector, const Vector3 &RightVector);
	inline bool operator>(const Vector3 &LeftVector, const Vector3 &RightVector);
	inline bool operator<(const Vector3 &LeftVector, const Vector3 &RightVector);
	inline bool operator>=(const Vector3 &LeftVector, const Vector3 &RightVector);
	inline bool operator<=(const Vector3 &LeftVector, const Vector3 &RightVector);

	inline float DotProduct(const Vector3 &LeftVector, const Vector3 &RightVector);
	inline Vector3 CrossProduct(const Vector3 &LeftVector, const Vector3 &RightVector);
}

#include"Vector3.inl"

#endif //__VECTOR3_H