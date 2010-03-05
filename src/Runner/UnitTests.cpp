/// @file
/// Unit tests entry point.

#include "Common.h"
#include "UnitTests.h"


int main(int argc, char* argv[])
{
	OC_UNUSED(argc);
	OC_UNUSED(argv);
	UnitTest::RunAllTests();

#ifdef __WIN__
	getchar();
#endif

	return 0;
}