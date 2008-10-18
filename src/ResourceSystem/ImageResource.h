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

		Bitmap GetBitmap();

		virtual void Load();
		virtual void Unload();
	};
}

#endif