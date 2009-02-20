
/// @name Smart assert macro implementation.
/// Note that it is also used by Box2d (b2Settings.h) and Boost (assert.hpp).
//@{
#undef assert
#ifdef USE_ASSERT

void DisplayAssert(const char* msg, const char* file, const int line);

#define ASSERT(expr) \
	__pragma(warning(push)) \
	__pragma(warning(disable:4127)) \
	if (!(expr)) { DisplayAssert(#expr, __FILE__, __LINE__); _asm { int 3 } } \
	__pragma(warning(pop))

#define ASSERT_MSG(expr, msg) \
	__pragma(warning(push)) \
	__pragma(warning(disable:4127)) \
	if (!(expr)) { DisplayAssert(msg, __FILE__, __LINE__); _asm { int 3 } } \
	__pragma(warning(pop))

#ifdef _DEBUG

#define DASSERT(expr) ASSERT(expr)
#define DASSERT_MSG(expr, msg) ASSERT_MSG(expr, msg)

#else

#define DASSERT(expr) ((void)0)
#define DASSERT_MSG(expr, msg) ((void)0)

#endif

#else

#define ASSERT(expr) ((void)0)
#define ASSERT_MSG(expr, msg) ((void)0)
#define DASSERT(expr) ((void)0)
#define DASSERT_MSG(expr, msg) ((void)0)

#endif
//@}