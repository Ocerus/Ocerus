/// @file
/// Unit tests entry point.

#include "Common.h"

#ifdef UNIT_TESTS


int main(int argc, char* argv[])
{
	UnitTest::RunAllTests();

#ifdef __WIN__
	getchar();
#endif
	return 0;
};


#endif // UNIT_TESTS
