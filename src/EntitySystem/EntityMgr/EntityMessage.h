#ifndef _ENTITYMESSAGE_H_
#define _ENTITYMESSAGE_H_

namespace EntitySystem
{

	/// Message is the only way how to pass/get some data from/to an entity.
	struct EntityMessage
	{
		enum eResult { RESULT_IGNORED, RESULT_OK, RESULT_ERROR };

		/// Define user types of messages here.
		enum eType { TYPE_DRAW=0, TYPE_UPDATE_PHYSICS_SERVER, TYPE_UPDATE_PHYSICS_CLIENT, NUM_TYPES };
		eType type;

		EntityMessage(eType _type): type(_type) {}
	};
}

#endif