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


	class TexturePtr : public SmartPointer<Texture>
	{
	public:
		explicit TexturePtr(Texture* pointer): SmartPointer<Texture>(pointer) {}
		TexturePtr(const TexturePtr& r): SmartPointer<Texture>(r) {}
		TexturePtr(const ResourceSystem::ResourcePtr& r): SmartPointer<Texture>() {	Recreate(r); }
		TexturePtr& operator=(const TexturePtr& r)
		{
			if (mPointer == static_cast<Texture*>(r.GetPointer()))
				return *this;
			Release();
			Recreate(r);
			return *this;
		}
	private:
		void Recreate(const TexturePtr& r)
		{
			mPointer = static_cast<Texture*>(r.GetPointer());
			mUseCountPtr = r.GetUseCountPtr();
			if (mUseCountPtr)
				++(*mUseCountPtr);
		}
	};
}

#endif