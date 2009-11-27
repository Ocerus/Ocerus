#include "Common.h"
#include "UnitTests.h"
#include "../Timer.h"


#ifdef __WIN__

#include <Windows.h>

inline void mySleep(uint64 ms)
{
	Sleep((DWORD)ms);
}

#else

#include <unistd.h>

inline void mySleep(uint64 ms)
{
	usleep(ms * 1000);
}

#endif


SUITE(Timer)
{
	const uint64 TOLERANCY_MILLIS = 1;

	TEST(ManualTimer)
	{
		Timer timer(true);
		timer.UpdateInMicroseconds(123789);
		CHECK_EQUAL(timer.GetMicroseconds(), 123789U);
		CHECK_EQUAL(timer.GetMilliseconds(), 123U);

		timer.Reset();
		timer.UpdateInMilliseconds(123789);
		CHECK_EQUAL(timer.GetMicroseconds(), 123789000U);
		CHECK_EQUAL(timer.GetMilliseconds(), 123789U);
	}

	TEST(AutomaticTimer)
	{
		Timer timer;
		mySleep(49);
		uint64 now = timer.GetMilliseconds();
		CHECK(now >= 49-TOLERANCY_MILLIS);

		timer.Reset();
		mySleep(49);
		now = timer.GetMicroseconds();
		CHECK(now >= 49000-TOLERANCY_MILLIS*1000);
	}

	TEST(ManualTimerPausing)
	{
		Timer timer(true);
		timer.UpdateInMilliseconds(123789);
		timer.Pause();
		timer.UpdateInMicroseconds(123789);
		timer.UpdateInMilliseconds(123789);
		CHECK_EQUAL(timer.GetMilliseconds(), 123789U);
		timer.Resume();
		timer.UpdateInMilliseconds(1);
		CHECK_EQUAL(timer.GetMilliseconds(), 123790U);
	}

	TEST(AutomaticTimerPausing)
	{
		Timer timer;
		mySleep(19);
		timer.Pause();
		mySleep(10);
		CHECK(timer.GetMilliseconds() >= 19-TOLERANCY_MILLIS);
		timer.Resume();
		mySleep(10);
		CHECK(timer.GetMilliseconds() >= 29-TOLERANCY_MILLIS);
	}
}
