/// @file
/// Derived class from Resource representing texture.

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Base.h"
#include "../ResourceSystem/Resource.h"

namespace GfxSystem
{
	/// Resource class representing texture.
	class Texture : public ResourceSystem::Resource
	{
	public:

		/// Default constructor.
		Texture(void);

		/// Default destructor.
		virtual ~Texture(void);

		/// Returns a pointer to a new instance of the Texture
		static ResourceSystem::ResourcePtr CreateMe(void);

		/// Returns implementation specific pointer to the texture object.
		TextureHandle GetTexture(void);

		/// Returns widh in pixels of texture.
		uint32 GetWidth(void);

		/// Returns widh in pixels of texture.
		uint32 GetHeight(void);

		/// Returns the resource type associated with this class.
		static ResourceSystem::eResourceType GetResourceType() { return ResourceSystem::RESTYPE_TEXTURE; }

	protected:

		friend class GfxRenderer;

		virtual size_t LoadImpl(void);
		virtual bool UnloadImpl(void);

		void Init(void);

	private:
		TextureHandle mHandle;
		ePixelFormat mFormat;
		uint32 mHeight, mWidth;

	};
}

#endif