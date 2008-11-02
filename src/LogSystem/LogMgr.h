#ifndef LogMgr_h__
#define LogMgr_h__

#include "../Utility/Singleton.h"
#include "../Utility/Settings.h"
#include <boost/filesystem/fstream.hpp>

/// Macro for easier use.
#define gLogMgr LogSystem::LogMgr::GetSingleton()

/// Level of severity of a log message. Not all messages will be stored in the log depending on the logger settings.
enum eLogSeverity { LOG_TRIVIAL=0, LOG_INFO, LOG_ERROR };

namespace LogSystem
{
	/** This class allows storing arbitrary notes into the logfile for later review.
	*/
	class LogMgr : public Singleton<LogMgr>
	{
	public:
		/**	Creates new instance of the logger
		@param severityLevel Messages with lower level will be discarded.
		*/
		LogMgr(const string& name, eLogSeverity severityLevel);
		~LogMgr(void);
	
		/// These methods can be used for storing messages in the log.
		//@{
		void LogMessage(const string& msg, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const string& msg2, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const int32 num, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const int32 num, const int32 num2, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const float32 num, eLogSeverity severity = LOG_INFO);
		//@}

	private:
		boost::filesystem::ofstream mOutStream;
		eLogSeverity mSeverityLevel;
	};
}

#endif // LogMgr_h__