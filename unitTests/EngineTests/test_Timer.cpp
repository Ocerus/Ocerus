#include <UnitTest++.h>

#include "Utility/Timer.h"
#include <Windows.h>


TEST(ManualTimer)
{
	Timer timer(true);
	timer.UpdateInMicroseconds(123789);
	CHECK_EQUAL(timer.GetMicroseconds(), 123789);
	CHECK_EQUAL(timer.GetMilliseconds(), 123);

	timer.Reset();
	timer.UpdateInMilliseconds(123789);
	CHECK_EQUAL(timer.GetMicroseconds(), 123789000);
	CHECK_EQUAL(timer.GetMilliseconds(), 123789);
}

TEST(AutomaticTimer)
{
	Timer timer;
	Sleep(49);
	uint64 now = timer.GetMilliseconds();
	CHECK_CLOSE(49, now, 1);

	timer.Reset();
	Sleep(49);
	now = timer.GetMicroseconds();
	CHECK_CLOSE(49000, now, 1000);
}

TEST(ManualTimerPausing)
{
	Timer timer(true);
	timer.UpdateInMilliseconds(123789);
	timer.Pause();
	timer.UpdateInMicroseconds(123789);
	timer.UpdateInMilliseconds(123789);
	CHECK_EQUAL(timer.GetMilliseconds(), 123789);
	timer.Resume();
	timer.UpdateInMilliseconds(1);
	CHECK_EQUAL(timer.GetMilliseconds(), 123790);
}

TEST(AutomaticTimerPausing)
{
	Timer timer;
	Sleep(19);
	timer.Pause();
	Sleep(10);
	CHECK_CLOSE(19, timer.GetMilliseconds(), 1);
	timer.Resume();
	Sleep(10);
	CHECK_CLOSE(29, timer.GetMilliseconds(), 1);
}
