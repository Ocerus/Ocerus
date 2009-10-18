/// @file
/// Defines messages which can be send to entities.

#ifndef _ENTITYMESSAGE_H_
#define _ENTITYMESSAGE_H_

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
			/// Sent shortly after the entity is created and all initial properties are set.
			INIT=0,
			/// Sent shortly after the INIT message is sent. This allows initialization of components which need other
			/// components to be inited first.
			POST_INIT,
			/// Sent shortly before the entity is destroyed.
			DESTROY,
			/// Sent when the entity is to be redrawn.
			DRAW,
			/// Sent when the underlay under the entity is to be redrawn.
			DRAW_UNDERLAY,
			/// Sent when the overlay above the entity is to be redrawn.
			DRAW_OVERLAY,
			/// Sent when the selected entity is to be redrawn.
			DRAW_SELECTED,
			/// Sent when the logic of the entity is to be updated.
			UPDATE_LOGIC,
			/// Sent when the physics of the entity is to be updated.
			UPDATE_PHYSICS,
			/// Sent when the mouse attempts to pick the entity.
			MOUSE_PICK,

			/// Number of entity message types.
			NUM_TYPES
		};

		/// User type of this message.
		eType type;

		/// Data carried by this message.
		void* data;

		EntityMessage(eType _type): type(_type), data(0) {}
		EntityMessage(eType _type, void* _data): type(_type), data(_data) {}
	};
}

#endif