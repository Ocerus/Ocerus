#include "Common.h"
#include "LogMgr.h"
//TODO std pryc!
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace LogSystem;


LogSystem::LogMgr::LogMgr(const String& name, eLogSeverity severityLevel): mSeverityLevel(severityLevel)
{
	mOutStream.open(name.c_str());

	gLogMgr.LogMessage("Log created", LOG_INFO);
}

LogSystem::LogMgr::~LogMgr()
{
	mOutStream.close();
}


void LogSystem::LogMgr::LogMessage(const String& msg, eLogSeverity severity)
{
	if (severity >= mSeverityLevel)
	{
		#pragma warning(disable: 4996)
		struct tm* locTime;
		time_t ctime;
		time(&ctime);
		locTime = localtime(&ctime);

		std::stringstream ss;
		ss << std::setw(2) << std::setfill('0') << locTime->tm_hour
			<< ":" << std::setw(2) << std::setfill('0') << locTime->tm_min
			<< ":" << std::setw(2) << std::setfill('0') << locTime->tm_sec
			<< ":";
		if (severity == LOG_ERROR)
			ss << " !! ERROR !! ";
		ss << msg << std::endl;
		String& str = ss.str();

		gApp.WriteToConsole(str);
		/*if (GUISystem::GUIMgr::GetSingletonPtr())
			gGUIMgr.AddConsoleMessage(str);*/

		mOutStream << str;
		mOutStream.flush();
	}
}

void LogSystem::LogMgr::LogMessage(const String& msg, const uint32 num, eLogSeverity severity)
{
	std::stringstream ss;
	ss << msg << " (" << num << ")";
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage(const String& msg, const uint64 num, eLogSeverity severity)
{
	std::stringstream ss;
	ss << msg << " (" << num << ")";
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage(const String& msg, const int32 num, eLogSeverity severity)
{
	std::stringstream ss;
	ss << msg << " (" << num << ")";
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const String& msg, const float32 num, eLogSeverity severity )
{
	std::stringstream ss;
	ss << msg << " (" << num << ")";
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const String& msg, const String& msg2, eLogSeverity severity )
{
	std::stringstream ss;
	ss << msg << msg2;
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const String& msg, const String& msg2, const String& msg3, eLogSeverity severity )
{
	std::stringstream ss;
	ss << msg << msg2 << msg3;
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const String& msg, const String& msg2, const String& msg3, const String& msg4, eLogSeverity severity )
{
	std::stringstream ss;
	ss << msg << msg2 << msg3 << msg4;
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const String& msg, const String& msg2, const String& msg3, const String& msg4, const String& msg5, eLogSeverity severity )
{
	std::stringstream ss;
	ss << msg << msg2 << msg3 << msg4 << msg5;
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const String& msg, const int32 num, const int32 num2, eLogSeverity severity /*= LOG_INFO*/ )
{
	std::stringstream ss;
	ss << msg << " (" << num << ";" << num2 << ")";
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const String& msg, const int32 num, const String& msg2, const int32 num2, const String& msg3, eLogSeverity severity /*= LOG_INFO*/ )
{
	std::stringstream ss;
	ss << msg << num << msg2 << num2 << msg3;
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const String& msg, const int32 num, const String& msg2, const String& msg3, eLogSeverity severity /*= LOG_INFO*/ )
{
	std::stringstream ss;
	ss << msg << num << msg2 << msg3;
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const String& msg, const int32 num, const String& msg2, eLogSeverity severity /*= LOG_INFO*/ )
{
	std::stringstream ss;
	ss << msg << num << msg2;
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const String& msg, const String& msg2, const String& msg3, const int32 num, const String& msg4, const int32 num2, const String& msg5, eLogSeverity severity /*= LOG_INFO*/ )
{
	std::stringstream ss;
	ss << msg << msg2 << msg3 << num << msg4 << num2 << msg5;
	LogMessage(ss.str(), severity);
}