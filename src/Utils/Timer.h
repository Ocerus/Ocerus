#ifndef Timer_h__
#define Timer_h__

#include "Settings.h"

namespace Utils
{
	/// @brief This class measures time.
	/// @remarks The timer can be in two modes - automatic or manual. If it's automatic you don't need to update it and
	/// just get the time when needed. If it's manual you have to call one of the update methods to move the time forward.
	/// This way you have more control of the time. For example, in a game you want the timer to update only when the
	/// game is actually in progress and is not paused.
	class Timer
	{
	public:

		/// @brief Constructs the timer.
		/// @remarks If manual is set to false, the timer will be constructed in the automatic mode, otherwise
		/// it will be constructed in the manual mode.
		Timer(const bool manual = false);

		~Timer(void);

		/// Resets the current time to zero.
		void Reset(void);

		/// Pauses the timer until it is resumed or reset.
		void Pause(void);

		/// Resumes work of the timer.
		void Resume(void);

		/// Updates the time if the timer is in the manual mode.
		void UpdateInMicroseconds(const uint64 delta);

		/// Updates the time if the timer is in the manual mode.
		void UpdateInMilliseconds(const uint64 delta);

		/// Updates the time if the timer is in the manual mode.
		void UpdateInSeconds(const float64 delta);

		/// Returns time since last reset in milliseconds.
		uint64 GetMilliseconds(void);

		/// Returns time since last reset in microseconds.
		uint64 GetMicroseconds(void);

	private:

		int64 mStartTime;
		int64 mFrequency;
		uint32 mStartTick;
		int64 mLastTime;

		int64 mPausedTimeMicro; // in microseconds
		int64 mTotalPauseDelta;
		bool mPaused;

		const bool mManual;
	};
}

#endif // Timer_h__