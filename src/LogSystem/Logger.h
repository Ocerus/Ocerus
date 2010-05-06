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
		/// Optionally the stack trace can be written to the log.
		Logger(int32 logLevel, const bool generateStackTrace): mLogLevel(logLevel), mGenerateStackTrace(generateStackTrace) {}

		/// Destructs the logger and actually writes the logged message into the output.
		~Logger(void);

		inline Logger& operator<<(bool value)
		{
			*this << (value ? "true" : "false");
			return *this;
		}

		inline Logger& operator<<(const Vector2& value)
		{
			*this << "Vector2(" << value.x << " ; " << value.y << ")";
			return *this;
		}

		Logger& operator<<(const EntitySystem::EntityHandle& value);

		Logger& operator<<(const GfxSystem::Color& value);

		Logger& operator<<(const Reflection::PropertyHolder& value);

		Logger& operator<<(const Utils::StringKey& value);

		/// Templated writable operator << for easier logging.
		template<typename T>
		Logger& operator<<(T value)
		{
			mMessageBuffer << value;
			return *this;
		}

	private:
		int32 mLogLevel;
		bool mGenerateStackTrace;
		stringstream mMessageBuffer;
	};
	
}

#endif // Logger_h__