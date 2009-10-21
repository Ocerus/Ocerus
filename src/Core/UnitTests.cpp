/// @file
/// Unit tests entry point.

#include "Common.h"

#ifdef UNIT_TESTS


int main(int argc, char* argv[])
{	
	UnitTest::RunAllTests();
	getchar();

	return 0;
};


#endif // UNIT_TESTS
