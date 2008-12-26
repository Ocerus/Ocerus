#include "Common.h"
#include "ParticleSystem.h"

using namespace GfxSystem;

ParticleSystem::ParticleSystem(hgeParticleSystem* ps)
{
	mPs = ps;
	mRenderDone = false;
	mLoaded = true;
	mActive = false;
}

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
		DYN_DELETE mPs;
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
	mWorldX = x;
	mWorldY = y;
	mMoveParticles = bMoveParticles;
}


void ParticleSystem::SetScale(float32 scale)
{
	if (mLoaded) mPs->SetScale(scale);
}

void ParticleSystem::SetAngle(float32 angle)
{
	if (mLoaded) mPs->info.fDirection = angle;
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


void ParticleSystem::Render(float32 scale)
{
	if (mLoaded) 
	{
		if (scale != -1.0f) mPs->SetScale(scale);
		mPs->MoveTo((float)(gGfxRenderer.WorldToScreenX(mWorldX) / mPs->GetScale()),
			    (float)(gGfxRenderer.WorldToScreenY(mWorldY) / mPs->GetScale()), mMoveParticles);
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
	return mWorldX;
}

float32 ParticleSystem::GetWorldY(void) const
{
	return mWorldY;
}