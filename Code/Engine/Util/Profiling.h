#ifndef __PROFILING_HEADER
#define __PROFILING_HEADER

#include "PreCompiled.h"
#include <map>

#include "HighResTime.h"

#ifdef _PROFILER

namespace Engine
{
	class ScopedTimer
	{
		const char *mName;
		Tick mStart;

	public:
		ScopedTimer(const char *i_pName);
		~ScopedTimer();
	} ;

	class Accumulator
	{
		long double mCount;
		long double mMax;
		long double mMin;
		long double mSum;
	
	public:
		Accumulator();

		Accumulator(double i_TimeDifferenceinMS);

		inline void CalcCount(void)
		{
			mCount++;
		}

		inline void CalcSum(double i_TimeDifference)
		{
			mSum += i_TimeDifference;
		}

		inline void CalcMax(double i_TimeDifference)
		{
			if (mMax < i_TimeDifference)
			{
				mMax = i_TimeDifference;
			}
		}
		
		inline void CalcMin(double i_TimeDifference)
		{
			if (mMin > i_TimeDifference)
			{
				mMin = i_TimeDifference;
			}
		}

		inline long double GetAverage()
		{
			if (mCount != 0)
			{
				return mSum / mCount;
			}
			else
			{
				return 0.0f;
			}
		}

		inline long double GetCount(void) const
		{
			return mCount;
		}

		inline long double GetSum(void) const
		{
			return mSum;
		}

		inline long double GetMax(void) const
		{
			return mMax;
		}
		
		inline long double GetMin(void) const
		{
			return mMin;
		}
	} ;

	class Profiler
	{
		std::map<std::string, Accumulator> mProfileData;
	public:
		~Profiler();
		void AddTimingData(const std::string i_pTimerName, const double i_TimeDifferenceinMS);
		void PrintTimingData(void);
	} ;

	void PrintAccumulators();
	void ProfileDebugPrint(const char *pFormat, ...);
}
#endif

#if defined(_PROFILER)
#define PROFILE_SCOPE_BEGIN(Name) { Gameme::ScopedTimer MyTimer(Name);
#define PROFILE_SCOPE_END() }
#define PROFILE_UNSCOPED(Name) Gameme::ScopedTimer MyTimer(Name);
#define PROFILE_PRINT_RESULTS() Gameme::PrintAccumulators();
#else
#define PROFILE_SCOPE_BEGIN(Name) __noop;
#define PROFILE_SCOPE_END() __noop;
#define PROFILE_UNSCOPED(Name) __noop;
#define PROFILE_PRINT_RESULTS() __noop;
#endif

#endif //__PROFILING_HEADER

