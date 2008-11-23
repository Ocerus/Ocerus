#include "Common.h"
#include "ComponentDescription.h"
#include "ComponentDescriptionItem.h"

using namespace EntitySystem;

ComponentDescription::ComponentDescription() 
{
	Invalidate();
}

ComponentDescription::ComponentDescription(eComponentType type, DescriptionItems* items):
	mType(type),
	mItems(items),
	mIndex(0)
{
}

void ComponentDescription::Init(eComponentType type)
{
	Clear();
	mType = type;
	mItems = DYN_NEW DescriptionItems();
	mIndex = 0;
}

void ComponentDescription::Clear(void)
{
	if (mItems)
	{
		for (DescriptionItems::const_iterator i=mItems->begin(); i!=mItems->end(); ++i)
			DYN_DELETE (*i);
		DYN_DELETE mItems;
	}
}

ComponentDescription::~ComponentDescription()
{
	Clear();
}

void ComponentDescription::Invalidate()
{
	mType = CT_INVALID;
	mItems = 0;
}

ComponentDescriptionItem* ComponentDescription::GetNextItem()
{
	assert(mIndex<mItems->size());
	return mItems->at(mIndex++);
}

