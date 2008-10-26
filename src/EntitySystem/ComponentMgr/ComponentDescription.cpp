#include "ComponentDescription.h"

using namespace EntitySystem;

ComponentDescriptionItem::ComponentDescriptionItem(): mIsDefined(false), mData(0)
{
}

ComponentDescriptionItem::ComponentDescriptionItem(const int8 data): mData(0), mIsDefined(true)
{
	mData = DYN_NEW int8(data);
	mType = TYPE_INT8;
}

int8 ComponentDescriptionItem::GetInt8() const
{
	assert(mType == TYPE_INT8 && "Wrong component item type.");
	return *static_cast<int8*>(mData);
}

ComponentDescriptionItem::ComponentDescriptionItem(const int16 data): mData(0), mIsDefined(true)
{
	mData = DYN_NEW int16(data);
	mType = TYPE_INT16;
}

int16 ComponentDescriptionItem::GetInt16() const
{
	assert(mType == TYPE_INT16 && "Wrong component item type.");
	return *static_cast<int16*>(mData);
}

ComponentDescriptionItem::ComponentDescriptionItem(const int32 data): mData(0), mIsDefined(true)
{
	mData = DYN_NEW int32(data);
	mType = TYPE_INT32;
}

int32 ComponentDescriptionItem::GetInt32() const
{
	assert(mType == TYPE_INT32 && "Wrong component item type.");
	return *static_cast<int32*>(mData);
}

ComponentDescriptionItem::ComponentDescriptionItem(const string& data): mData(0), mIsDefined(true)
{
	mData = DYN_NEW string(data);
	mType = TYPE_STRING;
}

string& ComponentDescriptionItem::GetString() const
{
	assert(mType == TYPE_STRING && "Wrong component item type.");
	return *static_cast<string*>(mData);
}

ComponentDescriptionItem::~ComponentDescriptionItem()
{
	if (mData)
		DYN_DELETE mData;
}



ComponentDescription::ComponentDescription(eComponentType type): mType(type), mIndex(0) {}

ComponentDescription::~ComponentDescription() {}

void ComponentDescription::AddItem(ComponentDescriptionItem* item)
{
	mItems.push_back(item);
}

ComponentDescriptionItem* ComponentDescription::GetNextItem()
{
	assert(mIndex<mItems.size());
	return mItems[mIndex++];
}