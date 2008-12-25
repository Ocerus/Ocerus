#ifndef _PARTICLESYSTEMMGR_H_
#define _PARTICLESYSTEMMGR_H_

#include "Common.h"
#include "../Utility/Settings.h"
#include "ParticleSystem.h"
#include "ParticleResource.h"
#include <list>

struct hgeParticleSystemInfo;
class hgeParticleManager;
class hgeParticleSystem;

/// @name Macro for easier use
#define gPSMgr GfxSystem::ParticleSystemMgr::GetSingleton()

namespace GfxSystem
{
	class ParticleSystemMgr : public Singleton<ParticleSystemMgr>
	{
	public:
		ParticleSystemMgr(void);
		~ParticleSystemMgr(void);

		void Update(float delta);
		void Render(void);
		void Transpose(float dx, float dy);
		void SetScale(float s);
		void GetTransposition(float* dx, float* dy) const {*dx=tX; *dy=tY;}
		int GetNum(void) { return psList.size(); }

		//ParticleSystem* SpawnPS(hgeParticleSystemInfo* psi, float x, float y);
		ParticleSystemPtr SpawnPS(const string& psiName , float x, float y);
		bool IsPSAlive(ParticleSystemPtr ps);
		void UnregisterPS(ParticleSystemPtr ps);
		//void KillPS(ParticleSystemPtr ps);
		void KillAll(void);
	private:
		ParticleSystemMgr(const ParticleSystemMgr &);
		ParticleSystemMgr& operator=(const ParticleSystemMgr &);
		
		typedef std::list<ParticleSystemPtr> PSList;
		typedef std::list<ParticleSystemPtr*> pPSList;

		float tX;
		float tY;
		PSList psList;
	};
}

#endif