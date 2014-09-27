
#include "PreCompiled.h"
#include "Debug.h"
#include "HighResTime.h"

namespace Engine
{

	static const double SEC_TO_MILLISEC = 1000.0f;
	static const double CONSTANT_TIME_FRAME = 1000.0f / 600.0f;

	/******************************************************************************
	Function     : Initilize
	Description  : Function to initilize HighResTimer, calculates FrameFrequency 
					and PreviousFrameCounter
	Input        : 
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	void HighResTimer::Initilize(void)
	{
		if (QueryPerformanceFrequency(&FrameFrequency) == 0)
		{
			assert(false);
		}

		if (QueryPerformanceCounter(&PreviousFrameCounter) == 0)
		{
			assert(false);
		}

		//Warmup
		CalculateFrameTime();
		CalculateFrameTime();
	}

	/******************************************************************************
	Function     : CalculateFrameTime
	Description  : Function to calculate frame time and store it in LastFrameinMS
	Input        : 
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	void HighResTimer::CalculateFrameTime(void)
	{
		assert(FrameFrequency.QuadPart > 0);

		LARGE_INTEGER CurrentFrameCounter = {0};

		assert(TRUE == QueryPerformanceCounter(&CurrentFrameCounter));
		
		QueryPerformanceCounter(&CurrentFrameCounter);

		double FrameTime = ((CurrentFrameCounter.QuadPart - PreviousFrameCounter.QuadPart) * SEC_TO_MILLISEC ) / FrameFrequency.QuadPart;
		
		LastFrameinMS = (FrameTime);

		//CONSOLE_PRINT("FrameTime %f", LastFrameinMS);

		PreviousFrameCounter = CurrentFrameCounter;

		return;
	}

	/******************************************************************************
	Function     : GetLastFrameMS
	Description  : function to get last frame time
	Input        : 
	Output       : delta time
	Return Value : double

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	double HighResTimer::GetLastFrameMS(void)
	{
	#ifdef _STATIC_TIMEFRAME_
		return CONSTANT_TIME_FRAME;
	#else
		//CONSOLE_PRINT("LastFrameinMS %f", LastFrameinMS);
		return LastFrameinMS;
	#endif
	}

	/******************************************************************************
	Function     : Tick
	Description  : constructor for Tick class
	Input        : 
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	Tick::Tick()
	{
		if (QueryPerformanceFrequency(&Frequency) == 0)
		{
			assert(false);
		}
	}

	/******************************************************************************
	Function     : CalcCurrentTick
	Description  : function to calculate Curren tick
	Input        : 
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	void Tick::CalcCurrentTick(void)
	{
		if (QueryPerformanceCounter(&Counter) == 0)
		{
			assert(false);
		}
	}

	/******************************************************************************
	Function     : GetTickDifferenceinMS
	Description  : function to return the Tick Difference in Milli Seconds
	Input        : 
	Output       : Tick Difference
	Return Value : double

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	double Tick::GetTickDifferenceinMS(void) const
	{
		assert(Frequency.QuadPart > 0);

		LARGE_INTEGER CurrentCounter = {0};

		QueryPerformanceCounter(&CurrentCounter);

		LONGLONG diff = (CurrentCounter.QuadPart - Counter.QuadPart);
		return double( diff * SEC_TO_MILLISEC ) / Frequency.QuadPart;
	}
}