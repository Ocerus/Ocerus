#ifndef Timer_h__
#define Timer_h__

#include "Settings.h"
#include "Windows.h"

class Timer
{
public:
	Timer(void);
	~Timer(void);

	void Reset(void);
	uint64 GetMilliseconds(void);
	uint64 GetMicroseconds(void);

private:
	LARGE_INTEGER mStartTime;
	LARGE_INTEGER mFrequency;
	DWORD mStartTick;
	LONGLONG mLastTime;
};

#endif // Timer_h__