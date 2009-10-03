#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

#include "Base.h"
//TODO tohle smazat
#include "hgeparticle.h"
#include "GfxRenderer.h"

namespace GfxSystem
{
	/// This class is a wrapper around hgeParticleSystem class. It forwards some methods and add some
	/// new for convenience.	  
	//TODO cely predelat poradne.
	class ParticleSystem
	{
	public:

		/// Moves the particle system to new world coordinates
		inline void MoveTo(Vector2 pos, bool bMoveParticles = true) { MoveTo(pos.x, pos.y, bMoveParticles); }

		/// Moves the particle system to new world coordinates
		void MoveTo(float32 x, float32 y, bool bMoveParticles = true);

		/// Set scale of PS.
		void SetScale(float scale);

		/// Set angle of PS. 0 pi = 12 hour (increments clockwise). Argument in radians.
		void SetAngle(float angle);

		/// Set speed of emitted particles.
		void SetSpeed(float fSpeedMin, float fSpeedMax);

		/// Start emitting at given (world) coordinates.
		void FireAt(float32 x, float32 y);

		/// Start emitting.
		void Fire(void);

		/// Stop emitting.
		void Stop(void);

		/// Update logic and position of particles.
		void Update(float delta);

		/// Render PS.
		void Render(void);

		/// Return underlying hgeParticleSystem (for debug only, do not use otherwise).
		hgeParticleSystem* GetPS(void);

		/// Return number of particles in the system.
		int32 GetParticlesAlive(void) const;

		/// Return age of the system. -2 = system is dead.
		float GetAge(void) const;

		/// Return true if object was rendered in the last render cycle.
		bool GetRenderDone(void) const;

		/// Return true if the system is active (emitting particles).
		bool GetActiveState(void) const;

		/// Return lifetime of the system. -1 means neverending system.
		float32 GetLifeTime(void) const;

		/// Return current coordinates.
		float32 GetWorldX(void) const;

		/// Return current coordinates.
		float32 GetWorldY(void) const;

		/// Set rendering flag to false.
		void SetRenderDone(void);

		/// Set the state of the system. True if the system is active, false otherwise.
		void SetActiveState(bool state);

		~ParticleSystem(void);

	private:

		friend class ParticleSystemMgr;

		/// Private constructor. New systems can only be created with ParticleSystem manager
		ParticleSystem(hgeParticleSystem* ps);

		/// Unloads the underlying hgePS.
		void Unload(void);

		hgeParticleSystem* mPs;
		bool mRenderDone;
		bool mLoaded;
		bool mActive;
		float32 mPositionX;
		float32 mLastPositionX;
		float32 mPositionY;
		float32 mLastPositionY;
		float32 mScale;
		bool mMoveParticles;
	};
}

#endif