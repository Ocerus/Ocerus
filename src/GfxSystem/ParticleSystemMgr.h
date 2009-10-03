#ifndef _PARTICLESYSTEMMGR_H_
#define _PARTICLESYSTEMMGR_H_

#include "Base.h"
#include "ParticleSystem.h"
#include "ParticleResource.h"

struct hgeParticleSystemInfo;
class hgeParticleManager;
class hgeParticleSystem;

/// Macro for easier use
#define gPSMgr GfxSystem::ParticleSystemMgr::GetSingleton()

namespace GfxSystem
{
	/// This class manage all active ParticleSystems
	class ParticleSystemMgr : public Singleton<ParticleSystemMgr>
	{
	public:
		ParticleSystemMgr(void);
		~ParticleSystemMgr(void);
		
		/// Update all active particle systems.
		void Update(float32 delta);

		/// Render all PS which were not rendered manually.
		void Render(float32 scale = -1.0f);

		/// Set scale of all active systems.
		void SetScale(float32 s);
		
		/// Return number of active particle systems.
		int32 GetNum(void) { return mPsList.size(); }

		/// Create new particle system.
		ParticleSystemPtr SpawnPS(char* psiName);

		/// Create new particle system.
		ParticleSystemPtr SpawnPS(StringKey group, StringKey name);

		/// Create new particle system.
		ParticleSystemPtr SpawnPS(char* psiName, int32 x, int32 y);

		/// Create new particle system.
		ParticleSystemPtr SpawnPS(StringKey group, StringKey name, int32 x, int32 y);

		/// Return true if PS is registred in manager, false otherwise.
		bool IsPSAlive(ParticleSystemPtr ps);

		/// Unregister PS from manager, unloading all of its internal resources.
		void UnregisterPS(ParticleSystemPtr ps);

		/// Unregister all PS from manager
		void KillAll(void);

	private:

		typedef list<ParticleSystemPtr> PSList;

		float32 mScale;
		PSList mPsList;
	};
}

#endif