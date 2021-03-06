#include "Common.h"
#include "EntityDescription.h"

using namespace EntitySystem;

EntityDescription::EntityDescription()
{
	Reset();
}

EntityDescription::~EntityDescription()
{

}

void EntityDescription::Reset(void)
{
	mName = "";
	mComponents.clear();
	mKind = EK_ENTITY;
	mPrototype = INVALID_ENTITY_ID;
	mID = INVALID_ENTITY_ID;
	mTransient = false;
}

void EntityDescription::AddComponent(const eComponentType type)
{
	OC_ASSERT_MSG(type != CT_INVALID, "Invalid component description type");
	mComponents.push_back(type);
}

void EntitySystem::EntityDescription::CopyComponents( const EntityHandle source )
{
	ComponentTypeList componentTypes;
	gEntityMgr.GetEntityComponentTypes(source, componentTypes);
	for (ComponentTypeList::iterator it=componentTypes.begin(); it!=componentTypes.end(); ++it)
	{
		AddComponent(*it);
	}
}