#include "Common.h"
#include "EntityDescription.h"

using namespace EntitySystem;

EntityDescription::EntityDescription()
{

}

EntityDescription::~EntityDescription()
{
	Clear();
}

void EntityDescription::Clear(void)
{
	mName = "";
	mComponents.clear();
	mKind = EK_ENTITY;
	mPrototype = INVALID_ENTITY_ID;
	mID = INVALID_ENTITY_ID;
}

void EntityDescription::Init()
{
	Clear();
	mIndex = 0;
}

void EntityDescription::AddComponent(const eComponentType type)
{
	OC_ASSERT_MSG(type != CT_INVALID, "Invalid component description type");
	mComponents.push_back(type);
}
