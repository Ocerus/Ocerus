/// @file
/// Definitions of message handle function declaration.

#include "Common.h"
#include "EntityMessage.h"

using namespace EntitySystem;

const char* const ResultNames[] =
{
	/// Message was not received by any entity at all.
	"RESULT_IGNORED",
	/// Message was received by some entities and all of them replied OK.
	"RESULT_OK",
	/// Message was received by some entities and at least one of them replied ERROR.
	"RESULT_ERROR"
};

const char* const TypeNames[] =
{
	#define ENTITY_MESSAGE_TYPE(typeID, handlerDecl, parameters) #typeID,
	#include "EntityMessageTypes.h"
	#undef ENTITY_MESSAGE_TYPE

	/// Number of entity message types.
	"NUM_TYPES"
};

const char* const HandlerDeclaration[] =
{
	#define ENTITY_MESSAGE_TYPE(typeID, handlerDecl, parameters) handlerDecl,
	#include "EntityMessageTypes.h"
	#undef ENTITY_MESSAGE_TYPE

	"NUM_TYPES"
};

#define ENTITY_MESSAGE_TYPE(typeID, handlerDecl, parameters) \
	const Reflection::ePropertyType MessageParameterTypes_##typeID[] = parameters;
#include "EntityMessageTypes.h"
#undef ENTITY_MESSAGE_TYPE

const char* EntityMessage::GetHandlerDeclaration(const EntityMessage::eType type)
{
	return HandlerDeclaration[type];
}

const Reflection::ePropertyType* EntitySystem::EntityMessage::GetParameterTypes( const eType messageType )
{
	switch (messageType)
	{
	#define ENTITY_MESSAGE_TYPE(typeID, handlerDecl, parameters) \
		case typeID: return MessageParameterTypes_##typeID;
	#include "EntityMessageTypes.h"
	#undef ENTITY_MESSAGE_TYPE
	case NUM_TYPES: break;
	}

	OC_ASSERT_MSG(false, "Unknown entity message type");

	return 0;
}

uint32 EntitySystem::EntityMessage::GetParametersCount( const eType messageType )
{
	switch (messageType)
	{
	#define ENTITY_MESSAGE_TYPE(typeID, handlerDecl, parameters) \
		case typeID: return (sizeof(MessageParameterTypes_##typeID) / sizeof(ePropertyType)) - 1;
	#include "EntityMessageTypes.h"
	#undef ENTITY_MESSAGE_TYPE
	case NUM_TYPES: break;
	}

	OC_ASSERT_MSG(false, "Unknown entity message type");

	return 0;
}

const char* EntitySystem::EntityMessage::GetTypeName( const eType messageType )
{
	return TypeNames[messageType];
}

const char* EntitySystem::EntityMessage::GetResultName( const eResult messageResult )
{
	return ResultNames[messageResult];
}

bool EntitySystem::EntityMessage::AreParametersValid( void ) const
{
	if (parameters.GetParametersCount() != GetParametersCount(type))
		return false;

	const ePropertyType* desiredTypes = GetParameterTypes(type);
	for (uint32 i=0; i<parameters.GetParametersCount(); ++i)
	{
		if (parameters.GetParameter(i).GetType() != desiredTypes[i])
			return false;
	}

	return true;
}