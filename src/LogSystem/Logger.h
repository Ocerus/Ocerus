/// @file
/// Logging macros and helper classes.

#ifndef Logger_h__
#define Logger_h__

#include "Base.h"
#include "EntitySystem/EntityMgr/EntityHandle.h"
#include "GfxSystem/GfxStructures.h"
#include "Properties/PropertyHolder.h"

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

		inline Logger& operator<<(const Vector2& value)
		{
			*this << "Vector2(" << value.x << " ; " << value.y << ")";
			return *this;
		}

	    inline Logger& operator<<(const EntitySystem::EntityHandle& value)
	    {
	        *this << "Entity(" << value.GetID() << ")";
		    return *this;
	    }

		inline Logger& operator<<(const GfxSystem::Color& value)
		{
			*this << "Color(" << (uint32)value.r << ", " << (uint32)value.g << ", " << (uint32)value.b << ", " << (uint32)value.a << ")";
			return *this;
		}

		inline Logger& operator<<(const Reflection::PropertyHolder& value)
		{
			*this << "Property(" << value.GetKey().ToString() << ")";
			return *this;
		}

	    inline Logger& operator<<(Utils::StringKey value)
    	{
		    *this << value.ToString();
    		return *this;
    	}

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
