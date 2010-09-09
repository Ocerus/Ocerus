#include "Common.h"
#include "Logger.h"
#include "LogMgr.h"

#ifdef USE_DBGLIB

#ifdef __WIN__
#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#endif

#include "EntitySystem/EntityMgr/EntityHandle.h"
#include "GfxSystem/GfxStructures.h"
#include "Properties/PropertyHolder.h"

#include <DbgLib/DbgLib.h>

#endif

using namespace LogSystem;

LogSystem::Logger::~Logger( void )
{
#ifdef USE_DBGLIB
	if (mGenerateStackTrace)
	{
		mMessageBuffer << "\n STACK TRACE:";
		const size_t stackTraceMaxSize = 10;
		string stackTrace[stackTraceMaxSize];
		size_t stackTraceSize = DbgLib::CDebugFx::GenerateStackTrace(stackTrace, stackTraceMaxSize);
		for (DbgLib::uintx i=1; i<stackTraceSize; ++i)
		{
			mMessageBuffer << "\n " << stackTrace[i];
		}
	}
#endif

	string messageString = mMessageBuffer.str();
	if (messageString.size() > 0)
	{
		LogMgr::GetSingleton().LogMessage(messageString, mLogLevel);
	}
}

Logger& LogSystem::Logger::operator<<(const EntitySystem::EntityHandle& value)
{
	if (value.IsValid())
	{
		*this << "Entity(\"" << value.GetName() << "\", " << value.GetID() << ")";
	}
	else
	{
		*this << "Entity(Null, " << EntitySystem::INVALID_ENTITY_ID << ")";
	}
	return *this;
}

Logger& LogSystem::Logger::operator<<(const GfxSystem::Color& value)
{
	*this << "Color(" << (uint32)value.r << ", " << (uint32)value.g << ", " << (uint32)value.b << ", " << (uint32)value.a << ")";
	return *this;

}

Logger& LogSystem::Logger::operator<<(const Reflection::PropertyHolder& value)
{
	*this << "Property(" << value.GetKey().ToString() << ")";
	return *this;

}

Logger& LogSystem::Logger::operator<<(const Utils::StringKey& value)
{
	*this << value.ToString();
	return *this;
}