#include "Component.h"

using namespace EntitySystem;

Component::Component() {}

Component::~Component() {}

EntityMessage::eResult Component::HandleMessage(const EntityMessage& msg)
{
	return EntityMessage::RESULT_IGNORED;
}