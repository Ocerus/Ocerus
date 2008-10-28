#ifndef LogMgr_h__
#define LogMgr_h__

#include "../Utility/Singleton.h"
#include "../Utility/Settings.h"
#include <boost/filesystem/fstream.hpp>

#define gLogMgr LogSystem::LogMgr::GetSingleton()

enum eLogSeverity { LOG_TRIVIAL=0, LOG_INFO, LOG_ERROR };

namespace LogSystem
{

	class LogMgr : public Singleton<LogMgr>
	{
	public:
		LogMgr(const string& name, eLogSeverity severityLevel);
		~LogMgr(void);

		void LogMessage(const string& msg, eLogSeverity severity);
		void LogMessage(const string& msg, const int32 num, eLogSeverity severity);
		void LogMessage(const string& msg, const float32 num, eLogSeverity severity);

	private:
		boost::filesystem::ofstream mOutStream;
		eLogSeverity mSeverityLevel;

		void WriteCurTime(void);
	};
}

#endif // LogMgr_h__