
#include "PreCompiled.h"
#include "Profiling.h"
#include "Debug.h"

#ifdef _PROFILER
namespace Engine
{
	Profiler g_Profiler;

	/******************************************************************************
		Function     : ScopedTimer
		Description  : constructor of scoped timer 
		Input        : const char *i_pName
		Output       : 
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	ScopedTimer::ScopedTimer(const char *i_pName):
		mName(i_pName)
	{
		mStart.CalcCurrentTick();

		if (i_pName == NULL)
		{
			mName = "UnknownTimer";
		}

		#if defined(_DEBUG)
			CONSOLE_PRINT("Scoped timer %s is started", mName);
		#endif
	}

	/******************************************************************************
		Function     : ~ScopedTimer
		Description  : Destructor of scoped timer 
		Input        : 
		Output       : 
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	ScopedTimer::~ScopedTimer()
	{
		double TimeInMS = mStart.GetTickDifferenceinMS();

		g_Profiler.AddTimingData(mName, TimeInMS);

		#if defined(_DEBUG)
			CONSOLE_PRINT("Scoped timer %s is Ended, TimeTaken in MS is %f", mName == NULL ? "UnknownTimer" : mName, TimeInMS);
		#endif
	}

	/******************************************************************************
		Function     : Accumulator
		Description  : accumulator constructor that initilizes the Count, Max, 
						Min and Sum 
		Input        : 
		Output       : 
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	Accumulator::Accumulator():
			mCount(0),
			mMax(0.0f),
			mMin(MAXLONG),
			mSum(0.0f)
	{

	}

	/******************************************************************************
	Function     : Accumulator
	Description  : accumulator constructor that initilizes the Count, Max, 
					Min and Sum with input Time difference
	Input        : double i_TimeDifferenceinMS
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	Accumulator::Accumulator(double i_TimeDifferenceinMS):
		mMax(i_TimeDifferenceinMS),
		mMin(i_TimeDifferenceinMS),
		mSum(i_TimeDifferenceinMS)
	{
		mCount = 1;
	}

	/******************************************************************************
	Function     : AddTimingData
	Description  : Adds timing data with name and tick difference
	Input        : const std::string i_pTimerName, const double i_TimeDifferenceinMS
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	void Profiler::AddTimingData(const std::string i_pTimerName, const double i_TimeDifferenceinMS)
	{
		using namespace std;

		map<string,Accumulator>::iterator iter;

		iter = mProfileData.find(i_pTimerName);

		if (iter != mProfileData.end())
		{
			iter->second.CalcCount();
			iter->second.CalcMax(i_TimeDifferenceinMS);
			iter->second.CalcMin(i_TimeDifferenceinMS);
			iter->second.CalcSum(i_TimeDifferenceinMS);
		}
		else
		{
			Accumulator NewAccumulator(i_TimeDifferenceinMS);
			mProfileData.insert ( pair<string,Accumulator>(i_pTimerName,NewAccumulator) );
		}
	}

	/******************************************************************************
	Function     : PrintTimingData
	Description  : print timing data to Console
	Input        : 
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	void Profiler::PrintTimingData(void)
	{
		using namespace std;

		map<string,Accumulator>::iterator iter;
		
		for (iter = mProfileData.begin(); iter != mProfileData.end(); ++iter)
		{
			Accumulator pIterator = iter->second;

			ProfileDebugPrint("Timer Name: %s			|	Count: %lf	|	Max:%lf	|	Min:%lf	|	Sum:%lf	|	Average:%lf	|", 
									iter->first.c_str(), iter->second.GetCount(), 
									iter->second.GetMax(), iter->second.GetMin(), 
									iter->second.GetSum(), iter->second.GetAverage());
		}
	}

	
	/******************************************************************************
	Function     : ~Profiler
	Description  : Destructor to clear profile data
	Input        : 
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	Profiler::~Profiler()
	{
		mProfileData.clear();
	}

	/******************************************************************************
	Function     : PrintAccumulators
	Description  : Print accumulators by calling the global profiler variable
	Input        : 
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	void PrintAccumulators()
	{
		g_Profiler.PrintTimingData();
	}

	/******************************************************************************
	Function     : ProfileDebugPrint
	Description  : console debug print version for profile
	Input        : 
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	void ProfileDebugPrint(const char *pFormat, ...)
	{
		va_list Args;
		char aDebugString[MAX_DEBUG_STRING + 2] = {'\0'};
		unsigned long ulOutLength;

		if (!pFormat)
		{
			printf("\nNull Pointer Received\n");
		}

		va_start(Args, pFormat);
	
		//ulInputLength = strlen(pFormat);
		//ulMaxLength = ulInputLength > MAX_DEBUG_STRING ? MAX_DEBUG_STRING : ulInputLength;

		ulOutLength = vsprintf_s(aDebugString, MAX_DEBUG_STRING, pFormat, Args);
		va_end(Args);

		//End with a new line and set null character
		aDebugString[ulOutLength++] = '\n';
		aDebugString[ulOutLength] = '\0';

		OutputDebugStringA(aDebugString);
		return;

		return;
	}

}

#endif