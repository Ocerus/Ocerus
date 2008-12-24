#include "Common.h"
#include "Timer.h"
#include <Windows.h>

Timer::Timer(const bool manual): mManual(manual)
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
	mPausedTimeMicro = 0;
	mTotalPauseDelta = 0;
	mPaused = false;
}

uint64 Timer::GetMilliseconds()
{
	return GetMicroseconds()/1000;
}

uint64 Timer::GetMicroseconds()
{
	if (mManual)
	{
		return mLastTime - mTotalPauseDelta;
	}
	else
	{
		if (mPaused)
			return mPausedTimeMicro;

		LARGE_INTEGER curTime;
		QueryPerformanceCounter(&curTime);
		// time since last reset
		LONGLONG newTime = curTime.QuadPart - mStartTime;

		// detect and compensate for performance counter leaps
		// (surprisingly common, see Microsoft KB: Q274323)
		// scale up to get milliseconds
		uint64 newTicks = (uint64) (1000 * newTime / mFrequency);
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
	
		return micro - mTotalPauseDelta;;
	}
}

void Timer::Pause( void )
{
	if (!mPaused)
	{
		mPausedTimeMicro = GetMicroseconds();
		mPaused = true;
	}
}

void Timer::Resume( void )
{
	if (mPaused)
	{
		mPaused = false; // must unpause here so that GetMicroseconds returns something reasonable.
		mTotalPauseDelta += GetMicroseconds() - mPausedTimeMicro;
	}
}

void Timer::UpdateInMicroseconds( const uint64 delta )
{
	assert(mManual && "Timer must be set to manual control to be updatable");
	if (!mPaused)
		mLastTime += delta;
}

void Timer::UpdateInMiliseconds( const uint64 delta )
{
	UpdateInMicroseconds(1000 * delta);
}

void Timer::UpdateInSeconds( const float64 delta )
{
	UpdateInMicroseconds((uint64)(1000000.0f * delta));
}