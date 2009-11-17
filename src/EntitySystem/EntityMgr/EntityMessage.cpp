/// @file
/// Definitions of message handle function declaration.

#include "Common.h"
#include "EntityMessage.h"

namespace EntitySystem
{
	const char* const EntityMessage::HandleDeclaration[] =
	{
		#define ENTITY_MESSAGE_TYPE(typeID, handlerDecl) handlerDecl,
		#include "EntityMessageTypes.h"
		#undef ENTITY_MESSAGE_TYPE

		"NUM_TYPES"
	};

	const char* EntityMessage::GetHandleDeclaration(const EntityMessage::eType type)
	{
		return HandleDeclaration[type];
	}
}