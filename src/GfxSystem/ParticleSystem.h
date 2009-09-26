#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

#include "Settings.h"
#include "ResourcePointers.h"
//TODO tohle smazat
#include "hgeparticle.h"
#include "GfxRenderer.h"

namespace GfxSystem
{
	/** This class is a wrapper around hgeParticleSystem class. It forwards some methods and add some
	    new for convenience.	  
	 */
	//TODO cely predelat poradne.
	class ParticleSystem
	{
	public:
		~ParticleSystem(void);
		
		/// @name Move particle system to new WORLD coordinates
		//@{
		inline void MoveTo(Vector2 pos, bool bMoveParticles = true) { MoveTo(pos.x, pos.y, bMoveParticles); }
		void MoveTo(float32 x, float32 y, bool bMoveParticles = true);
		//@}

		/// @name Set scale of PS
		void SetScale(float scale);
		/// @name Set angle of PS. 0 pi = 12 hour (increments clockwise). Argument in radians
		void SetAngle(float angle);
		/// @name Set speed of emitted particles
		void SetSpeed(float fSpeedMin, float fSpeedMax);
		/// @name Start emitting at given (world) coordinates
		void FireAt(float32 x, float32 y);
		/// @name Start emitting
		void Fire(void);
		/// @name Stop emitting
		void Stop(void);
		/// @name Update logic and position of particles
		void Update(float delta);
		/// @name Render PS
		void Render(void);

		/// @name Return underlying hgeParticleSystem (for debug only, do not use otherwise)
		hgeParticleSystem* GetPS(void);
		/// @name Return number of particles in the system
		int32 GetParticlesAlive(void) const;
		/// @name Return age of the system. -2 = system is dead
		float GetAge(void) const;
		/// @name Return true if object was rendered in the last render cycle
		bool GetRenderDone(void) const;
		/// @name Return true if the system is active (emitting particles)
		bool GetActiveState(void) const;
		/// @name Return lifetime of the system. -1 means neverending system
		float32 GetLifeTime(void) const;
		/// @name Return coordinates
		//@{
		float32 GetWorldX(void) const;
		float32 GetWorldY(void) const;
		//@}

		/// @name Use to set rendering flag to false
		void SetRenderDone(void);
		/// @name Use to set state of the system. True if the system is active, false otherwise
		void SetActiveState(bool state);

	private:
		friend class ParticleSystemMgr;
		/// @name Private constructor. New systems can only be created with ParticleSystem manager
		ParticleSystem(hgeParticleSystem* ps);
		/// @name unload underlying hgePS
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