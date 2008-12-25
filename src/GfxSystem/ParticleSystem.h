#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

#include "../Utility/Settings.h"
#include "hgeparticle.h"
#include "GfxRenderer.h"

namespace GfxSystem
{
	class ParticleSystem
	{
	public:
		~ParticleSystem(void);

		inline void MoveTo(Point pos, bool bMoveParticles = false) { MoveTo(pos.x, pos.y, bMoveParticles); }
		void MoveTo(int32 x, int32 y, bool bMoveParticles = false);
		void Transpose(float dx, float dy);
		void SetScale(float scale);
		void SetAngle(float angle);
		void SetSpeed(float fSpeedMin, float fSpeedMax);
		void FireAt(int32 x, int32 y);
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