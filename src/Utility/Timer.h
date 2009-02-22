#ifndef Timer_h__
#define Timer_h__

#include "Settings.h"

/** This class measures time.
*/
class Timer
{
public:
	Timer(const bool manual = false);
	~Timer(void);

	/// @name Resets the counter.
	void Reset(void);
	/// @name Pauses the timer until it is resumed or reset.
	void Pause(void);
	/// @name Resumes work of the timer.
	void Resume(void);
	/// @name Updates the time if it's manual.
	//@{
	void UpdateInMicroseconds(const uint64 delta);
	void UpdateInMilliseconds(const uint64 delta);
	void UpdateInSeconds(const float64 delta);
	//@}
	/// @name Returns time since reset in milliseconds.
	uint64 GetMilliseconds(void);
	/// @name Returns time since reset in microseconds.
	uint64 GetMicroseconds(void);

private:
	/// @name Winapi stuff.
	//@{
	int64 mStartTime;
	int64 mFrequency;
	uint32 mStartTick;
	int64 mLastTime;
	//@}

	/// @name Stuff for pausing.
	//@{
	int64 mPausedTimeMicro; // in microseconds
	int64 mTotalPauseDelta;
	bool mPaused;
	//@}

	const bool mManual;
	

};

#endif // Timer_h__