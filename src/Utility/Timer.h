#ifndef Timer_h__
#define Timer_h__

#include "Settings.h"

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
	int64 mStartTime;
	int64 mFrequency;
	uint32 mStartTick;
	int64 mLastTime;
};

#endif // Timer_h__