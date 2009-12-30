/// @file
/// Defines messages which can be send to entities.

#ifndef _ENTITYMESSAGE_H_
#define _ENTITYMESSAGE_H_

#include <Properties/PropertyFunctionParameters.h>

namespace EntitySystem
{
	/// Defines a message which can be sent to entities.
	/// After the message is sent to an entity, it will go through all of its components and hope one (or more)
	/// of them picks it up. If not, RESULT_IGNORED is returned by the operation. The message can carry parameters using
	/// the Reflection::PropertyFunctionParameters class. The run-time type check on the parameters is invoked before the
	/// message is sent.
	struct EntityMessage
	{
	public:

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

		/// String name of an entity message results.
		static const char* GetResultName(const eResult messageResult);

		/// Defines user types of messages.
		enum eType
		{
			#define ENTITY_MESSAGE_TYPE(typeID, handlerDecl, params) typeID,
			#include "EntityMessageTypes.h"
			#undef ENTITY_MESSAGE_TYPE

			/// Number of entity message types.
			NUM_TYPES
		};

		/// Returns the string name of a message type.
		static const char* GetTypeName(const eType messageType);

		/// Returns the handle declaration of all entity message types.
		static const char* GetHandlerDeclaration(const eType messageType);

		/// Returns the array of property types the given message type accepts.
		static const Reflection::ePropertyType* GetParameterTypes(const eType messageType);

		/// Returns the number of arguments the given message type accepts.
		static const uint32 GetParametersCount(const eType messageType);

		/// User type of this message.
		eType type;

		/// Data carried by this message.
		Reflection::PropertyFunctionParameters parameters;

		/// Constructs a new message with no parameters.
		EntityMessage(eType _type): type(_type) {}

		/// Constructs a new message with given parameters.
		EntityMessage(eType _type, Reflection::PropertyFunctionParameters _data): type(_type), parameters(_data) {}

		/// Returns true if the parameters of the message match the definition of its type (see EntityMessageTypes.h).
		bool AreParametersValid(void) const;

	};
}

#endif