#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

#include "Common.h"
#include "../Utility/Settings.h"
#include "hgeparticle.h"

//class hgeParticleSystem;

namespace GfxSystem
{
	class ParticleSystem
	{
	public:
		~ParticleSystem(void);
		
		void MoveTo(float x, float y, bool bMoveParticles = false);
		void Transpose(float dx, float dy);
		void SetScale(float scale);
		void SetAngle(float angle);
		void SetSpeed(float fSpeedMin, float fSpeedMax);
		void FireAt(float x, float y);
		void Update(float delta);
		void Render(void);

		hgeParticleSystem* GetPS(void);
		int GetParticlesAlive(void) const;
		float GetAge(void) const;
		bool GetRenderDone(void) const;
		void SetRenderDone(void);


	private:
		friend class ParticleSystemMgr;
		ParticleSystem(hgeParticleSystem* ps);
		void Unload(void);

		hgeParticleSystem* ps;
		bool renderDone;
		bool loaded;
	};
	typedef SmartPointer<ParticleSystem> ParticleSystemPtr;
}

#endif