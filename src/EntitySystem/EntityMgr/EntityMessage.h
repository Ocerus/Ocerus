/// @file
/// Defines messages which can be send to entities.

#ifndef _ENTITYMESSAGE_H_
#define _ENTITYMESSAGE_H_

namespace EntitySystem
{
	/// @brief Defines messages which can be send to entities.
	/// @remarks
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
			TYPE_DRAW_PLATFORM=0,
			TYPE_DRAW_PLATFORM_LINK,
			TYPE_DRAW_PLATFORM_ITEM,
			TYPE_DRAW_PROJECTILE,
			TYPE_DRAW_UNDERLAY,
			TYPE_DRAW_OVERLAY,
			TYPE_UPDATE_PHYSICS_SERVER,
			TYPE_UPDATE_PHYSICS_CLIENT,
			TYPE_GET_POSITION,
			TYPE_GET_BODY_POSITION,
			TYPE_GET_RELATIVE_POSITION,
			TYPE_GET_ANGLE,
			TYPE_GET_RELATIVE_ANGLE,
			TYPE_GET_ARC_ANGLE,
			TYPE_GET_POLYSHAPE,
			TYPE_GET_DURABILITY_RATIO,
			TYPE_GET_DENSITY,
			TYPE_GET_MAX_HITPOINTS,
			TYPE_GET_HITPOINTS,
			TYPE_GET_MAX_POWER,
			TYPE_GET_POWER_RATIO,
			TYPE_GET_PHYSICS_BODY,
			TYPE_GET_BLUEPRINTS,
			TYPE_GET_MATERIAL,
			TYPE_GET_PARENT,
			TYPE_GET_STABILIZATION_RATIO,
			TYPE_GET_TEXTURE,
			TYPE_GET_TEXTURE_ANGLE,
			TYPE_GET_TEXTURE_SCALE,
			TYPE_GET_EFFECT,
			TYPE_GET_EFFECT_SCALE,
			TYPE_GET_RESOURCE_GROUP,
			TYPE_SET_ANGLE,
			TYPE_SET_RELATIVE_ANGLE,
			TYPE_SET_POWER_RATIO,
			TYPE_ADD_PLATFORM,
			TYPE_REMOVE_PLATFORM,
			TYPE_DETACH_PLATFORM,
			TYPE_ADD_PLATFORM_ITEM,
			TYPE_MOUSE_PICK,
			TYPE_LINK_PLATFORMS,
			TYPE_LINKS_PLATFORM, // question
			TYPE_UNLINK_PLATFORM,
			TYPE_PHYSICS_UPDATE_MASS,
			TYPE_START_SHOOTING,
			TYPE_STOP_SHOOTING,
			TYPE_STRIKE,
			TYPE_DAMAGE,
			TYPE_KNOCKBACK_DETACH,
			TYPE_EXPLOSION,
			TYPE_UPDATE_TEAM,
			TYPE_DIE,
			TYPE_POST_INIT,

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