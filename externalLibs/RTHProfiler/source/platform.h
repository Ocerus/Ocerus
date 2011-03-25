
#ifndef platform_h__
#define platform_h__

#ifdef _WIN32

inline void Profile_Get_Ticks(int64 * ticks)
{
	__asm
	{
		push eax;
		push edx;
		push ecx;
		mov ecx,ticks;
		_emit 0Fh
		_emit 31h
		mov [ecx],eax;
		mov [ecx+4],edx;
		pop ecx;
		pop edx;
		pop eax;
	}
}

#include <windows.h>

inline float Profile_Get_Tick_Rate(void)
{
	static float _CPUFrequency = -1.0f;

	if (_CPUFrequency == -1.0f) {
		int64 curr_rate = 0;
		::QueryPerformanceFrequency ((LARGE_INTEGER *)&curr_rate);
		_CPUFrequency = (float)curr_rate;
	} 

	return _CPUFrequency;
}

#else

#include <sys/types.h>
#include <sys/time.h>

inline void Profile_Get_Ticks(int64* ticks)
{
	timeval t1;
	gettimeofday(&t1, NULL);
	*ticks = t1.tv_usec + t1.tv_sec * 1000000;
}

inline float Profile_Get_Tick_Rate(void)
{
	return 1000000.0;
}

#endif

#endif // platform_h__
