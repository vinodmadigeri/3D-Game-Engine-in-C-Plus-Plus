#ifndef __RANDOM_GEN_H
#define __RANDOM_GEN_H

#include "PreCompiled.h"
#include <stdlib.h>

namespace Engine
{
	/******************************************************************************
	 Function     : GenerateRandomNumber
	 Description  : Function to generate random number withing min and max range
	 Input        : const int iMinRange, const int iMaxRange
	 Output       : 
	 Return Value : float  
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	inline float GenerateRandomNumber(const int iMinRange, const int iMaxRange);
}

#include "RandomNumber.inl"

#endif // __RANDOM_GEN_H
