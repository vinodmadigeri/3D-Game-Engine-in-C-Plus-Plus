

namespace Engine
{
	inline Vector4::Vector4()
	{

	}

	inline Vector4::Vector4(const Vector3 & i_Vector3, float i_w = 1.0f):
	fX(i_Vector3.x()),	fY(i_Vector3.y()),	fZ(i_Vector3.z()),	fW(i_w)
	{
		
	}

	inline Vector4::Vector4(const float i_x, const float i_y, const float i_z):
	fX(i_x),	fY(i_y),	fZ(i_z),	fW(1.0f)
	{

	}

	inline Vector4::Vector4(const float i_x, const float i_y, const float i_z, const float i_w):
	fX(i_x),	fY(i_y),	fZ(i_z),	fW(i_w)
	{

	}

	inline Vector4::Vector4(const Vector4 & i_Vector4):
	fX(i_Vector4.x()),	fY(i_Vector4.y()),	fZ(i_Vector4.z()),	fW(i_Vector4.w())
	{

	}

	inline Vector4::~Vector4()
	{

	}

	inline float Vector4::x() const
	{
		return fX;
	}

	inline Vector3 Vector4::GetAsVector3(void) const
	{
		return Vector3(fX, fY, fZ);
	}

	inline float Vector4::y() const
	{
		return fY;
	}

	inline float Vector4::z() const
	{
		return fZ;
	}

	inline float Vector4::w() const
	{
		return fW;
	}

	inline void Vector4::x(const float fXvalue)
	{
		assert(!IsNan(fXvalue));
		fX = fXvalue;
	}

	inline void Vector4::y(const float fYvalue)
	{
		assert(!IsNan(fYvalue));
		fY = fYvalue;
	}

	inline void Vector4::z(const float fZvalue)
	{
		assert(!IsNan(fZvalue));
		fZ = fZvalue;
	}

	inline void Vector4::w(const float fWvalue)
	{
		assert(!IsNan(fWvalue));
		fW = fWvalue;
	}
}