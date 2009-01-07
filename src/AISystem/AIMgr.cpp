#include "Common.h"
#include "AIMgr.h"
#include "SimpleAI.h"

namespace AISystem {
	AIMgr::AIMgr() : mAIEnabled(false), mSkipped(0) {}

	void AIMgr::AssignAITo( EntitySystem::EntityHandle pawn ) {
		if (mFreeAIs.size() > 0) {
			mFreeAIs.back()->AttachTo( pawn );
			mFreeAIs.pop_back();
		} else {
			// change one RTTI system can create instance based on string!
			mAIs.push_back( DYN_NEW SimpleAI( pawn, gEntityMgr.FindFirstEntity( "ship0" ) ) );
		}
	}

	void AIMgr::DisconnectSelf( GenericAI * controller ) {
		mFreeAIs.push_back( controller );
	}

	void AIMgr::ClearUnusedAIs() {
		std::vector<GenericAI*>::iterator iter = mAIs.begin();

		while ( iter != mAIs.end() ) {
			DYN_DELETE *iter;
			++iter;
		}

		mFreeAIs.clear();
	}

	void AIMgr::Update( float32 delta ) {
		if ( mSkipped % skip_frames == 0 && mAIEnabled ) {
			std::vector<GenericAI*>::iterator iter = mAIs.begin();
			while ( iter != mAIs.end() ) {
				if ( !((SimpleAI*)*iter)->IsFree() )
					((SimpleAI*)*iter)->DoLogic( delta );
				++iter;
			}
		}
		++mSkipped;
	}

	bool AIMgr::TriggerAI() {
		return mAIEnabled = !mAIEnabled;
	}

	AIMgr::~AIMgr() {
		std::vector<GenericAI*>::iterator iter = mAIs.begin();
		while ( iter != mAIs.end() ) {
			DYN_DELETE *iter;
			++iter;
		}
	}
}