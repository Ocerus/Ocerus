#include "EntityDescription.h"

using namespace EntitySystem;

EntityDescription::EntityDescription(): mIndex(0) {}

EntityDescription::~EntityDescription()
{
	for (ComponentDescriptionsList::const_iterator iter = mComponentDescriptions.begin(); iter!=mComponentDescriptions.end(); ++iter)
		DYN_DELETE (*iter);
}

void EntityDescription::AddComponentDescription(ComponentDescription* desc)
{
	mComponentDescriptions.push_back(desc);
}

ComponentDescription* EntityDescription::GetNextComponentDescription()
{
	assert(mIndex<mComponentDescriptions.size());
	return mComponentDescriptions[mIndex++];
}