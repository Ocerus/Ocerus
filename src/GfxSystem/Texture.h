/// @file
/// Derived class from Resource representing texture.

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

		/// Returns implementation specific pointer to the texture object.
		uint32 GetTexture(void);

		uint32 GetWidth(void);
		uint32 GetHeight(void);

		/// Returns the resource type associated with this class.
		static ResourceSystem::eResourceType GetResourceType() { return ResourceSystem::RESTYPE_TEXTURE; }

	protected:

		friend class GfxRenderer;

		virtual size_t LoadImpl(void);
		virtual bool UnloadImpl(void);
		void Init(void);

	private:
		uint32 mHandle;
		ePixelFormat mFormat;
		uint32 mHeight, mWidth;

	};
}

#endif