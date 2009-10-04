#include "Common.h"
#include "ParticleSystem.h"

using namespace GfxSystem;

ParticleSystem::ParticleSystem(hgeParticleSystem* ps):
	mPs(ps),
	mRenderDone(false),
	mLoaded(true),
	mActive(false),
	mScale(1),
	mPositionX(0),
	mPositionY(0),
	mLastPositionX(0),
	mLastPositionY(0) {}

ParticleSystem::~ParticleSystem(void) 
{
	//gLogMgr.LogMessage("Particle SYSTEM Destroyed");
	Unload();
}

void ParticleSystem::Unload(void)
{
	//gPSMgr.UnregisterPS(ParticleSystemPtr(this));
	//gLogMgr.LogMessage("Particle SYSTEM Unloaded");
	if (mLoaded)
	{
		delete mPs;
	}
	mLoaded = false;
}
/*
void ParticleSystem::MoveTo(int32 x, int32 y, bool bMoveParticles)
{
	if (mLoaded) 
	{
		//mPs->MoveTo((float32)(x / mPs->GetScale()),(float32)(y / mPs->GetScale()), bMoveParticles);
		//mPs->MoveTo((float32)(x),(float32)(y), bMoveParticles);
	}
}*/

void ParticleSystem::MoveTo(float32 x, float32 y, bool bMoveParticles)
{
	mLastPositionX = mPositionX;
	mLastPositionY = mPositionY;
	mPositionX = x;
	mPositionY = y;
	mMoveParticles = bMoveParticles;
}


void ParticleSystem::SetScale(float32 scale)
{
	mScale = scale;
}

void ParticleSystem::SetAngle(float32 angle)
{
	//Hardwire: added 1/2 PI
	if (mLoaded) mPs->info.fDirection = angle + MathUtils::HALF_PI;
}


void ParticleSystem::SetSpeed(float32 fSpeedMin, float32 fSpeedMax)
{
	if (mLoaded) 
	{
		mPs->info.fSpeedMin = fSpeedMin;
		mPs->info.fSpeedMax = fSpeedMax;
	}
}

hgeParticleSystem* ParticleSystem::GetPS(void)
{
	return mPs;
}

void ParticleSystem::FireAt(float32 x, float32 y)
{
	if (mLoaded && !mActive) 
	{
		//mPs->FireAt((float32)(x / mPs->GetScale()), (float32)(y / mPs->GetScale()));
		//mPs->FireAt((float32)(x), (float32)(y));
		Stop();
		MoveTo(x, y);
		Fire();
		mActive = true;
	}
}

void ParticleSystem::Fire(void)
{
	if (mLoaded && !mActive)
	{
		mPs->Fire();
		mActive = true;
	}
}

void ParticleSystem::Stop(void)
{
	if (mLoaded && mActive)
	{
		mPs->Stop();
		mActive = false;
	}
}

void ParticleSystem::Update(float32 delta)
{
	if (mLoaded) mPs->Update(delta);
}


void ParticleSystem::Render()
{
	if (mLoaded) 
	{
		mPs->SetScale(gGfxRenderer.WorldToScreenScale(mScale));
		//Hardwire: I added this line here to workaround bug causing to move particles when the camera moved
		mPs->MoveTo((float)(gGfxRenderer.WorldToScreenX(mLastPositionX) / mPs->GetScale()),
			(float)(gGfxRenderer.WorldToScreenY(mLastPositionY) / mPs->GetScale()), true);
		mPs->MoveTo((float)(gGfxRenderer.WorldToScreenX(mPositionX) / mPs->GetScale()),
			    (float)(gGfxRenderer.WorldToScreenY(mPositionY) / mPs->GetScale()), mMoveParticles);
		mPs->Render(); 
	}
	mRenderDone = true; 
	mMoveParticles = true;
}

int32 ParticleSystem::GetParticlesAlive(void) const 
{ 
	return ((mLoaded) ? mPs->GetParticlesAlive() : 0); 
}

float32 ParticleSystem::GetAge(void) const 
{
	return ((mLoaded) ? mPs->GetAge() : -2); 
}

bool ParticleSystem::GetRenderDone(void) const 
{
	return mRenderDone; 
}

void ParticleSystem::SetRenderDone(void)
{
	mRenderDone = false; 
}

float32 ParticleSystem::GetLifeTime(void) const 
{
	return mPs->info.fLifetime; 
}

float32 ParticleSystem::GetWorldX(void) const
{
	return mPositionX;
}

float32 ParticleSystem::GetWorldY(void) const
{
	return mPositionY;
}