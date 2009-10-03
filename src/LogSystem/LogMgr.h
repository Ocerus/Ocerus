/// @file
/// Log system entry point.

#ifndef LogMgr_h__
#define LogMgr_h__

#include "Base.h"
#include "Singleton.h"
//TODO tohle pryc nejak
#include <boost/filesystem/fstream.hpp>

/// Macro for easier use.
#define gLogMgr LogSystem::LogMgr::GetSingleton()

/// Level of severity of a log message. Not all messages will be stored in the log depending on the logger settings.
enum eLogSeverity { LOG_TRIVIAL=0, LOG_INFO, LOG_WARNING, LOG_ERROR };

/// Logging system helps with debugging of the application by allowing programmers to state when certain things happen in a human readable form.
namespace LogSystem
{
	/// This class allows storing arbitrary notes into the logfile for later reviews.
	class LogMgr : public Singleton<LogMgr>
	{
	public:

		///	@brief Creates a new instance of the logger.
		/// @param name Name of the log file to be used.
		/// @param severityLevel Messages with lower level will be discarded.
		LogMgr(const string& name, eLogSeverity severityLevel);
		~LogMgr(void);
	
		//@{
		/// Saves a note into the log file.
		void LogMessage(const string& msg, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const string& msg2, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const string& msg2, const string& msg3, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const string& msg2, const string& msg3, const string& msg4, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const string& msg2, const string& msg3, const string& msg4, const string& msg5, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const uint64 num, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const uint32 num, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const int32 num, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const int32 num, const int32 num2, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const float32 num, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const int32 num, const string& msg2, const int32 num2, const string& msg3, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const int32 num, const string& msg2, const string& msg3, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const int32 num, const string& msg2, eLogSeverity severity = LOG_INFO);
		void LogMessage(const string& msg, const string& msg2, const string& msg3, const int32 num, const string& msg4, const int32 num2, const string& msg5, eLogSeverity severity = LOG_INFO);
		//@}

	private:

		boost::filesystem::ofstream mOutStream;
		eLogSeverity mSeverityLevel;
	};
}

#endif // LogMgr_h__