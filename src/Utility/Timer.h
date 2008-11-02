#ifndef Timer_h__
#define Timer_h__

#include "Settings.h"
#include <Windows.h>

/** This class measures time.
*/
class Timer
{
public:
	Timer(void);
	~Timer(void);

	/// Resets the counter.
	void Reset(void);
	/// Returns time since reset in milliseconds.
	uint64 GetMilliseconds(void);
	/// Returns time since reset in microseconds.
	uint64 GetMicroseconds(void);

private:
	LARGE_INTEGER mStartTime;
	LARGE_INTEGER mFrequency;
	DWORD mStartTick;
	LONGLONG mLastTime;
};

#endif // Timer_h__