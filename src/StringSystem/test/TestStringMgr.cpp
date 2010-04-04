#include "Common.h"
#include "UnitTests.h"
#include "../StringMgr.h"

using namespace StringSystem;

SUITE(StringMgr)
{
	TEST(Init)
	{
		::Test::InitResources();
		StringMgr::CreateSingleton();

		CHECK(StringMgr::SingletonExists());
	}
	
	TEST(LoadDefault)
	{
	  gStringMgr.LoadLanguagePack();
	  CHECK_EQUAL(gStringMgr.GetTextData("default_string"), "Default");
	  CHECK_EQUAL(gStringMgr.GetTextData("language_specific"), "Default");
	  CHECK_EQUAL(gStringMgr.GetTextData("country_specific"), "Default");
	}
	
	TEST(LoadLanguage)
	{
	  gStringMgr.LoadLanguagePack("cs");
	  CHECK_EQUAL(gStringMgr.GetTextData("default_string"), "Default");
	  CHECK_EQUAL(gStringMgr.GetTextData("language_specific"), "Language");
	  CHECK_EQUAL(gStringMgr.GetTextData("country_specific"), "Language");
	}
	
	TEST(LoadCountry)
	{
	  gStringMgr.LoadLanguagePack("cs", "CZ");
	  CHECK_EQUAL(gStringMgr.GetTextData("default_string"), "Default");
	  CHECK_EQUAL(gStringMgr.GetTextData("language_specific"), "Language");
	  CHECK_EQUAL(gStringMgr.GetTextData("country_specific"), "Country");
	}
	
	
	TEST(Clean)
	{
		::Test::CleanSubsystems();
	}
}