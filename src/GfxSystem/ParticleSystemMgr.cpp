#include "Common.h"
#include "ParticleSystemMgr.h"
#include "hgeparticle.h"

using namespace GfxSystem;

ParticleSystemMgr::ParticleSystemMgr(void)
{
	mScale = 1;
}

ParticleSystemMgr::~ParticleSystemMgr(void)
{
	KillAll();
}

void ParticleSystemMgr::Update(float32 delta)
{
	PSList del;
	for(PSList::iterator i = mPsList.begin(); i != mPsList.end(); ++i)
	{
		(*i)->Update(delta);
		if((*i)->GetAge()==-2.0f && (*i)->GetParticlesAlive()==0)
		{
			if ((*i)->GetLifeTime() != -1.0f) del.push_back(*i);
		}
	}
	while (!del.empty()) 
	{
		UnregisterPS(del.back());
		del.pop_back();
	}
}

void ParticleSystemMgr::Render(float32 scale)
{
	for(PSList::iterator i = mPsList.begin(); i != mPsList.end(); ++i)
	{ 
		if (!(*i)->GetRenderDone())
		{
			if (scale != -1.0f) (*i)->SetScale(scale);
			(*i)->Render();
		}
		(*i)->SetRenderDone();
	}
}

void ParticleSystemMgr::SetScale(float32 s)
{
	for(PSList::iterator i = mPsList.begin(); i != mPsList.end(); ++i) (*i)->SetScale(s);
	mScale = s;
}

ParticleSystemPtr ParticleSystemMgr::SpawnPS(StringKey group, StringKey name)
{
	ParticleResourcePtr psi = gResourceMgr.GetResource(group, name);
	if (psi.IsNull())
		return ParticleSystemPtr();
	mPsList.push_back(ParticleSystemPtr(new ParticleSystem(new hgeParticleSystem(psi->GetPsi()))));
	return mPsList.back();	
}

ParticleSystemPtr ParticleSystemMgr::SpawnPS(char* groupSlashName)
{
	ParticleResourcePtr psi = gResourceMgr.GetResource(groupSlashName);
	if (psi.IsNull())
		return ParticleSystemPtr();
	mPsList.push_back(ParticleSystemPtr(new ParticleSystem(new hgeParticleSystem(psi->GetPsi()))));
	return mPsList.back();	
}

ParticleSystemPtr ParticleSystemMgr::SpawnPS(StringKey group, StringKey name, int32 x, int32 y)
{
	ParticleResourcePtr psi = gResourceMgr.GetResource(group, name);
	if (psi.IsNull())
		return ParticleSystemPtr();
	mPsList.push_back(ParticleSystemPtr(new ParticleSystem(new hgeParticleSystem(psi->GetPsi()))));
	mPsList.back()->SetScale(mScale);
	mPsList.back()->FireAt(gGfxRenderer.ScreenToWorldX(x), gGfxRenderer.ScreenToWorldY(y));
	return mPsList.back();	
}

ParticleSystemPtr ParticleSystemMgr::SpawnPS(char* groupSlashName, int32 x, int32 y)
{
	ParticleResourcePtr psi = gResourceMgr.GetResource(groupSlashName);
	if (psi.IsNull())
		return ParticleSystemPtr();
	mPsList.push_back(ParticleSystemPtr(new ParticleSystem(new hgeParticleSystem(psi->GetPsi()))));
	mPsList.back()->SetScale(mScale);
	mPsList.back()->FireAt(gGfxRenderer.ScreenToWorldX(x), gGfxRenderer.ScreenToWorldY(y));
	return mPsList.back();	
}

bool ParticleSystemMgr::IsPSAlive(ParticleSystemPtr ps)
{
	for(PSList::iterator i = mPsList.begin(); i != mPsList.end(); ++i) {
		if (*i == ps) {
			return true;
		}
	}
	return false;
}

void ParticleSystemMgr::UnregisterPS(ParticleSystemPtr ps)
{
	ps->Unload();
	mPsList.remove(ps);

}

void ParticleSystemMgr::KillAll(void)
{
	for(PSList::iterator i = mPsList.begin(); i != mPsList.end(); ++i)
		(*i)->Unload();
	mPsList.clear();
}


