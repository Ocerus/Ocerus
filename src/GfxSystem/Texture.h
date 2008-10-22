#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "../ResourceSystem/Resource.h"

namespace GfxSystem
{
	class Texture : public ResourceSystem::Resource
	{
	public:
		virtual ~Texture() {}

		static ResourceSystem::ResourcePtr CreateMe();

		/*library specific texture*/ void _GetTexture();

		virtual void Load();
		virtual void Unload();
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