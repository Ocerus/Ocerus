#include "Common.h"
#include "SmartAssert.h"

#ifdef __WIN__

#include <Windows.h>
void DisplayAssert(const char* msg, const char* file, const int line)
{
	MessageBox(NULL, msg, "An assertion failed!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
}

void CRITICAL_FAILURE( const char* msg )
{
	MessageBox(NULL, msg, "Critical failure!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	exit(1);
}

#else

void DisplayAssert(const char* msg, const char* file, const int line)
{
	// Smart enough for linux geeks 8D
    fprintf(stderr, "%s:%d %s\n", file, line, msg);
}

void CRITICAL_FAILURE( const char* msg )
{
    fprintf(stderr, "%s\n", msg);
	exit(1);
}

#endif



// force Boost to use our assert implementation
#include <boost/assert.hpp>

namespace boost
{
    void assertion_failed(char const* expr, char const* function, char const* file, long line)
    {
        (void) function;
        DisplayAssert(expr, file, line);
		DEBUG_BREAK();
    }
}
