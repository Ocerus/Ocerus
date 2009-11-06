#ifndef _PARTICLERESOURCE_H_
#define _PARTICLERESOURCE_H_

#include "Base.h"
#include "../ResourceSystem/Resource.h"

class hgeAnimation;
struct hgeParticleSystemInfo;

namespace GfxSystem
{
	class ParticleResource : public ResourceSystem::Resource
	{
	public:
		virtual ~ParticleResource(void);
		static ResourceSystem::ResourcePtr CreateMe(void);

		inline hgeParticleSystemInfo* GetPsi(void)
		{
			EnsureLoaded();
			return mPsi;
		}

	protected:
		hgeAnimation* mSprite;

		hgeParticleSystemInfo* mPsi;

		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);
	};
}


#endif
