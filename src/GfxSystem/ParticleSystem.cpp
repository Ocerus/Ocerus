#include "Common.h"
#include "ParticleSystem.h"

using namespace GfxSystem;

ParticleSystem::ParticleSystem(hgeParticleSystem* ps)
{
	this->ps = ps;
	renderDone = false;
	loaded = true;
}

ParticleSystem::~ParticleSystem(void) 
{
	//gLogMgr.LogMessage("Particle SYSTEM Destroyed");
	if (loaded)
	{
		delete ps;
	}
}

void ParticleSystem::Unload(void)
{
	//gPSMgr.UnregisterPS(ParticleSystemPtr(this));
	//gLogMgr.LogMessage("Particle SYSTEM Unloaded");
	if (loaded)
	{
		delete ps;
	}
	loaded = false;
}

void ParticleSystem::MoveTo(int32 x, int32 y, bool bMoveParticles)
{
	if (loaded) ps->MoveTo((float32)x,(float32)y,bMoveParticles);
}

void ParticleSystem::Transpose(float dx, float dy)
{
	if (loaded) ps->Transpose(dx, dy);
}

void ParticleSystem::SetScale(float scale)
{
	if (loaded) ps->SetScale(scale);
}

void ParticleSystem::SetAngle(float angle)
{
	if (loaded) ps->info.fDirection = angle;
}


void ParticleSystem::SetSpeed(float fSpeedMin, float fSpeedMax)
{
	if (loaded) 
	{
		ps->info.fSpeedMin = fSpeedMin;
		ps->info.fSpeedMax = fSpeedMax;
	}
}

hgeParticleSystem* ParticleSystem::GetPS(void)
{
	return ps;
}

void ParticleSystem::FireAt(int32 x, int32 y)
{
	if (loaded) ps->FireAt((float32)x, (float32)y);
}

void ParticleSystem::Update(float delta)
{
	if (loaded) ps->Update(delta);
}

void ParticleSystem::Render(void)
{
	if (loaded) ps->Render(); 
	renderDone = true; 
}

int ParticleSystem::GetParticlesAlive(void) const 
{ 
	return ((loaded) ? ps->GetParticlesAlive() : 0); 
}

float ParticleSystem::GetAge(void) const 
{
	return ((loaded) ? ps->GetAge() : -2); 
}

bool ParticleSystem::GetRenderDone(void) const 
{
	return renderDone; 
}

void ParticleSystem::SetRenderDone(void)
{
	renderDone = false; 
}