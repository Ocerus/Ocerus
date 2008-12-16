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
}

void EntityDescription::Init(const eEntityType type)
{
	Clear();
	mIndex = 0;
	mType = type;
}

void EntityDescription::AddComponent(const eComponentType type)
{
	assert(type != CT_INVALID && "Invalid component description type");
	mComponents.push_back(type);
}

eComponentType EntityDescription::GetNextComponent()
{
	assert(mIndex<mComponents.size());
	return mComponents[mIndex++];
}