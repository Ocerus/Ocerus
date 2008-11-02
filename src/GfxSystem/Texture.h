#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "../ResourceSystem/Resource.h"
#include "../Utility/Settings.h"

namespace GfxSystem
{
	class Texture : public ResourceSystem::Resource
	{
	public:
		virtual ~Texture(void) {}

		static ResourceSystem::ResourcePtr CreateMe(void);

		/*library specific texture type _GetTexture(void); */
		uint64 GetTexture(void);	// hge's DWORD = uint64

		virtual uint32 GetWidth(void);
		virtual uint32 GetHeight(void);
		
	protected:
		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);

		void Init(void);

		uint64 mHandle;

		friend class GfxRenderer;
	};


	typedef SmartPointer<Texture> TexturePtr;
}

#endif