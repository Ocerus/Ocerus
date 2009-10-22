#include "Common.h"
#ifdef UNIT_TESTS
#include "../Core/UnitTests.h"

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
		CHECK_CLOSE(49, now, 1U);

		timer.Reset();
		mySleep(49);
		now = timer.GetMicroseconds();
		CHECK_CLOSE(49000, now, 1000U);
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
		CHECK_CLOSE(19, timer.GetMilliseconds(), 1U);
		timer.Resume();
		mySleep(10);
		CHECK_CLOSE(29, timer.GetMilliseconds(), 1U);
	}
}

#endif