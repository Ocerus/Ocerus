#include "Common.h"
#include "Timer.h"

#ifdef __WIN__
    #include <Windows.h>
#else
    #include <sys/time.h>
    int64 GetMicrosecondsUnix()
    {
        timeval ts;
        gettimeofday(&ts, 0);
        return (int64)(ts.tv_sec * 1000000 + ts.tv_usec);
    }
#endif

Timer::Timer(const bool manual): mManual(manual)
{
	Reset();
}

Timer::~Timer() {}

void Timer::Reset()
{
#ifdef __WIN__
	LARGE_INTEGER large;
	QueryPerformanceFrequency(&large);
	mFrequency = large.QuadPart;
	QueryPerformanceCounter(&large);
	mStartTime = large.QuadPart;
	mStartTick = GetTickCount();
#else
    mStartTime = GetMicrosecondsUnix();
    mStartTick = 0; // not used on unices 
#endif
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

#ifdef __WIN__
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
#else
        int64 curTime = GetMicrosecondsUnix();
        uint64 micro = (uint64) (curTime - mStartTime);
#endif
		return micro - mTotalPauseDelta;
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
	OC_ASSERT_MSG(mManual, "Timer must be set to manual control to be updatable");
	if (!mPaused)
		mLastTime += delta;
}

void Timer::UpdateInMilliseconds( const uint64 delta )
{
	UpdateInMicroseconds(1000 * delta);
}

void Timer::UpdateInSeconds( const float64 delta )
{
	UpdateInMicroseconds((uint64)(1000000.0f * delta));
}
