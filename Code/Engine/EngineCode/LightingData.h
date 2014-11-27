#ifndef __LIGHTING_DATA
#define __LIGHTING_DATA

#include "PreCompiled.h"
#include "Vector3.h"

namespace Engine
{
	struct LightingData
	{
		std::string mLightingDataName;
		Vector3 mDataValue;

		LightingData(const char * Name, const float iData0, const float iData1, const float iData2)
		{
			assert(Name);
			mLightingDataName = Name;
			mDataValue = Vector3(iData0, iData1, iData2);
		}
	};
}

#endif //__LIGHTING_DATA