#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Base.h"
#include "../ResourceSystem/Resource.h"

namespace GfxSystem
{
	class Texture : public ResourceSystem::Resource
	{
	public:

		Texture(void);
		virtual ~Texture(void);

		static ResourceSystem::ResourcePtr CreateMe(void);

		enum ePixelFormat { FORMAT_RGB, FORMAT_ARGB };

		bool LoadFromBitmap(const void* pixels, uint32 pixelsLength, uint32 width, uint32 height, ePixelFormat format);

		/// Returns implementation specific pointer to the texture object.
		uint32 GetTexture(void);

		virtual uint32 GetWidth(void);
		virtual uint32 GetHeight(void);
		
	protected:

		friend class GfxRenderer;

		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);
		void Init(void);

		uint32 mHandle;
		ePixelFormat mFormat;
		uint32 mHeight, mWidth;

	};

}

#endif