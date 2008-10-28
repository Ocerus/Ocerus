#include "LogMgr.h"
#include <sstream>
#include <iomanip>

using namespace LogSystem;


LogSystem::LogMgr::LogMgr(const string& name, eLogSeverity severityLevel): mSeverityLevel(severityLevel)
{
	mOutStream.open(name.c_str());
}

LogSystem::LogMgr::~LogMgr()
{
	mOutStream.close();
}

void LogSystem::LogMgr::WriteCurTime(void)
{
	#pragma warning(disable: 4996)
	struct tm* locTime;
	time_t ctime;
	time(&ctime);
	locTime = localtime(&ctime);
	mOutStream << std::setw(2) << std::setfill('0') << locTime->tm_hour
		<< ":" << std::setw(2) << std::setfill('0') << locTime->tm_min
		<< ":" << std::setw(2) << std::setfill('0') << locTime->tm_sec
		<< ":" ;
}

void LogSystem::LogMgr::LogMessage(const string& msg, eLogSeverity severity)
{
	if (severity >= mSeverityLevel)
	{
		WriteCurTime();
		mOutStream << msg << std::endl;
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

