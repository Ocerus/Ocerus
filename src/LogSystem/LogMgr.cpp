#include "LogMgr.h"
#include "../Core/Application.h"
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace LogSystem;


LogSystem::LogMgr::LogMgr(const string& name, eLogSeverity severityLevel): mSeverityLevel(severityLevel)
{
	mOutStream.open(name.c_str());

	gLogMgr.LogMessage("Log created", LOG_INFO);
}

LogSystem::LogMgr::~LogMgr()
{
	mOutStream.close();
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
		if (severity == LOG_ERROR)
			ss << "!! ERROR !! ";
		ss << std::setw(2) << std::setfill('0') << locTime->tm_hour
			<< ":" << std::setw(2) << std::setfill('0') << locTime->tm_min
			<< ":" << std::setw(2) << std::setfill('0') << locTime->tm_sec
			<< ":" << msg << std::endl;
		string& str = ss.str();

		gApp.WriteToConsole(str);

		mOutStream << str;
		mOutStream.flush();
	}
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

void LogSystem::LogMgr::LogMessage( const string& msg, const int32 num, const int32 num2, eLogSeverity severity /*= LOG_INFO*/ )
{
	std::stringstream ss;
	ss << msg << " (" << num << ";" << num2 << ")";
	LogMessage(ss.str(), severity);
}