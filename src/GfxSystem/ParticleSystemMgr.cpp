#include "Common.h"
#include "ParticleSystemMgr.h"
#include "hgeparticle.h"

using namespace GfxSystem;

ParticleSystemMgr::ParticleSystemMgr(void)
{
	tX=tY=0.0f;
}

ParticleSystemMgr::~ParticleSystemMgr(void)
{
	KillAll();
}

void ParticleSystemMgr::Update(float delta)
{
	PSList del;
	for(PSList::iterator i = psList.begin(); i != psList.end(); i++)
	{
		(*i)->Update(delta);
		if((*i)->GetAge()==-2.0f && (*i)->GetParticlesAlive()==0)
		{
			//delete (*i);
			del.push_back(*i);
			/*std::stringstream ss;
			ss << psList.size();
			gLogMgr.LogMessage("DEL DELETE " + ss.str());
			UnregisterPS(*i);
			gLogMgr.LogMessage("DEL DELETED");*/
		}
	}
	//for(pPSList::iterator i = del.begin(); i != del.end(); i++)
	//{
	//	std::stringstream ss;
	//	ss << (*i)->IsUnique();
	//	gLogMgr.LogMessage("SYSTEM UPDATE: " + ss.str());
	//	//(!i->IsUnique()) ? (*i)->Unload() : UnregisterPS(*i);
	//	UnregisterPS((*(*i)));
	//}
	//gLogMgr.LogMessage("DEL DELETE");
	//del.clear();
	//gLogMgr.LogMessage("DEL DELETED");
	while (!del.empty()) 
	{
		UnregisterPS(del.back());
		del.pop_back();
	}
}

void ParticleSystemMgr::Render(void)
{
	for(PSList::iterator i = psList.begin(); i != psList.end(); i++)
	{ 
		if (!(*i)->GetRenderDone())
		{
			(*i)->Render();
		}
		(*i)->SetRenderDone();
	}
}

void ParticleSystemMgr::SetScale(float s)
{
	for(PSList::iterator i = psList.begin(); i != psList.end(); i++) (*i)->SetScale(s);
}

//ParticleSystem* ParticleSystemMgr::SpawnPS(hgeParticleSystemInfo* psi, float x, float y)
ParticleSystemPtr ParticleSystemMgr::SpawnPS(const string& psiName, float x, float y)
{
	ParticleResourcePtr psi = gResourceMgr.GetResource("psi", psiName);
	psList.push_back(ParticleSystemPtr(DYN_NEW ParticleSystem(DYN_NEW hgeParticleSystem(psi->GetPsi()))));
	psList.back()->FireAt(x,y);
	psList.back()->Transpose(tX,tY);
	return psList.back();	
}

bool ParticleSystemMgr::IsPSAlive(ParticleSystemPtr ps)
{
	for(PSList::iterator i = psList.begin(); i != psList.end(); i++) {
		if (*i == ps) {
			return true;
		}
	}
	return false;
}

void ParticleSystemMgr::Transpose(float dx, float dy)
{
	for(PSList::iterator i = psList.begin(); i != psList.end(); i++) (*i)->Transpose(dx, dy);
	tX = dx;
	tY = dy;
}

void ParticleSystemMgr::UnregisterPS(ParticleSystemPtr ps)
{
	ps->Unload();
	//gLogMgr.LogMessage("DTOR START");
	psList.remove(ps);
	//std::stringstream ss;
	//ss << psList.size();
	//gLogMgr.LogMessage("DTOR FINISHED " + ss.str());
}
/*
void ParticleSystemMgr::KillPS(ParticleSystemPtr ps)
{
	ps->Unload();
	UnregisterPS(ps);
}*/

void ParticleSystemMgr::KillAll(void)
{
	for(PSList::iterator i = psList.begin(); i != psList.end(); i++) UnregisterPS(*i);
		//(!i->IsUnique()) ? (*i)->Unload() : UnregisterPS(*i);
	psList.clear();
}

