
#include "PreCompiled.h"
#include <math.h>
#include "Matrix4x4.h"
#include "MathUtil.h"

const double PI  =3.141592653589793238463;

namespace Engine
{
	/******************************************************************************
	Function     : Matrix Functions
	Description  :  
	Input        : 
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
		
	static const Matrix4x4 Identity (
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

	Matrix4x4::Matrix4x4
	(
		float i_11, float i_12, float i_13, float i_14,
		float i_21, float i_22, float i_23, float i_24,
		float i_31, float i_32, float i_33, float i_34,
		float i_41, float i_42, float i_43, float i_44
	):
	m11(i_11), m12(i_12), m13(i_13), m14(i_14), 
	m21(i_21), m22(i_22), m23(i_23), m24(i_24), 
	m31(i_31), m32(i_32), m33(i_33), m34(i_34), 
	m41(i_41), m42(i_42), m43(i_43), m44(i_44)
	{

	}

	Matrix4x4::Matrix4x4
	(
	)
	{

	}

	float Matrix4x4::At(int i_Row, int i_Column) const
	{
		switch(i_Row)
		{
			case 1: 
			{
				switch(i_Column)
				{
					case 1:	return m11;
					case 2:	return m12;
					case 3:	return m13;
					case 4:	return m14;
					default:assert(false);
				}
				break;
			}

			case 2: 
			{
				switch(i_Column)
				{
					case 1:	return m21;
					case 2:	return m22;
					case 3:	return m23;
					case 4:	return m24;
					default:assert(false);
				}
				break;
			}

			case 3: 
			{
				switch(i_Column)
				{
					case 1:	return m31;
					case 2:	return m32;
					case 3:	return m33;
					case 4:	return m34;
					default:assert(false);
				}
				break;
			}

			case 4: 
			{
				switch(i_Column)
				{
					case 1:	return m41;
					case 2:	return m42;
					case 3:	return m43;
					case 4:	return m44;
					default:assert(false);
				}
				break;
			}

			default:assert(false);
		}
		
		assert(false);
		return 0;
	}

	void Matrix4x4::SetAt(int i_Row, int i_Column, float i_Value)
	{
		switch(i_Row)
		{
			case 1: 
			{
				switch(i_Column)
				{
					case 1:	m11 = i_Value; break;
					case 2:	m12 = i_Value; break;
					case 3:	m13 = i_Value; break;
					case 4:	m14 = i_Value; break;
					default:assert(false);
				}
				break;
			}

			case 2: 
			{
				switch(i_Column)
				{
					case 1:	m21 = i_Value; break;
					case 2:	m22 = i_Value; break;
					case 3:	m23 = i_Value; break;
					case 4:	m24 = i_Value; break;
					default:assert(false);
				}
				break;
			}

			case 3: 
			{
				switch(i_Column)
				{
					case 1:	m31 = i_Value; break;
					case 2:	m32 = i_Value; break;
					case 3:	m33 = i_Value; break;
					case 4:	m34 = i_Value; break;
					default:assert(false);
				}
				break;
			}

			case 4: 
			{
				switch(i_Column)
				{
					case 1:	m41 = i_Value; break;
					case 2:	m42 = i_Value; break;
					case 3:	m43 = i_Value; break;
					case 4:	m44 = i_Value; break;
					default:assert(false);
				}
				break;
			}

			default:assert(false);
		}
	}


	Matrix4x4::Matrix4x4(const Matrix4x4 & i_Other):
	m11(i_Other.At(1,1)), m12(i_Other.At(1,2)), m13(i_Other.At(1,3)), m14(i_Other.At(1,4)), 
	m21(i_Other.At(2,1)), m22(i_Other.At(2,2)), m23(i_Other.At(2,3)), m24(i_Other.At(2,4)), 
	m31(i_Other.At(3,1)), m32(i_Other.At(3,2)), m33(i_Other.At(3,3)), m34(i_Other.At(3,4)), 
	m41(i_Other.At(4,1)), m42(i_Other.At(4,2)), m43(i_Other.At(4,3)), m44(i_Other.At(4,4))
	{

	}

	Matrix4x4 & Matrix4x4::operator=(const Matrix4x4 & i_rhs)
	{
		m11 = i_rhs.At(1,1); m12 = i_rhs.At(1,2); m13 = i_rhs.At(1,3); m14 = i_rhs.At(1,4); 
		m21 = i_rhs.At(2,1); m22 = i_rhs.At(2,2); m23 = i_rhs.At(2,3); m24 = i_rhs.At(2,4); 
		m31 = i_rhs.At(3,1); m32 = i_rhs.At(3,2); m33 = i_rhs.At(3,3); m34 = i_rhs.At(3,4); 
		m41 = i_rhs.At(4,1); m42 = i_rhs.At(4,2); m43 = i_rhs.At(4,3); m44 = i_rhs.At(4,4);

		return *this;
	}

	void Matrix4x4::CreateIdentity(void)
	{
		m11 = 1.0f; m12 = 0.0f; m13 = 0.0f; m14 = 0.0f; 
		m21 = 0.0f; m22 = 1.0f; m23 = 0.0f; m24 = 0.0f; 
		m31 = 0.0f; m32 = 0.0f; m33 = 1.0f; m34 = 0.0f; 
		m41 = 0.0f; m42 = 0.0f; m43 = 0.0f; m44 = 1.0f;
	}

	void Matrix4x4::CreateXRotation(float i_RotationDegrees)
	{
		float rTheta = static_cast<float>(i_RotationDegrees * (PI / 180.0f)); //Convert in Radians
		
		m11 = 1.0f;		m12 = 0.0f;			m13 = 0.0f;				m14 = 0.0f; 
		m21 = 0.0f;		m22 = cos(rTheta);	m23 = -sin(rTheta);		m24 = 0.0f; 
		m31 = 0.0f;		m32 = sin(rTheta);	m33 = cos(rTheta);		m34 = 0.0f; 
		m41 = 0.0f;		m42 = 0.0f;			m43 = 0.0f;				m44 = 1.0f;
	}

	void Matrix4x4::CreateYRotation(float i_RotationDegrees)
	{
		float rTheta = static_cast<float>(i_RotationDegrees * (PI / 180.0f)); //Convert in Radians
		
		m11 = cos(rTheta);	m12 = 0.0f;		m13 = sin(rTheta);	m14 = 0.0f; 
		m21 = 0.0f;			m22 = 1.0f;		m23 = 0.0f;			m24 = 0.0f; 
		m31 = -sin(rTheta);	m32 = 0.0f;		m33 = cos(rTheta);	m34 = 0.0f; 
		m41 = 0.0f;			m42 = 0.0f;		m43 = 0.0f;			m44 = 1.0f;
	}

	void Matrix4x4::CreateZRotation(float i_RotationDegrees)
	{
		float rTheta = static_cast<float>(i_RotationDegrees * (PI / 180.0f)); //Convert in Radians
		
		m11 = cos(rTheta);	m12 = -sin(rTheta);	m13 = 0.0f;		m14 = 0.0f; 
		m21 = sin(rTheta);	m22 = cos(rTheta);	m23 = 0.0f;		m24 = 0.0f; 
		m31 = 0.0f;			m32 = 0.0f;			m33 = 1.0f;		m34 = 0.0f; 
		m41 = 0.0f;			m42 = 0.0f;			m43 = 0.0f;		m44 = 1.0f;
	}

	void Matrix4x4::CreateTranslation(float i_TranslateX, float i_TranslateY, float i_TranslateZ)
	{
		m11 = 1.0f;		m12 = 0.0f;		m13 = 0.0f;		m14 = i_TranslateX; 
		m21 = 0.0f;		m22 = 1.0f;		m23 = 0.0f;		m24 = i_TranslateY; 
		m31 = 0.0f;		m32 = 0.0f;		m33 = 1.0f;		m34 = i_TranslateZ; 
		m41 = 0.0f;		m42 = 0.0f;		m43 = 0.0f;		m44 = 1.0f;
	}

	void Matrix4x4::CreateTranslation(const Vector3 & i_TranslateVector)
	{
		m11 = 1.0f;		m12 = 0.0f;		m13 = 0.0f;		m14 = i_TranslateVector.x(); 
		m21 = 0.0f;		m22 = 1.0f;		m23 = 0.0f;		m24 = i_TranslateVector.y(); 
		m31 = 0.0f;		m32 = 0.0f;		m33 = 1.0f;		m34 = i_TranslateVector.z(); 
		m41 = 0.0f;		m42 = 0.0f;		m43 = 0.0f;		m44 = 1.0f;
	}

	void Matrix4x4::CreateScale(float i_ScaleX, float i_ScaleY, float i_ScaleZ)
	{
		m11 = i_ScaleX;		m12 = 0.0f;			m13 = 0.0f;			m14 = 0.0f; 
		m21 = 0.0f;			m22 = i_ScaleY;		m23 = 0.0f;			m24 = 0.0f; 
		m31 = 0.0f;			m32 = 0.0f;			m33 = i_ScaleZ;		m34 = 0.0f; 
		m41 = 0.0f;			m42 = 0.0f;			m43 = 0.0f;			m44 = 1.0f;
	}

	void Matrix4x4::CreateScale(const Vector3 & i_ScaleVector)
	{
		m11 = i_ScaleVector.x();	m12 = 0.0f;					m13 = 0.0f;					m14 = 0.0f; 
		m21 = 0.0f;					m22 = i_ScaleVector.y();	m23 = 0.0f;					m24 = 0.0f; 
		m31 = 0.0f;					m32 = 0.0f;					m33 = i_ScaleVector.z();	m34 = 0.0f; 
		m41 = 0.0f;					m42 = 0.0f;					m43 = 0.0f;					m44 = 1.0f;
	}

	void Matrix4x4::Swap(float & i_A, float & i_B) const
	{
		float fTemp = i_A;
		i_A = i_B;
		i_B = fTemp;
	}

	void Matrix4x4::Transpose(void)
	{
			Swap(m12, m21);		Swap(m13, m31);		Swap(m14, m41); 
								Swap(m23, m32);		Swap(m24, m42); 
													Swap(m34, m43); 
	}

	Matrix4x4 Matrix4x4 ::GetTranspose(void) const
	{
		Matrix4x4 TMatrix(  m11, m21, m31, m41,
							m12, m22, m32, m42,
							m13, m23, m33, m43,
							m14, m24, m34, m44 );

		return TMatrix;
	}

	Matrix4x4 Matrix4x4::operator+(const Matrix4x4 & i_rhs) const
	{
		float m11 = this->m11 + i_rhs.At(1,1);
		float m12 = this->m12 + i_rhs.At(1,2);
		float m13 = this->m13 + i_rhs.At(1,3);
		float m14 = this->m14 + i_rhs.At(1,4);

		float m21 = this->m21 + i_rhs.At(2,1);
		float m22 = this->m22 + i_rhs.At(2,2);
		float m23 = this->m23 + i_rhs.At(2,3);
		float m24 = this->m24 + i_rhs.At(2,4);

		float m31 = this->m31 + i_rhs.At(3,1);
		float m32 = this->m32 + i_rhs.At(3,2);
		float m33 = this->m33 + i_rhs.At(3,3);
		float m34 = this->m34 + i_rhs.At(3,4);

		float m41 = this->m41 + i_rhs.At(4,1);
		float m42 = this->m42 + i_rhs.At(4,2);
		float m43 = this->m43 + i_rhs.At(4,3);
		float m44 = this->m44 + i_rhs.At(4,4);

		return Matrix4x4(m11, m12, m13, m14,
						 m21, m22, m23, m24,
						 m31, m32, m33, m34,
						 m41, m42, m43, m44);
	}

	Matrix4x4 Matrix4x4::operator-(const Matrix4x4 & i_rhs) const
	{
		float m11 = this->m11 - i_rhs.At(1,1);
		float m12 = this->m12 - i_rhs.At(1,2);
		float m13 = this->m13 - i_rhs.At(1,3);
		float m14 = this->m14 - i_rhs.At(1,4);

		float m21 = this->m21 - i_rhs.At(2,1);
		float m22 = this->m22 - i_rhs.At(2,2);
		float m23 = this->m23 - i_rhs.At(2,3);
		float m24 = this->m24 - i_rhs.At(2,4);

		float m31 = this->m31 - i_rhs.At(3,1);
		float m32 = this->m32 - i_rhs.At(3,2);
		float m33 = this->m33 - i_rhs.At(3,3);
		float m34 = this->m34 - i_rhs.At(3,4);

		float m41 = this->m41 - i_rhs.At(4,1);
		float m42 = this->m42 - i_rhs.At(4,2);
		float m43 = this->m43 - i_rhs.At(4,3);
		float m44 = this->m44 - i_rhs.At(4,4);

		return Matrix4x4(m11, m12, m13, m14,
						 m21, m22, m23, m24,
						 m31, m32, m33, m34,
						 m41, m42, m43, m44);
	}

	Matrix4x4 Matrix4x4::operator*(const Matrix4x4 & i_rhs) const
	{
		Matrix4x4 T = *this;

		float m11 = T.m11 * i_rhs.At(1,1) + T.m12 * i_rhs.At(2,1) + T.m13 * i_rhs.At(3,1) + T.m14 * i_rhs.At(4,1);
		float m12 = T.m11 * i_rhs.At(1,2) + T.m12 * i_rhs.At(2,2) + T.m13 * i_rhs.At(3,2) + T.m14 * i_rhs.At(4,2);
		float m13 = T.m11 * i_rhs.At(1,3) + T.m12 * i_rhs.At(2,3) + T.m13 * i_rhs.At(3,3) + T.m14 * i_rhs.At(4,3);
		float m14 = T.m11 * i_rhs.At(1,4) + T.m12 * i_rhs.At(2,4) + T.m13 * i_rhs.At(3,4) + T.m14 * i_rhs.At(4,4);

		float m21 = T.m21 * i_rhs.At(1,1) + T.m22 * i_rhs.At(2,1) + T.m23 * i_rhs.At(3,1) + T.m24 * i_rhs.At(4,1);
		float m22 = T.m21 * i_rhs.At(1,2) + T.m22 * i_rhs.At(2,2) + T.m23 * i_rhs.At(3,2) + T.m24 * i_rhs.At(4,2);
		float m23 = T.m21 * i_rhs.At(1,3) + T.m22 * i_rhs.At(2,3) + T.m23 * i_rhs.At(3,3) + T.m24 * i_rhs.At(4,3);
		float m24 = T.m21 * i_rhs.At(1,4) + T.m22 * i_rhs.At(2,4) + T.m23 * i_rhs.At(3,4) + T.m24 * i_rhs.At(4,4);

		float m31 = T.m31 * i_rhs.At(1,1) + T.m32 * i_rhs.At(2,1) + T.m33 * i_rhs.At(3,1) + T.m34 * i_rhs.At(4,1);
		float m32 = T.m31 * i_rhs.At(1,2) + T.m32 * i_rhs.At(2,2) + T.m33 * i_rhs.At(3,2) + T.m34 * i_rhs.At(4,2);
		float m33 = T.m31 * i_rhs.At(1,3) + T.m32 * i_rhs.At(2,3) + T.m33 * i_rhs.At(3,3) + T.m34 * i_rhs.At(4,3);
		float m34 = T.m31 * i_rhs.At(1,4) + T.m32 * i_rhs.At(2,4) + T.m33 * i_rhs.At(3,4) + T.m34 * i_rhs.At(4,4);

		float m41 = T.m41 * i_rhs.At(1,1) + T.m42 * i_rhs.At(2,1) + T.m43 * i_rhs.At(3,1) + T.m44 * i_rhs.At(4,1);
		float m42 = T.m41 * i_rhs.At(1,2) + T.m42 * i_rhs.At(2,2) + T.m43 * i_rhs.At(3,2) + T.m44 * i_rhs.At(4,2);
		float m43 = T.m41 * i_rhs.At(1,3) + T.m42 * i_rhs.At(2,3) + T.m43 * i_rhs.At(3,3) + T.m44 * i_rhs.At(4,3);
		float m44 = T.m41 * i_rhs.At(1,4) + T.m42 * i_rhs.At(2,4) + T.m43 * i_rhs.At(3,4) + T.m44 * i_rhs.At(4,4);

		return Matrix4x4(m11, m12, m13, m14,
						 m21, m22, m23, m24,
						 m31, m32, m33, m34,
						 m41, m42, m43, m44);
	}

	bool Matrix4x4 ::operator==(const Matrix4x4 & i_rhs) const
	{
		if (AlmostEqualRelative(i_rhs.At(1,1), m11) 
			&&  AlmostEqualRelative(i_rhs.At(1,2), m12)
			&&  AlmostEqualRelative(i_rhs.At(1,2), m12)
			&& 	AlmostEqualRelative(i_rhs.At(1,3), m13)
			&& 	AlmostEqualRelative(i_rhs.At(1,4), m14)
			&& 	AlmostEqualRelative(i_rhs.At(2,1), m21)
			&& 	AlmostEqualRelative(i_rhs.At(2,2), m22)
			&& 	AlmostEqualRelative(i_rhs.At(2,3), m23)
			&& 	AlmostEqualRelative(i_rhs.At(2,4), m24)

			&& 	AlmostEqualRelative(i_rhs.At(3,1), m31)
			&& 	AlmostEqualRelative(i_rhs.At(3,2), m32)
			&& 	AlmostEqualRelative(i_rhs.At(3,3), m33)
			&& 	AlmostEqualRelative(i_rhs.At(3,4), m34)
			&& 	AlmostEqualRelative(i_rhs.At(4,1), m41)
			&& 	AlmostEqualRelative(i_rhs.At(4,2), m42)
			&& 	AlmostEqualRelative(i_rhs.At(4,3), m43)
			&& 	AlmostEqualRelative(i_rhs.At(4,4), m44))
		{
			return true;
		}

		return false;
	}

	Vector4 Matrix4x4::MultiplyLeft( const Vector4 & i_Other) const
	{
		//Transpose the existing matrix for Multiply left - Since the translation matrix is store for Multiply Right format
		Matrix4x4 TMatrix = GetTranspose();
		
		float X = i_Other.x() * TMatrix.At(1,1) + i_Other.y() * TMatrix.At(2,1) + i_Other.z() * TMatrix.At(3,1) + i_Other.w() * TMatrix.At(4,1);
		float Y = i_Other.x() * TMatrix.At(1,2) + i_Other.y() * TMatrix.At(2,2) + i_Other.z() * TMatrix.At(3,2) + i_Other.w() * TMatrix.At(4,2);
		float Z = i_Other.x() * TMatrix.At(1,3) + i_Other.y() * TMatrix.At(2,3) + i_Other.z() * TMatrix.At(3,3) + i_Other.w() * TMatrix.At(4,3);
		float W = i_Other.x() * TMatrix.At(1,4) + i_Other.y() * TMatrix.At(2,4) + i_Other.z() * TMatrix.At(3,4) + i_Other.w() * TMatrix.At(4,4);
		
		Vector4 MultipliedVector(X, Y, Z, W);

		return MultipliedVector;
	}

	Vector4 Matrix4x4::MultiplyRight( const Vector4 & i_Other) const
	{
		float X = i_Other.x() * m11 + i_Other.y() * m12 + i_Other.z() * m13 + i_Other.w() * m14;
		float Y = i_Other.x() * m21 + i_Other.y() * m22 + i_Other.z() * m23 + i_Other.w() * m24;
		float Z = i_Other.x() * m31 + i_Other.y() * m32 + i_Other.z() * m33 + i_Other.w() * m34;
		float W = i_Other.x() * m41 + i_Other.y() * m42 + i_Other.z() * m43 + i_Other.w() * m44;
		
		Vector4 MultipliedVector(X, Y, Z, W);

		return MultipliedVector;
	}

	inline float Matrix4x4::MinorDeterminant2x2(float m11, float m12,
												float m21, float m22) const
	{
		return (m11 * m22 - m21 * m12);
	}

	inline float Matrix4x4::MinorDeterminant3x3(float m11, float m12, float m13,
												float m21, float m22, float m23,
												float m31, float m32, float m33) const
	{
		return ((m11 * MinorDeterminant2x2(m22, m23, m32, m33)) - (m12 * MinorDeterminant2x2(m21, m23, m31, m33)) + (m13 * MinorDeterminant2x2(m21, m22, m31, m32)));
	}

	inline float Matrix4x4::Determinant(void)
	{
		return (  (m11 * MinorDeterminant3x3(m22, m23, m24, 
											 m32, m33, m34, 
											 m42, m43, m44))
				- (m12 * MinorDeterminant3x3(m21, m23, m24, 
											 m31, m33, m34, 
											 m41, m43, m44))
				+ (m13 * MinorDeterminant3x3(m21, m22, m24, 
											 m31, m32, m34, 
											 m41, m42, m44))
				- (m14 * MinorDeterminant3x3(m21, m22, m23, 
											 m31, m32, m33, 
											 m41, m42, m43)));
	}
	
	void Matrix4x4::FindInverse(Matrix4x4 & i_Other) const
	{
		float fDeterminant = i_Other.Determinant();
		
		assert(fDeterminant != 0.0f);
		
		Matrix4x4 T = i_Other;

		i_Other.m11 =  (MinorDeterminant3x3(T.m22, T.m23, T.m24, T.m32, T.m33, T.m34, T.m42, T.m43, T.m44) / fDeterminant);
		i_Other.m12 = -(MinorDeterminant3x3(T.m21, T.m23, T.m24, T.m31, T.m33, T.m34, T.m41, T.m43, T.m44) / fDeterminant);
		i_Other.m13 =  (MinorDeterminant3x3(T.m21, T.m22, T.m24, T.m31, T.m32, T.m34, T.m41, T.m42, T.m44) / fDeterminant);
		i_Other.m14 = -(MinorDeterminant3x3(T.m21, T.m22, T.m23, T.m31, T.m32, T.m33, T.m41, T.m42, T.m43) / fDeterminant);

		
		i_Other.m21 = -(MinorDeterminant3x3(T.m12, T.m13, T.m14, T.m32, T.m33, T.m34, T.m42, T.m43, T.m44) / fDeterminant);
		i_Other.m22 =  (MinorDeterminant3x3(T.m11, T.m13, T.m14, T.m31, T.m33, T.m34, T.m41, T.m43, T.m44) / fDeterminant);
		i_Other.m23 = -(MinorDeterminant3x3(T.m11, T.m12, T.m14, T.m31, T.m32, T.m34, T.m41, T.m42, T.m44) / fDeterminant);
		i_Other.m24 =  (MinorDeterminant3x3(T.m11, T.m12, T.m13, T.m31, T.m32, T.m33, T.m41, T.m42, T.m43) / fDeterminant);

		i_Other.m31 =  (MinorDeterminant3x3(T.m12, T.m13, T.m14, T.m22, T.m23, T.m24, T.m42, T.m43, T.m44) / fDeterminant);
		i_Other.m32 = -(MinorDeterminant3x3(T.m11, T.m13, T.m14, T.m21, T.m23, T.m24, T.m41, T.m43, T.m44) / fDeterminant);
		i_Other.m33 =  (MinorDeterminant3x3(T.m11, T.m12, T.m14, T.m21, T.m22, T.m24, T.m41, T.m42, T.m44) / fDeterminant);
		i_Other.m34 = -(MinorDeterminant3x3(T.m11, T.m12, T.m13, T.m21, T.m22, T.m23, T.m41, T.m42, T.m43) / fDeterminant);

		i_Other.m41 = -(MinorDeterminant3x3(T.m12, T.m13, T.m14, T.m22, T.m23, T.m24, T.m32, T.m33, T.m34) / fDeterminant);
		i_Other.m42 =  (MinorDeterminant3x3(T.m11, T.m13, T.m14, T.m21, T.m23, T.m24, T.m31, T.m33, T.m34) / fDeterminant);
		i_Other.m43 = -(MinorDeterminant3x3(T.m11, T.m12, T.m14, T.m21, T.m22, T.m24, T.m31, T.m32, T.m34) / fDeterminant);
		i_Other.m44 =  (MinorDeterminant3x3(T.m11, T.m12, T.m13, T.m21, T.m22, T.m23, T.m31, T.m32, T.m33) / fDeterminant);

		i_Other.Transpose();
	}

	void Matrix4x4::Inverse(void)
	{
		FindInverse(*this);
	}

	Matrix4x4 Matrix4x4::GetInverse(void) const
	{
		Matrix4x4 OutMatrix = *this;

		FindInverse(OutMatrix);

		return OutMatrix;
	}

	/******************************************************************************
	Function     : MatrixUnitTest
	Description  : Unittest function for Matrix class
	Input        : 
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/ 
	void MatrixUnitTest(void)
	{
		//Calculate Multiply
		Matrix4x4 M1(10,5,-2,8,
					4,5,7,-3,
					8,-7,5,3,
					4,5,-1,6);

		Matrix4x4 M2(10,5,-2,8,
					4,5,7,-3,
					8,-7,5,3,
					4,5,-1,6);

		Matrix4x4 M3 = M1 * M2;

		Matrix4x4 PreCalcMultiple( 136,129,-3,107,104,-19,65,20,104,-15,-43,118,76,82,16,50);

		assert(PreCalcMultiple == M3);

		Matrix4x4 M(10,5,-2,8,
					4,5,7,-3,
					8,-7,5,3,
					4,5,-1,6);

		//Check for right determinant and Inverse
		float PreCalcDeterminant = 3232.0f;

		assert(AlmostEqualRelative(PreCalcDeterminant, M.Determinant()));
		
		float m11 = static_cast<float>(0.191212871);
		float m12 = static_cast<float>(0.028774752);
		float m13 = static_cast<float>(-0.010829208);
		float m14 = static_cast<float>(-0.235148515);
		float m21 = static_cast<float>(0.011138614);
		float m22 = static_cast<float>(0.055074257);
		float m23 = static_cast<float>(-0.063737624);
		float m24 = static_cast<float>(0.044554455);
		float m31 = static_cast<float>(-0.189356436);
		float m32 = static_cast<float>(0.063737624);
		float m33 = static_cast<float>(0.083539604);
		float m34 = static_cast<float>(0.242574257);
		float m41 = static_cast<float>(-0.168316832);
		float m42 = static_cast<float>(-0.054455446);
		float m43 = static_cast<float>(0.074257426);
		float m44 = static_cast<float>(0.326732673);

		Matrix4x4 PreCalcInverse( m11,  m12, m13, m14,
								m21, m22, m23, m24, 
								m31, m32, m33, m34, 
								m41, m42, m43, m44);

		M.Inverse();
		assert(PreCalcInverse == M);
	}
}