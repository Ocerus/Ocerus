#include "Common.h"
#include "ComponentDescription.h"

using namespace EntitySystem;

ComponentDescriptionItem::ComponentDescriptionItem(const int8 data): mData(0)
{
	mData = DYN_NEW int8(data);
	mType = TYPE_INT8;
}

int8 ComponentDescriptionItem::GetInt8() const
{
	assert(mType == TYPE_INT8 && "Wrong component item type.");
	return *static_cast<int8*>(mData);
}

ComponentDescriptionItem::ComponentDescriptionItem(const int16 data): mData(0)
{
	mData = DYN_NEW int16(data);
	mType = TYPE_INT16;
}

int16 ComponentDescriptionItem::GetInt16() const
{
	assert(mType == TYPE_INT16 && "Wrong component item type.");
	return *static_cast<int16*>(mData);
}

ComponentDescriptionItem::ComponentDescriptionItem(const int32 data): mData(0)
{
	mData = DYN_NEW int32(data);
	mType = TYPE_INT32;
}

int32 ComponentDescriptionItem::GetInt32() const
{
	assert(mType == TYPE_INT32 && "Wrong component item type.");
	return *static_cast<int32*>(mData);
}

ComponentDescriptionItem::ComponentDescriptionItem(const int64 data): mData(0)
{
	mData = DYN_NEW int64(data);
	mType = TYPE_INT64;
}

int64 ComponentDescriptionItem::GetInt64() const
{
	assert(mType == TYPE_INT64 && "Wrong component item type.");
	return *static_cast<int64*>(mData);
}

//----------------------------

ComponentDescriptionItem::ComponentDescriptionItem(const uint8 data): mData(0)
{
	mData = DYN_NEW uint8(data);
	mType = TYPE_UINT8;
}

uint8 ComponentDescriptionItem::GetUint8() const
{
	assert(mType == TYPE_UINT8 && "Wrong component item type.");
	return *static_cast<uint8*>(mData);
}

ComponentDescriptionItem::ComponentDescriptionItem(const uint16 data): mData(0)
{
	mData = DYN_NEW uint16(data);
	mType = TYPE_UINT16;
}

uint16 ComponentDescriptionItem::GetUint16() const
{
	assert(mType == TYPE_UINT16 && "Wrong component item type.");
	return *static_cast<uint16*>(mData);
}

ComponentDescriptionItem::ComponentDescriptionItem(const uint32 data): mData(0)
{
	mData = DYN_NEW uint32(data);
	mType = TYPE_UINT32;
}

uint32 ComponentDescriptionItem::GetUint32() const
{
	assert(mType == TYPE_UINT32 && "Wrong component item type.");
	return *static_cast<uint32*>(mData);
}

ComponentDescriptionItem::ComponentDescriptionItem(const uint64 data): mData(0)
{
	mData = DYN_NEW int64(data);
	mType = TYPE_UINT64;
}

uint64 ComponentDescriptionItem::GetUint64() const
{
	assert(mType == TYPE_UINT64 && "Wrong component item type.");
	return *static_cast<uint64*>(mData);
}

//--------------------------------

ComponentDescriptionItem::ComponentDescriptionItem(const float32 data): mData(0)
{
	mData = DYN_NEW float32(data);
	mType = TYPE_FLOAT32;
}

float32 ComponentDescriptionItem::GetFloat32() const
{
	assert(mType == TYPE_FLOAT32 && "Wrong component item type.");
	return *static_cast<float32*>(mData);
}

ComponentDescriptionItem::ComponentDescriptionItem(const EntityHandle data): mData(0)
{
	mData = DYN_NEW EntityHandle(data);
	mType = TYPE_ENTITYHANDLE;
}

EntityHandle ComponentDescriptionItem::GetEntityHandle() const
{
	assert(mType == TYPE_ENTITYHANDLE && "Wrong component item type.");
	return *static_cast<EntityHandle*>(mData);
}

ComponentDescriptionItem::ComponentDescriptionItem(const string& data): mData(0)
{
	mData = DYN_NEW string(data);
	mType = TYPE_STRING;
}

string ComponentDescriptionItem::GetString() const
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

ComponentDescription::~ComponentDescription()
{
	for (DescriptionItems::const_iterator i=mItems.begin(); i!=mItems.end(); ++i)
		DYN_DELETE (*i);
}

void ComponentDescription::AddItem(ComponentDescriptionItem* item)
{
	mItems.push_back(item);
}

ComponentDescriptionItem* ComponentDescription::GetNextItem()
{
	assert(mIndex<mItems.size());
	return mItems[mIndex++];
}

