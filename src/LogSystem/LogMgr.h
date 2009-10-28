/// @file
/// Log system entry point.

#ifndef LogMgr_h__
#define LogMgr_h__

#include "Base.h"
#include "Singleton.h"
#include <boost/filesystem/fstream.hpp>

/// Logging system helps with debugging of the application by allowing programmers to state when certain things happen in a human readable form.
namespace LogSystem
{
	/// This class allows storing arbitrary notes into the logfile for later reviews.
	class LogMgr : public Singleton<LogMgr>
	{
	public:

		LogMgr(void);
		~LogMgr(void);

		///	Initializes the logger.
		/// @param name Name of the log file to be used.
		void Init(const string& name);
	
		/// Saves a note into the log file.
		void LogMessage(const string& msg, int32 loggingLevel);

	private:
		boost::filesystem::ofstream* mOutStream;
	};
}

#endif // LogMgr_h__