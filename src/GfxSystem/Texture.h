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

		enum ePixelFormat
		{
			AUTO = 0,
			L = 1,
			LA = 2,
			RGB = 3,
			RGBA = 4
		};

		bool LoadFromBitmap(const void* pixels, uint32 pixelsLength, uint32 width, uint32 height, ePixelFormat format);

		/// Returns implementation specific pointer to the texture object.
		uint32 GetTexture(void);

		virtual uint32 GetWidth(void);
		virtual uint32 GetHeight(void);

		/// Returns the resource type associated with this class.
		static ResourceSystem::eResourceType GetResourceType() { return ResourceSystem::RESTYPE_TEXTURE; }

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