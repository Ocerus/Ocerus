#ifndef _PARTICLERESOURCE_H_
#define _PARTICLERESOURCE_H_

#include "../ResourceSystem/Resource.h"
#include "../Utility/Settings.h"
#include "hgeanim.h"
#include "hgeparticle.h"

namespace GfxSystem
{
	/** This class is used to load and manage .psi resource file	   
	 */
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
	
	typedef SmartPointer<ParticleResource> ParticleResourcePtr;
}


#endif