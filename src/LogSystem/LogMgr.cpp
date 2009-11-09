#include "Common.h"
#include "LogMgr.h"
#include "../Core/Application.h"
#include <iomanip>

using namespace LogSystem;


LogSystem::LogMgr::LogMgr( void ):
	mOutStream(0)
{

}

LogSystem::LogMgr::~LogMgr()
{
	if (mOutStream)
	{
		mOutStream->close();
		delete mOutStream;
	}
}


void LogSystem::LogMgr::Init( const string& name )
{
	OC_ASSERT(!mOutStream);
	mOutStream = new boost::filesystem::ofstream();
	mOutStream->open(name.c_str());

	ocInfo << "Log created";
}

void LogSystem::LogMgr::LogMessage(const string& msg, int32 loggingLevel)
{
#ifdef __WIN__
	#pragma warning(disable: 4996)
#endif

	struct tm* locTime;
	time_t ctime;
	time(&ctime);
	locTime = localtime(&ctime);

	stringstream ss;
	ss << std::setw(2) << std::setfill('0') << locTime->tm_hour	<< ":"
	   << std::setw(2) << std::setfill('0') << locTime->tm_min  << ":" 
	   << std::setw(2) << std::setfill('0') << locTime->tm_sec	<< ": ";

	ss << msg << std::endl;

	string str = ss.str();

	gApp.WriteToConsole(str);

	(*mOutStream) << str;
	mOutStream->flush();
}
