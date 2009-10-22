
/// @name Smart assert macro implementation.
/// Note that it is also used by Box2d (b2Settings.h) and Boost (assert.hpp).
//@{
#undef assert
#ifdef USE_ASSERT

void DisplayAssert(const char* msg, const char* file, const int line);

#define OC_ASSERT(expr) \
	__pragma(warning(push)) \
	__pragma(warning(disable:4127)) \
	if (!(expr)) { DisplayAssert(#expr, __FILE__, __LINE__); _asm { int 3 } } \
	__pragma(warning(pop))

#define OC_ASSERT_MSG(expr, msg) \
	__pragma(warning(push)) \
	__pragma(warning(disable:4127)) \
	if (!(expr)) { DisplayAssert(msg, __FILE__, __LINE__); _asm { int 3 } } \
	__pragma(warning(pop))

#ifdef _DEBUG

#define OC_DASSERT(expr) OC_ASSERT(expr)
#define OC_DASSERT_MSG(expr, msg) OC_ASSERT_MSG(expr, msg)

#else

#define OC_DASSERT(expr) ((void)0)
#define OC_DASSERT_MSG(expr, msg) ((void)0)

#endif

#else

#define OC_ASSERT(expr) ((void)0)
#define OC_ASSERT_MSG(expr, msg) ((void)0)
#define OC_DASSERT(expr) ((void)0)
#define OC_DASSERT_MSG(expr, msg) ((void)0)

#endif

#define OC_NOT_REACHED() OC_ASSERT_MSG(0, "NOT_REACHED")
//@}
