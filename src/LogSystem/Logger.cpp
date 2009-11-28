#include "Common.h"
#include "Logger.h"
#include "LogMgr.h"

#ifdef USE_DBGLIB

#ifdef __WIN__
#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#endif

#include <DbgLib/DbgLib.h>

#endif

using namespace LogSystem;

LogSystem::Logger::~Logger( void )
{
#ifdef USE_DBGLIB
	if (mGenerateStackTrace)
	{
		mMessageBuffer << "\n STACK TRACE:";
		const size_t stackTraceMaxSize = 10;
		string stackTrace[stackTraceMaxSize];
		size_t stackTraceSize = DbgLib::CDebugFx::GenerateStackTrace(stackTrace, stackTraceMaxSize);
		for (DbgLib::uintx i=1; i<stackTraceSize; ++i)
		{
			mMessageBuffer << "\n " << stackTrace[i];
		}
	}
#endif

	string messageString = mMessageBuffer.str();
	if (messageString.size() > 0)
	{
		LogMgr::GetSingleton().LogMessage(messageString, mLogLevel);
	}
}
