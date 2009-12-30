/// @file
/// Log system entry point.

#ifndef LogMgr_h__
#define LogMgr_h__

#include "Base.h"
#include "Singleton.h"
#include <boost/filesystem/fstream.hpp>

/// Logging system helps with debugging of the application by allowing programmers to save info about what's going on.
namespace LogSystem
{
	/// This class allows you to store arbitrary notes into the logfile for later reviews.
	/// Note that it's not wise to use this class directly, but the macros inside LogMacros.h instead.
	class LogMgr : public Singleton<LogMgr>
	{
	public:

		/// Constructor.
		LogMgr(void);

		/// Destructor.
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