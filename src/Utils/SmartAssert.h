/// @file Smart assert macro implementation.
/// Note that it is also used by Box2d (b2Settings.h) and Boost (assert.hpp) and Angelscript (as_config.h).

#ifndef SmartAssert_h__
#define SmartAssert_h__

#include "../Setup/Platform.h"

#undef assert

/// Displays an error assertion message and terminates the program.
void DisplayAssert(const char* msg, const char* file, const int line);

#ifdef __WIN__
/// Breaks the execution at this line.
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK() asm("int $3")
#endif

/// Makes the expression used, but without any effect. Useful for removing compiler warnings.
#define EXPRESSION_COMPILED_OUT(expr) (void)sizeof(reinterpret_cast<const void*>(expr))
/// Asserts nothing.
#define OC_DISABLED_ASSERT(expr) do { EXPRESSION_COMPILED_OUT(expr); } while(0)
/// Asserts nothing.
#define OC_DISABLED_ASSERT_MSG(expr, msg) do { EXPRESSION_COMPILED_OUT(expr); EXPRESSION_COMPILED_OUT(msg); } while(0)
/// Terminates the execution with a failed message.
#define OC_FAIL(msg) { DisplayAssert((msg), __FILE__, __LINE__); DEBUG_BREAK(); }



#ifdef USE_ASSERT

#define OC_ASSERT(expr)	do { if (!(expr)) OC_FAIL(#expr); } while (0)
#define OC_ASSERT_MSG(expr, msg) do { if (!(expr)) OC_FAIL(msg); } while (0)

#ifdef _DEBUG
	#define OC_DASSERT(expr) OC_ASSERT(expr)
	#define OC_DASSERT_MSG(expr, msg) OC_ASSERT_MSG(expr, msg)
#else // _DEBUG
	#define OC_DASSERT(expr) OC_DISABLED_ASSERT(expr)
	#define OC_DASSERT_MSG(expr, msg) OC_DISABLED_ASSERT_MSG(expr, msg)
#endif // _DEBUG


#else // USE_ASSERT


#define OC_ASSERT(expr) OC_DISABLED_ASSERT(expr)
#define OC_ASSERT_MSG(expr, msg) OC_DISABLED_ASSERT_MSG(expr, msg)
#define OC_DASSERT(expr) OC_DISABLED_ASSERT(expr)
#define OC_DASSERT_MSG(expr, msg) OC_DISABLED_ASSERT_MSG(expr, msg)


#endif // USE_ASSERT



#define OC_NOT_REACHED() OC_ASSERT_MSG(0, "NOT_REACHED")
#define OC_SCRIPT_ASSERT() OC_ASSERT_MSG(r >= 0, "Something cannot be registered to the script engine.")


/// Override Boost's assert. The override function is in SmartAssert.cpp
#define BOOST_ENABLE_ASSERT_HANDLER


#endif // SmartAssert_h__

