
#include "PreCompiled.h"
#include <Math.h>

#include "MathUtil.h"



float fEpsilon = 0.00001f;

namespace Engine
{
	/******************************************************************************
	 Function     : IsNan
	 Description  : Check if the input float is not a number
	 Input        : const float fvalue
	 Output       : 
	 Return Value : bool
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	bool IsNan(const float fvalue)
	{
		volatile float fvolfloat = fvalue;

		if (fvolfloat != fvolfloat)
		{
			return true;
		}
	
		return false;
	}

	/******************************************************************************
	 Function     : SetEpsilonValue
	 Description  : Set the global EpsilonValue
	 Input        : const float fvalue
	 Output       : 
	 Return Value : void
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	void SetEpsilonValue(const float fvalue)
	{
		assert(!IsNan(fvalue));
		fEpsilon = fvalue;
	}

	/******************************************************************************
	 Function     : AlmostEqualRelative
	 Description  : Check if two floats are equal
	 Input        : const float fA, const float fB
	 Output       : 
	 Return Value : bool
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	bool AlmostEqualRelative(const float fA, const float fB)
	{
		float relativeError;

		assert(!IsNan(fA));
		assert(!IsNan(fB));

		if (fA == fB)
		{
			return true;
		}

		if ((fA == 0.0f) || (fB == 0.0f))
		{
			return false;
		}

		if (fabs(fB) > fabs(fA))
		{
			relativeError = fabs((fA - fB) / fB);
		}
		else
		{
			relativeError = fabs((fA - fB) / fA);
		}

		if (relativeError <= fEpsilon)
		{
			return true;
		}

		return false;
	}

	/******************************************************************************
	 Function     : MathUtil_UnitTest
	 Description  : Unit Test for Math util functions
	 Input        : void
	 Output       : 
	 Return Value : void
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	void MathUtil_UnitTest(void)
	{
		float fa, fb, fc, fd;
		float fValue = -1.0f;
		volatile float fToBeNan;

		SetEpsilonValue(0.00001f);

		fa = 0.1f;
		fb = 0.1f;
		fc = 5.0f;
		fd = 0.2f;
		assert((AlmostEqualRelative(fa / fc, fb * fd)));
		assert(!(AlmostEqualRelative(fc, fd)));

		fToBeNan = sqrt(fValue);

		assert(IsNan(fToBeNan));
		assert(!IsNan(fb));

		return;
	}
}