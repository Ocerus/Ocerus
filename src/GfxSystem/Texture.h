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

		enum ePixelFormat { FORMAT_RGB, FORMAT_ARGB };

		bool LoadFromBitmap(const void* pixels, uint32 pixelsLength, uint32 width, uint32 height, ePixelFormat format);

		/*library specific texture type _GetTexture(void); */
		uint64 GetTexture(void);	// hge's DWORD = uint64

		virtual uint32 GetWidth(bool bOriginal = false);
		virtual uint32 GetHeight(bool bOriginal = false);
		
	protected:
		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);

		void Init(void);

		uint64 mHandle;
		ePixelFormat mFormat;

		friend class GfxRenderer;
	};


	typedef SmartPointer<Texture> TexturePtr;
}

#endif