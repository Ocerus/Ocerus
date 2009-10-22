/// @file
/// Unit tests entry point.

#include "Common.h"
#include "UnitTests.h"


int main(int argc, char* argv[])
{
	UnitTest::RunAllTests();

#ifdef __WIN__
	getchar();
#endif

	return 0;
};

