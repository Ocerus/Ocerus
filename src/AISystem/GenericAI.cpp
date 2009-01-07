#include "Common.h"
#include "AIMgr.h"
#include "SimpleAI.h"

namespace AISystem {

	void GenericAI::AttachTo( EntitySystem::EntityHandle pawn ) {
		mPawn = pawn;
	}

	bool GenericAI::IsFree() {
		return !mPawn.IsValid();
	}

	void GenericAI::Disconnect() {
		mPawn.Invalidate();
		gAIMgr.DisconnectSelf( this );
	}

	GenericAI::GenericAI( EntitySystem::EntityHandle pawn ) : mPawn(pawn) {}
}	