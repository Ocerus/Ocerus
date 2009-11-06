
/// @name Smart assert macro implementation.
/// Note that it is also used by Box2d (b2Settings.h) and Boost (assert.hpp).
//@{

#undef assert

#ifdef USE_ASSERT


void DisplayAssert(const char* msg, const char* file, const int line);

#ifdef __WIN__

#define OC_ASSERT(expr) \
	do { if (!(expr)) { DisplayAssert(#expr, __FILE__, __LINE__); __debugbreak(); } } while (0)

#define OC_ASSERT_MSG(expr, msg) \
	do { if (!(expr)) { DisplayAssert(msg, __FILE__, __LINE__); __debugbreak(); } } while (0)

#else // __WIN__

#define OC_ASSERT(expr) \
	do { if (!(expr)) { DisplayAssert(#expr, __FILE__, __LINE__); } } while (0)

#define OC_ASSERT_MSG(expr, msg) \
	do { if (!(expr)) { DisplayAssert(msg, __FILE__, __LINE__); } } while (0)

#endif


#ifdef _DEBUG

#define OC_DASSERT(expr) OC_ASSERT(expr)
#define OC_DASSERT_MSG(expr, msg) OC_ASSERT_MSG(expr, msg)

#else // _DEBUG

#define OC_DASSERT(expr) do { (void)sizeof(expr); } while(0)
#define OC_DASSERT_MSG(expr, msg) do { (void)sizeof(expr); (void)sizeof(msg); } while(0)

#endif // _DEBUG


#else // USE_ASSERT


#define OC_ASSERT(expr) do { (void)sizeof(expr); } while(0)
#define OC_ASSERT_MSG(expr, msg) do { (void)sizeof(expr); (void)sizeof(msg); } while(0)
#define OC_DASSERT(expr) do { (void)sizeof(expr); } while(0)
#define OC_DASSERT_MSG(expr, msg) do { (void)sizeof(expr); (void)sizeof(msg); } while(0)


#endif // USE_ASSERT


#define OC_NOT_REACHED() OC_ASSERT_MSG(0, "NOT_REACHED")

//@}


/// Override Boost's assert. The override function is in SmartAssert.cpp
#define BOOST_ENABLE_ASSERT_HANDLER


