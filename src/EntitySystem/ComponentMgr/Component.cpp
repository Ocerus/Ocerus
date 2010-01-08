#include "Common.h"
#include "Component.h"

using namespace EntitySystem;

Component::Component() {}

Component::~Component() {}

EntityMessage::eResult Component::HandleMessage(const EntityMessage& msg)
{
	return EntityMessage::RESULT_IGNORED;
}

PropertyHolder Component::GetProperty(const StringKey name, const PropertyAccessFlags mask) const
{
	AbstractProperty* prop = GetRTTI()->GetProperty(name, mask);
	if (prop) return PropertyHolder(const_cast<Component*> (this), prop);
	else return PropertyHolder();
}