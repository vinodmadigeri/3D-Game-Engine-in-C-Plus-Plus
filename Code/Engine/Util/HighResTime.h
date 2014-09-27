#ifndef __HIGH_RES_TIMER
#define __HIGH_RES_TIMER

#include "PreCompiled.h"

namespace Engine
{
	class HighResTimer
	{
		double LastFrameinMS;
		LARGE_INTEGER PreviousFrameCounter;
		LARGE_INTEGER FrameFrequency;
	public:
		void Initilize(void);
		void CalculateFrameTime(void);
		double GetLastFrameMS(void);
	};
	
	class Tick
	{
		LARGE_INTEGER Counter;
		LARGE_INTEGER Frequency;
	public:
		Tick();
		void CalcCurrentTick(void);
		double GetTickDifferenceinMS(void) const;
	};



}
#endif //__HIGH_RES_TIMER