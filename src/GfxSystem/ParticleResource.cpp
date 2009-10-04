#include "Common.h"
#include "ParticleResource.h"
#include "Texture.h"

using namespace GfxSystem;

ResourceSystem::ResourcePtr ParticleResource::CreateMe(void)
{
	return ResourceSystem::ResourcePtr(new ParticleResource());
}

ParticleResource::~ParticleResource(void)
{

}

int32 ReadInt(InputStream& is)
{
	int a = is.get();
	int b = is.get();
	int c = is.get();
	int d = is.get();
	return a + (b<<8) + (c<<16) + (d<<24);
}

float32 ReadFloat(InputStream& is)
{
	union
	{
		float32 f;
		unsigned char b[4];
	} dat1;
	dat1.b[0] = is.get();
	dat1.b[1] = is.get();
	dat1.b[2] = is.get();
	dat1.b[3] = is.get();
	return dat1.f;	
}


bool ParticleResource::LoadImpl(void)
{
	mPsi = new hgeParticleSystemInfo();
	
	GfxSystem::TexturePtr tex = gResourceMgr.GetResource("psi", "particles.png");
	mSprite = new hgeAnimation(tex->GetTexture(), 16, 1.0f, 0, 0, 32, 32);
	mSprite->SetHotSpot(16,16);

	InputStream& is = OpenInputStream(ISM_BINARY);

	mPsi->sprite = (hgeSprite*)ReadInt(is);
	mPsi->nEmission = ReadInt(is);
	
	mPsi->fLifetime = ReadFloat(is);

	mPsi->fParticleLifeMin = ReadFloat(is);
	mPsi->fParticleLifeMax = ReadFloat(is);

	mPsi->fDirection = ReadFloat(is);
	mPsi->fSpread = ReadFloat(is);
	
	mPsi->bRelative = ((ReadInt(is) > 0) ? true : false);

	mPsi->fSpeedMin = ReadFloat(is);
	mPsi->fSpeedMax = ReadFloat(is);

	mPsi->fGravityMin = ReadFloat(is);
	mPsi->fGravityMax = ReadFloat(is);

	mPsi->fRadialAccelMin = ReadFloat(is);
	mPsi->fRadialAccelMax = ReadFloat(is);

	mPsi->fTangentialAccelMin = ReadFloat(is);
	mPsi->fTangentialAccelMax = ReadFloat(is);

	mPsi->fSizeStart = ReadFloat(is);
	mPsi->fSizeEnd = ReadFloat(is);
	mPsi->fSizeVar = ReadFloat(is);

	mPsi->fSpinStart = ReadFloat(is);
	mPsi->fSpinEnd = ReadFloat(is);
	mPsi->fSpinVar = ReadFloat(is);

	mPsi->colColorStart.r = ReadFloat(is); 
	mPsi->colColorStart.g = ReadFloat(is);
	mPsi->colColorStart.b = ReadFloat(is);
	mPsi->colColorStart.a = ReadFloat(is);

	mPsi->colColorEnd.r = ReadFloat(is);
	mPsi->colColorEnd.g = ReadFloat(is);
	mPsi->colColorEnd.b = ReadFloat(is);
	mPsi->colColorEnd.a = ReadFloat(is);

	mPsi->fColorVar = ReadFloat(is);
	mPsi->fAlphaVar = ReadFloat(is);

	CloseInputStream();
		
	mSprite->SetFrame((DWORD)mPsi->sprite & 0xFFFF);
	mSprite->SetBlendMode((DWORD)mPsi->sprite >> 16);
	mPsi->sprite = mSprite; 

	return true;
}

bool ParticleResource::UnloadImpl(void)
{
	delete mPsi;
	delete mSprite;
	return true;
}