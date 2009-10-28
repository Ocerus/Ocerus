/// @file
/// Levels of severity of logged messages.

#ifndef LogMacros_h__
#define LogMacros_h__


/// Setting of the current logging level. Only logging above the level (included) will work.
#define LOGGER_LEVEL 2


#define LL_ERROR 4
#if LOGGER_LEVEL <= LL_ERROR
/// Error messages logging. For enabling/disabling see the LOGGER_LEVEL macro.
#define ocError LogSystem::Logger(LL_ERROR)
#else
#define ocError if(1);else LogSystem::Logger(LL_ERROR)
#endif


#define LL_WARNING 3
#if LOGGER_LEVEL <= LL_WARNING
/// Warning messages logging. For enabling/disabling see the LOGGER_LEVEL macro.
#define ocWarning LogSystem::Logger(LL_WARNING)
#else
#define ocWarning if(1);else LogSystem::Logger(LL_WARNING)
#endif


#define LL_INFO 2
#if LOGGER_LEVEL <= LL_INFO
/// Logging of basic informations about what's going on. For enabling/disabling see the LOGGER_LEVEL macro.
#define ocInfo LogSystem::Logger(LL_INFO)
#else
#define ocInfo if(1);else LogSystem::Logger(LL_INFO)
#endif


#define LL_DEBUG 1
#if LOGGER_LEVEL <= LL_DEBUG
/// Logging of debugging messages. For enabling/disabling see the LOGGER_LEVEL macro.
#define ocDebug LogSystem::Logger(LL_DEBUG)
#else
#define ocDebug if(1);else LogSystem::Logger(LL_DEBUG)
#endif


#define LL_TRACE 0
#if LOGGER_LEVEL <= LL_TRACE
/// Logging of very informative debugging messages. For enabling/disabling see the LOGGER_LEVEL macro.
#define ocTrace LogSystem::Logger(LL_TRACE)
#else
#define ocTrace if(1);else LogSystem::Logger(LL_TRACE)
#endif



#endif // LogMacros_h__
