#ifndef _GENERICAI_H_
#define _GENERICAI_H_

namespace AISystem {
	class GenericAI {
	protected:
		EntitySystem::EntityHandle mPawn;
	public:
		void AttachTo( EntitySystem::EntityHandle pawn );
		void Disconnect();
		virtual void DoLogic( float32 delta ) = 0;
		bool IsFree();
		GenericAI( EntitySystem::EntityHandle pawn );
	};
}

#endif