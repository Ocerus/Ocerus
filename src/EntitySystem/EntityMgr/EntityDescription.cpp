#include "Common.h"
#include "EntityDescription.h"
#include "../ComponentMgr/ComponentDescription.h"

using namespace EntitySystem;

EntityDescription::EntityDescription() {}

EntityDescription::~EntityDescription()
{
	Clear();
}

void EntityDescription::Clear(void)
{
	for (ComponentDescriptionsList::const_iterator iter = mComponentDescriptions.begin(); iter!=mComponentDescriptions.end(); ++iter)
		DYN_DELETE (*iter);
	mComponentDescriptions.clear();
}

void EntityDescription::Init(const eEntityType type)
{
	Clear();
	mIndex = 0;
	mType = type;
}

void EntityDescription::AddComponentDescription(ComponentDescription& desc)
{
	assert(desc.GetType() != CT_INVALID && "Invalid component description type");
	mComponentDescriptions.push_back(DYN_NEW ComponentDescription(desc.GetType(), desc.GetItems()));
	desc.Invalidate();
}

ComponentDescription* EntityDescription::GetNextComponentDescription()
{
	assert(mIndex<mComponentDescriptions.size());
	return mComponentDescriptions[mIndex++];
}