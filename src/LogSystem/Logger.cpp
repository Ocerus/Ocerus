#include "Common.h"
#include "Logger.h"
#include "LogMgr.h"

using namespace LogSystem;

LogSystem::Logger::~Logger( void )
{
	string messageString = mMessageBuffer.str();
	if (messageString.size() > 0)
	{
		LogMgr::GetSingleton().LogMessage(messageString, mLogLevel);
	}
}
