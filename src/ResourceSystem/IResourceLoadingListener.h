#ifndef _IRESOURCELOADINGLISTENER_H_
#define _IRESOURCELOADINGLISTENER_H_

namespace ResourceSystem
{
	class IResourceLoadingListener
	{
		IResourceLoadingListener() {}
		virtual ~IResourceLoadingListener() {}

		virtual void ResourceGroupLoadStarted(const String& groupName, uint32 resourceCount) = 0;
		virtual void ResourceLoadStarted(const ResourcePtr& resource) = 0;
        virtual void ResourceLoadEnded(void) = 0;
		virtual void ResourceGroupLoadEnded(void);
	};
}

#endif