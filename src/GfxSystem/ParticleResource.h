#ifndef _PARTICLERESOURCE_H_
#define _PARTICLERESOURCE_H_

#include "Common.h"
#include "../ResourceSystem/Resource.h"
#include "../Utility/Settings.h"
#include "hgeanim.h"
#include "hgeparticle.h"

namespace GfxSystem
{
	class ParticleResource : public ResourceSystem::Resource
	{
	public:
		virtual ~ParticleResource(void);
		static ResourceSystem::ResourcePtr CreateMe(void);

		//static void LoadSprite(void);
		//static void UnloadSprite(void);

		inline hgeParticleSystemInfo* GetPsi(void) { return mPsi; }

	protected:	
		//static hgeAnimation* mSprite;
		hgeAnimation* mSprite;

		hgeParticleSystemInfo* mPsi;

		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);	
	};
	
	typedef SmartPointer<ParticleResource> ParticleResourcePtr;
}


#endif