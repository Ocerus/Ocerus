#ifndef _HGETEXTURE_H_
#define _HGETEXTURE_H_

#include "Texture.h"

class HgeTexture : Texture
{
	public:
		virtual ~Texture(void) {}

		static ResourceSystem::ResourcePtr CreateMe(void);

		/*library specific texture*/ void _GetTexture(void);

		virtual uint32 GetWidth();
		virtual uint32 GetHeight();
		
	protected:
		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);
};
		

#endif