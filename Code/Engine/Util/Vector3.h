#ifndef __VECTOR3_H
#define __VECTOR3_H

#include "PreCompiled.h"
#include <D3dx9math.h>
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

		void x(const float fXvalue);
		void y(const float fYvalue);
		void z(const float fZvalue);

		float x() const;
		float y() const;
		float z() const;
	
		D3DXVECTOR3 GetAsD3DXVECTOR3() const;
		void Vector3::GetAsFloatArray(float(&o_Array)[3], int &o_Count) const;

		void SetCoordinates(const float fX, const float fY, const float fZ);

		//Binary operators
		Vector3 operator+(const Vector3 &RightVector) const;
		Vector3 operator-(const Vector3 &RightVector) const;
		Vector3 operator*(const Vector3 &RightVector) const;
		Vector3 operator/(const Vector3 &RightVector) const;

		Vector3 operator+(const float fValue) const;
		Vector3 operator-(const float fValue) const;
		Vector3 operator*(const float fValue) const;
		Vector3 operator/(const float fValue) const;

		//Assignment operators
		void operator+=(const Vector3 &RightVector);
		void operator-=(const Vector3 &RightVector);
		void operator*=(const Vector3 &RightVector);
		void operator/=(const Vector3 &RightVector);

	
		void operator+=(const float fValue);
		void operator-=(const float fValue);
		void operator*=(const float fValue);
		void operator/=(const float fValue);

		//Increment Operators
		void operator++(void);
		void operator--(void);

		const Vector3 Negated() const;
		float Length() const;
		const Vector3 Normalized() const;
		Vector3 Reflected( const Vector3 & i_in ) const;
		Vector3 Truncated(const float iMaxVal) const;
		void Truncate(const float iMaxVal);

 	} ;

	bool operator==(const Vector3 &LeftVector, const Vector3 &RightVector);
	bool operator!=(const Vector3 &LeftVector, const Vector3 &RightVector);
	bool operator>(const Vector3 &LeftVector, const Vector3 &RightVector);
	bool operator<(const Vector3 &LeftVector, const Vector3 &RightVector);
	bool operator>=(const Vector3 &LeftVector, const Vector3 &RightVector);
	bool operator<=(const Vector3 &LeftVector, const Vector3 &RightVector);

	float DotProduct(const Vector3 &LeftVector, const Vector3 &RightVector);
	Vector3 CrossProduct(const Vector3 &LeftVector, const Vector3 &RightVector);
}

#include"Vector3.inl"

#endif //__VECTOR3_H