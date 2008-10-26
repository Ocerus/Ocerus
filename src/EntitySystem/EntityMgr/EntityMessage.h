#ifndef _ENTITYMESSAGE_H_
#define _ENTITYMESSAGE_H_

namespace EntitySystem
{

	struct EntityMessage
	{
		enum eType { TYPE_DRAW=0, TYPE_UPDATE_PHYSICS_SERVER, TYPE_UPDATE_PHYSICS_CLIENT, NUM_TYPES };
		enum eResult { RESULT_IGNORED, RESULT_OK, RESULT_ERROR };

		eType type;

		EntityMessage(eType _type): type(_type) {}
	};
}

#endif