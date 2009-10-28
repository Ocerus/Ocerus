/// @file
/// Logging macros and helper classes.

#ifndef Logger_h__
#define Logger_h__

#include "Base.h"

namespace LogSystem
{
	/// This classes makes logging easier. It implements << operators for all data types we could want to use for logging.
	/// An important thing is that the message is actually logged only when destructing instance of this class,
	/// which allows you to write
	/// Logger(level) << value1 << value2;
	/// without the need of writing 'endline' as the last value.
	class Logger
	{
	public:
		
		/// Constructs the logger with a given message logLevel level.
		Logger(int32 logLevel): mLogLevel(logLevel) {}

		/// Destructs the logger and actually writes the logged message into the output.
		~Logger(void);

		/// Templated writable operator << for easier logging.
		template<typename T>
		Logger& operator<<(T value)
		{
			mMessageBuffer << value;
			return *this;
		}

		template<>
		Logger& operator<<(EntitySystem::EntityHandle value)
		{
			*this << "Entity(" << value.GetID() << ")";
			return *this;
		}

		template<>
		Logger& operator<<(Utils::StringKey value)
		{
			*this << value.ToString();
			return *this;
		}


	private:
		int32 mLogLevel;
		stringstream mMessageBuffer;
	};
}


#endif // Logger_h__