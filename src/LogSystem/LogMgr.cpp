#include "Common.h"
#include "LogMgr.h"
#include "../Core/Application.h"
//TODO std pryc!
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace LogSystem;


LogSystem::LogMgr::LogMgr( void )
{

}

LogSystem::LogMgr::~LogMgr()
{
	mOutStream.close();
}


void LogSystem::LogMgr::Init( const string& name, eLogSeverity severityLevel )
{
	mSeverityLevel = severityLevel;

	mOutStream.open(name.c_str());

	gLogMgr.LogMessage("Log created", LOG_INFO);
}

void LogSystem::LogMgr::LogMessage(const string& msg, eLogSeverity severity)
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
		string& str = ss.str();

		gApp.WriteToConsole(str);
		/*if (GUISystem::GUIMgr::GetSingletonPtr())
			gGUIMgr.AddConsoleMessage(str);*/

		mOutStream << str;
		mOutStream.flush();
	}
}

void LogSystem::LogMgr::LogMessage(const string& msg, const uint32 num, eLogSeverity severity)
{
	std::stringstream ss;
	ss << msg << " (" << num << ")";
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage(const string& msg, const uint64 num, eLogSeverity severity)
{
	std::stringstream ss;
	ss << msg << " (" << num << ")";
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage(const string& msg, const int32 num, eLogSeverity severity)
{
	std::stringstream ss;
	ss << msg << " (" << num << ")";
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const string& msg, const float32 num, eLogSeverity severity )
{
	std::stringstream ss;
	ss << msg << " (" << num << ")";
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const string& msg, const string& msg2, eLogSeverity severity )
{
	std::stringstream ss;
	ss << msg << msg2;
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const string& msg, const string& msg2, const string& msg3, eLogSeverity severity )
{
	std::stringstream ss;
	ss << msg << msg2 << msg3;
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const string& msg, const string& msg2, const string& msg3, const string& msg4, eLogSeverity severity )
{
	std::stringstream ss;
	ss << msg << msg2 << msg3 << msg4;
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const string& msg, const string& msg2, const string& msg3, const string& msg4, const string& msg5, eLogSeverity severity )
{
	std::stringstream ss;
	ss << msg << msg2 << msg3 << msg4 << msg5;
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const string& msg, const int32 num, const int32 num2, eLogSeverity severity /*= LOG_INFO*/ )
{
	std::stringstream ss;
	ss << msg << " (" << num << ";" << num2 << ")";
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const string& msg, const int32 num, const string& msg2, const int32 num2, const string& msg3, eLogSeverity severity /*= LOG_INFO*/ )
{
	std::stringstream ss;
	ss << msg << num << msg2 << num2 << msg3;
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const string& msg, const int32 num, const string& msg2, const string& msg3, eLogSeverity severity /*= LOG_INFO*/ )
{
	std::stringstream ss;
	ss << msg << num << msg2 << msg3;
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const string& msg, const int32 num, const string& msg2, eLogSeverity severity /*= LOG_INFO*/ )
{
	std::stringstream ss;
	ss << msg << num << msg2;
	LogMessage(ss.str(), severity);
}

void LogSystem::LogMgr::LogMessage( const string& msg, const string& msg2, const string& msg3, const int32 num, const string& msg4, const int32 num2, const string& msg5, eLogSeverity severity /*= LOG_INFO*/ )
{
	std::stringstream ss;
	ss << msg << msg2 << msg3 << num << msg4 << num2 << msg5;
	LogMessage(ss.str(), severity);
}
