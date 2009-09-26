#ifndef _PARTICLESYSTEMMGR_H_
#define _PARTICLESYSTEMMGR_H_

#include "../Utility/Settings.h"
#include "ParticleSystem.h"
#include "ParticleResource.h"
#include <list>

/// @name Forward declarations of hge classes
//@{
struct hgeParticleSystemInfo;
class hgeParticleManager;
class hgeParticleSystem;
//@}

/// @name Macro for easier use
#define gPSMgr GfxSystem::ParticleSystemMgr::GetSingleton()

namespace GfxSystem
{
	/** This class manage all active ParticleSystems
	 */
	class ParticleSystemMgr : public Singleton<ParticleSystemMgr>
	{
	public:
		ParticleSystemMgr(void);
		~ParticleSystemMgr(void);
		
		/// @name Update all active particle systems
		void Update(float32 delta);
		/// @name Render all PS which were not rendered manually
		void Render(float32 scale = -1.0f);
		/// @name Set scale of all active systems
		void SetScale(float32 s);
		
		/// @name Return number of active particle systems
		int32 GetNum(void) { return mPsList.size(); }

		/// @name Create new particle system
		//@{
		ParticleSystemPtr SpawnPS(char* psiName);
		ParticleSystemPtr SpawnPS(StringKey group, StringKey name);
		ParticleSystemPtr SpawnPS(char* psiName, int32 x, int32 y);
		ParticleSystemPtr SpawnPS(StringKey group, StringKey name, int32 x, int32 y);
		//@}

		/// @name Return true if PS is registred in manager, false otherwise
		bool IsPSAlive(ParticleSystemPtr ps);

		/// @name Unregister PS from manager, unloading all of its internal resources.
		void UnregisterPS(ParticleSystemPtr ps);
		/// @name Unregister all PS from manager
		void KillAll(void);
	private:
		typedef List<ParticleSystemPtr> PSList;

		float32 mScale;
		PSList mPsList;
	};
}

#endif