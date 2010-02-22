#include "Common.h"
#include "UnitTests.h"
#include "../PropertyMap.h"
#include "../ValuedProperty.h"

SUITE(PropertyMap)
{
	TEST(ConstructorAndDestructor)
	{
		PropertyMap* testMap = new PropertyMap();

		{
			PropertyMap testMap2;
		} // the destructor is called here
		
		delete testMap;
	}


	TEST(AddingCheckingAndDeleting)
	{
		PropertyMap* testMap = new PropertyMap();
		ValuedProperty<int32>* testProperty = new ValuedProperty<int32>("Test", PA_FULL_ACCESS, "Comment");

		CHECK_EQUAL(testMap->HasProperty("Test"), false);
		CHECK_EQUAL(testMap->AddProperty(testProperty), true);
		CHECK_EQUAL(testMap->HasProperty("Test"), true);
		CHECK_EQUAL(testMap->DeleteProperty("Test"), true);

		delete testProperty;
		delete testMap;
	}


	TEST(GettingProperty)
	{
		PropertyMap* testMap = new PropertyMap();
		ValuedProperty<int32>* testProperty = new ValuedProperty<int32>("Test", PA_FULL_ACCESS, "Comment");

		AbstractProperty* getProperty = testMap->GetProperty("Test");
		CHECK(getProperty == 0);
		CHECK_EQUAL(testMap->AddProperty(testProperty), true);
		getProperty = testMap->GetProperty("Test");
		CHECK(getProperty);
		CHECK_EQUAL(getProperty->GetKey(), "Test");
		CHECK_EQUAL(getProperty->GetComment(), "Comment");
		CHECK_EQUAL(testMap->DeleteProperty("Test"), true);

		delete testProperty;
		delete testMap;
	}
}