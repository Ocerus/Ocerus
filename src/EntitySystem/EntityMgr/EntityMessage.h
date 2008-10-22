#ifndef _ENTITYMESSAGE_H_
#define _ENTITYMESSAGE_H_

namespace EntitySystem
{

	enum eEntityMessageResult { EMR_IGNORED, EMR_OK, EMR_ERROR };

	struct EntityMessage
	{
		enum eType { TYPE_DRAW=0, NUM_TYPES };
		eType type;

		EntityMessage(eType _type): type(_type) {}
	};
}

#endif