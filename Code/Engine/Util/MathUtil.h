#ifndef __MATH_UTIL_H
#define __MATH_UTIL_H

#include "PreCompiled.h"

namespace Engine
{
	double Get_PI_Value();
	bool IsNan(const float fvalue);
	void SetEpsilonValue(const float fvalue);
	bool AlmostEqualRelative( const float fA, const float fB);
	void MathUtil_UnitTest(void);
}
#endif //__MATH_UTIL_H