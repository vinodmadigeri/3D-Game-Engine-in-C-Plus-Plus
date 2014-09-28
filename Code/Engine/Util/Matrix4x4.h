#ifndef __MATXIT4X4_HEADER
#define __MATXIT4X4_HEADER

#include "PreCompiled.h"
#include "Vector4.h"

namespace Engine
{
	class Matrix4x4
	{
		float 
			m11, m12, m13, m14,
			m21, m22, m23, m24,
			m31, m32, m33, m34,
			m41, m42, m43, m44;

		void Swap(float & i_A, float & i_B) const;

		float MinorDeterminant3x3(float m11, float m12, float m13,
								  float m21, float m22, float m23,
								  float m31, float m32, float m33) const;

		float MinorDeterminant2x2(float m11, float m12,
								  float m21, float m22) const;

		void FindInverse(Matrix4x4 & i_Other) const;

	public:
		Matrix4x4(void);
		Matrix4x4(
			float i_11, float i_12, float i_13, float i_14,
			float i_21, float i_22, float i_23, float i_24,
			float i_31, float i_32, float i_33, float i_34,
			float i_41, float i_42, float i_43, float i_44);

		Matrix4x4(const Matrix4x4 & i_Other);

		Matrix4x4 & operator=(const Matrix4x4 & i_rhs);

		float At(int i_Row, int i_Column) const;

		void SetAt(int i_Row, int i_Column, float i_Value);

		void CreateIdentity(void);
		void CreateXRotation(float i_RotationDegrees);
		void CreateYRotation(float i_RotationDegrees);
		void CreateZRotation(float i_RotationDegrees);

		void CreateTranslation(float i_TranslateX, float i_TranslateY, float i_TranslateZ);
		void CreateTranslation(const Vector3 & i_TranslateVector);
		void CreateScale(float i_ScaleX, float i_ScaleY, float i_ScaleZ);
		void CreateScale(const Vector3 & i_ScaleVector);

		void Transpose(void); //Changes the Matrix4x4 internally
		Matrix4x4 GetTranspose(void) const; //Doesnt change the internal values of Matrix4x4, returns a transpose

		void Inverse(void); //Changes the Matrix4x4 internally

		Matrix4x4 GetInverse(void) const; //Doesnt change the internal values of Matrix4x4, returns an Inverse

		//Addition for Matrix4x4
		Matrix4x4 operator+(const Matrix4x4 & i_rhs) const;

		//Subtraction for Matrix4x4
		Matrix4x4 operator-(const Matrix4x4 & i_rhs) const;

		//Multiply for Matrix4x4
		Matrix4x4 operator*(const Matrix4x4 & i_rhs) const;

		//Is equal to for Matrix4x4
		bool operator==(const Matrix4x4 & i_rhs) const;

		//Multiply for Vector4
		Vector4 MultiplyLeft( const Vector4 & i_Other) const;
		Vector4 MultiplyRight( const Vector4 & i_Other) const;

		float Determinant(void);

		static const Matrix4x4 Identity;
	} ;

	inline Vector4 operator*(const Matrix4x4 & i_lhsMatrix, const Vector4 & i_rhsVector4)
	{
		return i_lhsMatrix.MultiplyRight(i_rhsVector4);
	}

	inline Vector4 operator*(const Vector4 & i_rhsVector4, const Matrix4x4 & i_lhsMatrix4x4)
	{
		return i_lhsMatrix4x4.MultiplyLeft(i_rhsVector4);
	}

	//Create an alias for chache allignment
	typedef __declspec(align(64)) Matrix4x4 CacheAllignedMatrix4x4;

	void MatrixUnitTest(void);
}
#endif //__MATXIT4X4_HEADER