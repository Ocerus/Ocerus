#include "Common.h"
#include "Timer.h"
#include <Windows.h>

Timer::Timer()
{
	Reset();
}

Timer::~Timer() {}

void Timer::Reset()
{
	LARGE_INTEGER large;
	QueryPerformanceFrequency(&large);
	mFrequency = large.QuadPart;
	QueryPerformanceCounter(&large);
	mStartTime = large.QuadPart;
	mStartTick = GetTickCount();
	mLastTime = 0;
}

uint64 Timer::GetMilliseconds()
{
	return GetMicroseconds()/1000;
}

uint64 Timer::GetMicroseconds()
{
	LARGE_INTEGER curTime;
	QueryPerformanceCounter(&curTime);
	// time since last reset
	LONGLONG newTime = curTime.QuadPart - mStartTime;
	// scale up to get milliseconds
	uint64 newTicks = (uint64) (1000 * newTime / mFrequency);

	// detect and compensate for performance counter leaps
	// (surprisingly common, see Microsoft KB: Q274323)
	uint64 check = GetTickCount() - mStartTick;
	int64 msecOff = (int64)(newTicks - check);
	if (msecOff < -100 || msecOff > 100)
	{
		// We must keep the timer running forward :)
		int64 adjust = (std::min)(msecOff * mFrequency / 1000, newTime - mLastTime);
		mStartTime += adjust;
		newTime -= adjust;

		// Re-calculate milliseconds
		newTicks = (uint64) (1000 * newTime / mFrequency);
	}

	// Record last time for adjust
	mLastTime = newTime;

	uint64 micro = (uint64) (1000000 * newTime / mFrequency);

	return micro;
}
