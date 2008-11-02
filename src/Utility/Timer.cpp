#include "Timer.h"

Timer::Timer()
{
	Reset();
}

Timer::~Timer() {}

void Timer::Reset()
{
	QueryPerformanceFrequency(&mFrequency);
	QueryPerformanceCounter(&mStartTime);
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
	LONGLONG newTime = curTime.QuadPart - mStartTime.QuadPart;
	// scale up to get milliseconds
	uint64 newTicks = (uint64) (1000 * newTime / mFrequency.QuadPart);

	// detect and compensate for performance counter leaps
	// (surprisingly common, see Microsoft KB: Q274323)
	uint64 check = GetTickCount() - mStartTick;
	int64 msecOff = (int64)(newTicks - check);
	if (msecOff < -100 || msecOff > 100)
	{
		// We must keep the timer running forward :)
		LONGLONG adjust = (std::min)(msecOff * mFrequency.QuadPart / 1000, newTime - mLastTime);
		mStartTime.QuadPart += adjust;
		newTime -= adjust;

		// Re-calculate milliseconds
		newTicks = (uint64) (1000 * newTime / mFrequency.QuadPart);
	}

	// Record last time for adjust
	mLastTime = newTime;

	uint64 micro = (uint64) (1000000 * newTime / mFrequency.QuadPart);

	return micro;
}
