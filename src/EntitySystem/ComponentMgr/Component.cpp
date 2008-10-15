#include "Component.h"

using namespace EntitySystem;

Component::Component() {}

Component::~Component() {}

eEntityMessageResult Component::HandleMessage(const EntityMessage& msg)
{
	return EMR_IGNORED;
}