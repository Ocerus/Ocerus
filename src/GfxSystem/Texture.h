#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "../ResourceSystem/Resource.h"

namespace GfxSystem
{
	class Texture : public ResourceSystem::Resource
	{
	public:
		virtual ~Texture(void) {}

		static ResourceSystem::ResourcePtr CreateMe(void);

		/*library specific texture*/ void _GetTexture(void);

	protected:
		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);
	};


	typedef SmartPointer<Texture> TexturePtr;
}

#endif