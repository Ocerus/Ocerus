#ifndef LogMgr_h__
#define LogMgr_h__

#include "../Utility/Singleton.h"
#include "../Utility/Settings.h"
#include <boost/filesystem/fstream.hpp>

/// @name Macro for easier use.
#define gLogMgr LogSystem::LogMgr::GetSingleton()

/// @name Level of severity of a log message. Not all messages will be stored in the log depending on the logger settings.
enum eLogSeverity { LOG_TRIVIAL=0, LOG_INFO, LOG_WARNING, LOG_ERROR };

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
		LogMgr(const String& name, eLogSeverity severityLevel);
		~LogMgr(void);
	
		/// @name These methods can be used for storing messages in the log.
		//@{
		void LogMessage(const String& msg, eLogSeverity severity = LOG_INFO);
		void LogMessage(const String& msg, const String& msg2, eLogSeverity severity = LOG_INFO);
		void LogMessage(const String& msg, const String& msg2, const String& msg3, eLogSeverity severity = LOG_INFO);
		void LogMessage(const String& msg, const String& msg2, const String& msg3, const String& msg4, eLogSeverity severity = LOG_INFO);
		void LogMessage(const String& msg, const String& msg2, const String& msg3, const String& msg4, const String& msg5, eLogSeverity severity = LOG_INFO);
		void LogMessage(const String& msg, const uint64 num, eLogSeverity severity = LOG_INFO);
		void LogMessage(const String& msg, const uint32 num, eLogSeverity severity = LOG_INFO);
		void LogMessage(const String& msg, const int32 num, eLogSeverity severity = LOG_INFO);
		void LogMessage(const String& msg, const int32 num, const int32 num2, eLogSeverity severity = LOG_INFO);
		void LogMessage(const String& msg, const float32 num, eLogSeverity severity = LOG_INFO);
		void LogMessage(const String& msg, const int32 num, const String& msg2, const int32 num2, const String& msg3, eLogSeverity severity = LOG_INFO);
		void LogMessage(const String& msg, const int32 num, const String& msg2, const String& msg3, eLogSeverity severity = LOG_INFO);
		void LogMessage(const String& msg, const int32 num, const String& msg2, eLogSeverity severity = LOG_INFO);
		void LogMessage(const String& msg, const String& msg2, const String& msg3, const int32 num, const String& msg4, const int32 num2, const String& msg5, eLogSeverity severity = LOG_INFO);
		//@}

	private:
		boost::filesystem::ofstream mOutStream;
		eLogSeverity mSeverityLevel;
	};
}

#endif // LogMgr_h__