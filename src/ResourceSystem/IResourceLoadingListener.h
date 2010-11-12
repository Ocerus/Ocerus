/// @file
/// Notifications of changes in the progress of loading of resources.

#ifndef _IRESOURCELOADINGLISTENER_H_
#define _IRESOURCELOADINGLISTENER_H_

#include "Resource.h"

namespace ResourceSystem
{
	/// This class provides callbacks from the resource manager, so it can be used to display info about resource
	///	loading.
	class IResourceLoadingListener
	{
	public:

		/// Default constructor.
		IResourceLoadingListener(void) {}

		/// Default virtual destructor.
		virtual ~IResourceLoadingListener(void) {}

		/// Called when a group of resources is being loaded. In resourceCount you are told how many resources will be
		///	loaded, so that you have an estimate.
		virtual void ResourceGroupLoadStarted(const string& groupName, uint32 resourceCount) = 0;

		/// Called when one single resource is being loaded.
		virtual void ResourceLoadStarted(const Resource* resource) = 0;

		/// Called when the resource which previously started loading is now loaded.
        virtual void ResourceLoadEnded(void) = 0;

		/// Called when a group of resources is done with loading.
		virtual void ResourceGroupLoadEnded(void) = 0;
	};
}

#endif