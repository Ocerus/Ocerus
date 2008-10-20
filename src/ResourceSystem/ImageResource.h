#ifndef _IMAGERESOURCE_H_
#define _IMAGERESOURCE_H_

#include "Resource.h"

namespace ResourceSystem
{
	class ImageResource : public Resource
	{
	public:
		virtual ~ImageResource() {}

		static ResourcePtr CreateMe();

		/*Bitmap*/ void GetBitmap();

		virtual void Load();
		virtual void Unload();
	};


	class ImageResourcePtr : public SmartPointer<ImageResource>
	{
	public:
		explicit ImageResourcePtr(ImageResource* pointer): SmartPointer<ImageResource>(pointer) {}
		ImageResourcePtr(const ImageResourcePtr& r): SmartPointer<ImageResource>(r) {}
		ImageResourcePtr(const ResourcePtr& r): SmartPointer<ImageResource>() {	Recreate(r); }
		ImageResourcePtr& operator=(const ImageResourcePtr& r)
		{
			if (mPointer == static_cast<ImageResource*>(r.GetPointer()))
				return *this;
			Release();
			Recreate(r);
			return *this;
		}
	private:
		void Recreate(const ImageResourcePtr& r)
		{
			mPointer = static_cast<ImageResource*>(r.GetPointer());
			mUseCountPtr = r.GetUseCountPtr();
			if (mUseCountPtr)
				++(*mUseCountPtr);
		}
	};
}

#endif