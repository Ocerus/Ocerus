#include "Common.h"
#include "EntityDescription.h"

using namespace EntitySystem;

EntityDescription::EntityDescription() {}

EntityDescription::~EntityDescription()
{
	Clear();
}

void EntityDescription::Clear(void)
{
	mComponents.clear();
	mID = "";
}

void EntityDescription::Init(const eEntityType type, const string& ID)
{
	Clear();
	mIndex = 0;
	mType = type;
	mID = ID;
}

void EntityDescription::AddComponent(const eComponentType type)
{
	BS_ASSERT_MSG(type != CT_INVALID, "Invalid component description type");
	mComponents.push_back(type);
}

eComponentType EntityDescription::GetNextComponent()
{
	BS_ASSERT(mIndex<mComponents.size());
	return mComponents[mIndex++];
}