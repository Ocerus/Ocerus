/// @file
/// Defines messages which can be send to entities.

#ifndef _ENTITYMESSAGE_H_
#define _ENTITYMESSAGE_H_

#include <Properties/PropertyFunctionParameters.h>

namespace EntitySystem
{
	/// Defines messages which can be send to entities.
	/// After the message is sent to an entity, it will go through all of its components and hope one (or more)
	/// of them picks it up.
	struct EntityMessage
	{
		/// Result the EntityMgr can receive after sending out a message to entities.
		enum eResult
		{
			/// Message was not received by any entity at all.
			RESULT_IGNORED,
			/// Message was received by some entities and all of them replied OK.
			RESULT_OK,
			/// Message was received by some entities and at least one of them replied ERROR.
			RESULT_ERROR
		};

		/// Defines user types of messages.
		enum eType
		{
			#define ENTITY_MESSAGE_TYPE(typeID, handlerDecl) typeID,
			#include "EntityMessageTypes.h"
			#undef ENTITY_MESSAGE_TYPE

			/// Number of entity message types.
			NUM_TYPES
		};

		/// String names of property types.
		static const char* const HandleDeclaration[];

		/// Returns the string name of this property type.
		static const char* GetHandleDeclaration(const eType type);

		/// User type of this message.
		eType type;

		/// Data carried by this message.
		Reflection::PropertyFunctionParameters data;

		EntityMessage(eType _type): type(_type) {}
		EntityMessage(eType _type, Reflection::PropertyFunctionParameters _data): type(_type), data(_data) {}
	};
}

#endif