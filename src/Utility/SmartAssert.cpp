#include "Common.h"
#include "SmartAssert.h"
#include <Windows.h>

void DisplayAssert(const char* msg, const char* file, const int line)
{
	MessageBox(NULL, msg, "An assertion failed!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
}