#ifndef _AIMGR_H_
#define _AIMGR_H_

#include "BasicTypes.h"
#include "GenericAI.h"

#define gAIMgr AISystem::AIMgr::GetSingleton()

namespace AISystem {
	const uint32 skip_frames = 6;

	class AIMgr : public Singleton<AIMgr> {
	protected:
		Vector<GenericAI*> mAIs;
		Vector<GenericAI*> mFreeAIs;

		bool mAIEnabled;
		uint32 mSkipped;
	public:
		void AssignAITo( EntitySystem::EntityHandle pawn );
		void DisconnectSelf( GenericAI * controller );
		void ClearUnusedAIs();
		void Update( float32 delta );
		bool TriggerAI();
		AIMgr();
		~AIMgr();
	};
}

#endif