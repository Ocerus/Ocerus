#include "Common.h"
#include "SmartAssert.h"

#ifdef __WIN__

#include <Windows.h>
void DisplayAssert(const char* msg, const char* file, const int line)
{
	MessageBox(NULL, msg, "An assertion failed!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
}

#else

void DisplayAssert(const char* msg, const char* file, const int line)
{
	/// Smart enough for linux geeks 8D
    fprintf(stderr, "%s:%d %s\n", file, line, msg);
}

#endif

#ifdef USE_ASSERT
namespace boost
{
    void assertion_failed(char const* expr, char const* function, char const* file, long line)
    {
        (void) function;
        DisplayAssert(expr, file, line);
    }
}
#endif // USE_ASSERT
