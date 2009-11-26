/// @file
/// Definitions of message handle function declaration.

#include "Common.h"
#include "EntityMessage.h"

namespace EntitySystem
{
	const char* const EntityMessage::ResultNames[] =
	{
		/// Message was not received by any entity at all.
		"RESULT_IGNORED",
		/// Message was received by some entities and all of them replied OK.
		"RESULT_OK",
		/// Message was received by some entities and at least one of them replied ERROR.
		"RESULT_ERROR"
	};
	
	const char* const EntityMessage::TypeNames[] =
	{
		#define ENTITY_MESSAGE_TYPE(typeID, handlerDecl) #typeID,
		#include "EntityMessageTypes.h"
		#undef ENTITY_MESSAGE_TYPE

		/// Number of entity message types.
		"NUM_TYPES"
	};
	
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