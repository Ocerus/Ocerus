#ifndef _ENTITYMESSAGE_H_
#define _ENTITYMESSAGE_H_

namespace EntitySystem
{

	/// Message is the only way how to pass/get some data from/to an entity.
	struct EntityMessage
	{
		enum eResult { RESULT_IGNORED, RESULT_OK, RESULT_ERROR };

		/// Define user types of messages here.
		enum eType
		{ 
			TYPE_DRAW=0,
			TYPE_DRAW_INNER, // used for drawing inner elements of an entity
			TYPE_UPDATE_PHYSICS_SERVER,
			TYPE_UPDATE_PHYSICS_CLIENT,
			TYPE_GET_POSITION,
			TYPE_GET_ANGLE,
			TYPE_GET_POLYSHAPE,
			TYPE_GET_DURABILITY_RATIO,
			TYPE_GET_DENSITY,
			TYPE_GET_MAX_HITPOINTS,
			TYPE_GET_PHYSICS_BODY,
			TYPE_GET_BLUEPRINTS,
			TYPE_GET_MATERIAL,
			TYPE_GET_PARENT,
			TYPE_PLATFORM_DETACH,

			NUM_TYPES
		};
		eType type;
		void* data;

		EntityMessage(eType _type): type(_type), data(0) {}
		EntityMessage(eType _type, void* _data): type(_type), data(_data) {}
	};
}

#endif