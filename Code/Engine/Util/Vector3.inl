namespace Engine
{
	inline float Vector3::x() const
	{
		return fX;
	}

	inline float Vector3::y() const
	{
		return fY;
	}

	inline float Vector3::z() const
	{
		return fZ;
	}

	inline void Vector3::x(const float fXvalue)
	{
		assert(!IsNan(fXvalue));
		fX = fXvalue;
	}

	inline void Vector3::y(const float fYvalue)
	{
		assert(!IsNan(fYvalue));
		fY = fYvalue;
	}

	inline void Vector3::z(const float fZvalue)
	{
		assert(!IsNan(fZvalue));
		fZ = fZvalue;
	}

	void Vector3::GetAsFloatArray(float (&o_Array)[3], int &o_Count) const
	{
		o_Array[0] = fX;
		o_Array[1] = fY;
		o_Array[2] = fZ;

		o_Count = 3;
	}

	inline void Vector3::SetCoordinates(const float fXvalue, const float fYvalue, const float fZvalue)
	{
		assert(!IsNan(fXvalue));
		assert(!IsNan(fYvalue));
		assert(!IsNan(fZvalue));

		x(fXvalue);
		y(fYvalue);
		z(fZvalue);
	}


	//Binary Operators
	inline Vector3 Vector3::operator+(const Vector3 &RightVector) const 
	{
		Vector3 Vector3Output;

		Vector3Output.SetCoordinates((x() + RightVector.x()), 
									(y() + RightVector.y()), 
									(z() + RightVector.z()));

		return Vector3Output;
	}

	inline Vector3 Vector3::operator-(const Vector3 &RightVector) const 
	{ 
		Vector3 Vector3Output;

		Vector3Output.SetCoordinates((x() - RightVector.x()), 
									(y() - RightVector.y()), 
									(z() - RightVector.z()));

		return Vector3Output;
	}

	inline Vector3 Vector3::operator*(const Vector3 &RightVector) const 
	{ 
		Vector3 Vector3Output;

		Vector3Output.SetCoordinates((x() * RightVector.x()), 
									(y() * RightVector.y()), 
									(z() * RightVector.z()));

		return Vector3Output;
	}

	inline Vector3 Vector3::operator/(const Vector3 &RightVector) const 
	{ 
		Vector3 Vector3Output;

		Vector3Output.SetCoordinates((x() / RightVector.x()), 
									(y() / RightVector.y()), 
									(z() / RightVector.z()));

		return Vector3Output;
	}

	inline Vector3 Vector3::operator+(const float fValue) const 
	{ 
		Vector3 Vector3Output;
	
		assert(!IsNan(fValue));

		Vector3Output.SetCoordinates((x() + fValue), 
									(y() + fValue), 
									(z() + fValue));

		return Vector3Output;
	}

	inline Vector3 Vector3::operator-(const float fValue) const 
	{ 
		Vector3 Vector3Output;
	
		assert(!IsNan(fValue));
	
		Vector3Output.SetCoordinates((x() - fValue), 
									(y() - fValue), 
									(z() - fValue));

		return Vector3Output;
	}

	inline Vector3 Vector3::operator*(const float fValue) const 
	{ 
		Vector3 Vector3Output;
	
		assert(!IsNan(fValue));
	
		Vector3Output.SetCoordinates((x() * fValue), 
									(y() * fValue), 
									(z() * fValue));

		return Vector3Output;
	}

	inline Vector3 Vector3::operator/(const float fValue) const 
	{ 
		Vector3 Vector3Output;

		assert(!IsNan(fValue));
		assert(0 != fValue);

		Vector3Output.SetCoordinates((x() / fValue), 
									(y() / fValue), 
									(z() / fValue));

		return Vector3Output;
	}


	//Assignment Operators
	inline void Vector3::operator+=(const Vector3 &RightVector)
	{
		SetCoordinates((x() + RightVector.x()), 
					(y() + RightVector.y()), 
					(z() + RightVector.z()));
	}

	inline void Vector3::operator-=(const Vector3 &RightVector)
	{
		SetCoordinates((x() - RightVector.x()), 
					(y() - RightVector.y()), 
					(z() - RightVector.z()));
	}

	inline void Vector3::operator*=(const Vector3 &RightVector)
	{
		SetCoordinates((x() * RightVector.x()), 
					(y() * RightVector.y()), 
					(z() * RightVector.z()));
	}

	inline void Vector3::operator/=(const Vector3 &RightVector)
	{
		SetCoordinates((x() / RightVector.x()), 
					(y() / RightVector.y()), 
					(z() / RightVector.z()));
	}

	inline void Vector3::operator+=(const float fValue)
	{
		assert(!IsNan(fValue));

		SetCoordinates((x() + fValue), 
					(y() + fValue), 
					(z() + fValue));
	}

	inline void Vector3::operator-=(const float fValue)
	{
		assert(!IsNan(fValue));

		SetCoordinates((x() - fValue), 
					(y() - fValue), 
					(z() - fValue));
	}

	inline void Vector3::operator*=(const float fValue)
	{
		assert(!IsNan(fValue));

		SetCoordinates((x() * fValue), 
					(y() * fValue), 
					(z() * fValue));
	}

	inline void Vector3::operator/=(const float fValue)
	{
		assert(!IsNan(fValue));
		assert(0 != fValue);

		SetCoordinates((x() / fValue), 
					(y() / fValue), 
					(z() / fValue));
	}

	//Increment Operators
	inline void Vector3::operator++(void)
	{
		SetCoordinates((x() + 1), (y() + 1), (z() + 1));
	}

	inline void Vector3::operator--(void)
	{
		SetCoordinates((x() - 1), (y() - 1), (z() - 1));
	}

	inline Vector3 ZeroVector(void)
	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	inline const Vector3 Vector3::Negated() const
	{
		return Vector3( -fX, -fY, -fZ );
	}

	inline float Vector3::Length() const { return sqrt( (fX * fX) + (fY * fY) + (fZ * fZ)); }

	inline const Vector3 Vector3::Normalized() const
	{
		float length = Length();
		
		if( length )
			return Vector3( fX / length, fY / length, fZ / length );
		else
			return Vector3( 0.0f, 0.0f, 0.0f );
	}

	inline Vector3 Vector3::Reflected( const Vector3 & i_in ) const
	{
		Vector3 Reversed = (*this).Negated();

		float proj = DotProduct( i_in, Reversed );

		Vector3 Proj = i_in.Normalized() * proj;

		return Reversed - (Reversed - Proj) * 2.0f;
	}

	//Conditional Operators
	inline bool operator==(const Vector3 &LeftVector, const Vector3 &RightVector)
	{
		if (!(AlmostEqualRelative(LeftVector.x(), RightVector.x()))
				|| !(AlmostEqualRelative(LeftVector.y(), RightVector.y()))
				|| !(AlmostEqualRelative(LeftVector.z(), RightVector.z())))
		{
			return false;
		}

		return true;
	}

	//Comparision Operators
	inline bool operator!=(const Vector3 &LeftVector, const Vector3 &RightVector)
	{
		if (AlmostEqualRelative(LeftVector.x(), RightVector.x())
				&& AlmostEqualRelative(LeftVector.y(), RightVector.y())
				&& AlmostEqualRelative(LeftVector.z(), RightVector.z()))
		{
			return false;
		}

		return true;
	}

	inline bool operator>(const Vector3 &LeftVector, const Vector3 &RightVector)
	{
		bool bCondition = false;

		if (LeftVector == RightVector)
		{
			bCondition = false;
		}
		else if ((LeftVector.x() > RightVector.x()) 
				&& (LeftVector.y() > RightVector.y()) 
				&& (LeftVector.z() > RightVector.z()))
		{
			bCondition = true;
		}

		return bCondition;
	}

	inline bool operator<(const Vector3 &LeftVector, const Vector3 &RightVector)
	{
		bool bCondition = false;

		if (LeftVector == RightVector)
		{
			bCondition = false;
		}
		else if ((LeftVector.x() < RightVector.x()) 
				&& (LeftVector.y() < RightVector.y()) 
				&& (LeftVector.z() < RightVector.z()))
		{
			bCondition = true;
		}

		return bCondition;
	}

	inline bool operator>=(const Vector3 &LeftVector, const Vector3 &RightVector)
	{
		bool bCondition = false;

		if (LeftVector == RightVector)
		{
			bCondition = true;
		}
		else if (LeftVector > RightVector)
		{
			bCondition = true;
		}

		return bCondition;
	}


	inline bool operator<=(const Vector3 &LeftVector, const Vector3 &RightVector)
	{
		bool bCondition = false;

		if (LeftVector == RightVector)
		{
			bCondition = true;
		}
		else if (LeftVector < RightVector)
		{
			bCondition = true;
		}

		return bCondition;
	}

	inline float DotProduct(const Vector3 &LeftVector, const Vector3 &RightVector)
	{
		float fX = LeftVector.x() * RightVector.x();
		float fY = LeftVector.y() * RightVector.y();
		float fZ = LeftVector.z() * RightVector.z();

		return (fX + fY + fZ);
	}

	inline Vector3 CrossProduct(const Vector3 &LeftVector, const Vector3 &RightVector)
	{
		float fX = LeftVector.y() * RightVector.z() - LeftVector.z() * RightVector.y();
		float fY = LeftVector.z() * RightVector.x() - LeftVector.x() * RightVector.z();
		float fZ = LeftVector.x() * RightVector.y() - LeftVector.y() * RightVector.x();

		return Vector3(fX, fY, fZ);
	}

}