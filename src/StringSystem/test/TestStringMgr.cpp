#include "Common.h"
#include "UnitTests.h"
#include "../StringMgr.h"

using namespace StringSystem;

SUITE(StringMgr)
{
	TEST(Init)
	{
		::Test::InitResources();
		bool* boolPointer = new bool(true);
		Utils::GlobalProperties::SetPointer("DevelopMode", boolPointer);
		StringMgr::Init();

		CHECK(StringMgr::IsInited());
	}
	
	TEST(LoadDefault)
	{
	  gStringMgrSystem.LoadLanguagePack();
	  CHECK_EQUAL(gStringMgrSystem.GetTextData("Test", "default_string"), "Default");
	  CHECK_EQUAL(gStringMgrSystem.GetTextData("Test", "language_specific"), "Default");
	  CHECK_EQUAL(gStringMgrSystem.GetTextData("Test", "country_specific"), "Default");
	}
	
	TEST(LoadLanguage)
	{
	  gStringMgrSystem.LoadLanguagePack("cs");
	  CHECK_EQUAL(gStringMgrSystem.GetTextData("Test", "default_string"), "Default");
	  CHECK_EQUAL(gStringMgrSystem.GetTextData("Test", "language_specific"), "Language");
	  CHECK_EQUAL(gStringMgrSystem.GetTextData("Test", "country_specific"), "Language");
	}
	
	TEST(LoadCountry)
	{
	  gStringMgrSystem.LoadLanguagePack("cs", "CZ");
	  CHECK_EQUAL(gStringMgrSystem.GetTextData("Test", "default_string"), "Default");
	  CHECK_EQUAL(gStringMgrSystem.GetTextData("Test", "language_specific"), "Language");
	  CHECK_EQUAL(gStringMgrSystem.GetTextData("Test", "country_specific"), "Country");
	}
	
	
	TEST(Clean)
	{
		::Test::CleanSubsystems();
	}
}