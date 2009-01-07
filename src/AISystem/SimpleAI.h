#ifndef _SIMPLEAI_H_
#define _SIMPLEAI_H_

#include <list>
#include "GenericAI.h"

namespace AISystem {

	struct TurretInfo {
		EntitySystem::EntityHandle mHandle;
		float32 mHalfArcAngle;
		float32 mPossibleLeft; // to get right just 1 - mHalfArcAngle
		float32 mCurrentRelativeAngle;

		TurretInfo( EntitySystem::EntityHandle handle );
	};

	class SimpleAI : public GenericAI {
	protected:
		EntitySystem::EntityHandle mEnemyPlayer;
		std::list<TurretInfo> mEngines;
		std::list<TurretInfo> mWeapons;
		std::list<EntitySystem::EntityHandle> mEnemyPlatforms;

		void SetEngines();
		void TargetWeapons();
		void Refresh();
	public:
		void DoLogic( float32 delta );
		SimpleAI( EntitySystem::EntityHandle my_player, EntitySystem::EntityHandle enemy_player);
	};
}

#endif