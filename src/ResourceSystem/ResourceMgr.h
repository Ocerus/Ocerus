#ifndef _RESOURCEMGR_H_
#define _RESOURCEMGR_H_

#include "ResourceGroup.h"
#include "Resource.h"

namespace ResourceSystem
{
	class ResourceLoadingListener;

	enum eResourceTypes { RESTYPE_IMAGE=0, NUM_RESOURCE_TYPES,	RESTYPE_AUTODETECT };

	struct ResourceGroup
	{
		string mName;
		vector<ResourcePtr> mResources;
	};

	class ResourceMgr : public Singleton<ResourceMgr>
	{
	public:
		//TODO all creating CreateMe methods of all res types should be inited in the ctor
		ResourceMgr();
		~ResourceMgr();

		// in this case the resource types will be autodetected
		void AddResourceDirToGroup(const string& path, const string& group, const string& includeRegexp = "*.*", const string& excludeRegexp = "");
		void AddResourceFileToGroup(const string& filepath, const string& group, eResourceType type = RESTYPE_AUTODETECT);
		// load all resources in this group
		// doesn't need to be called, resources are loaded on-the-fly if someone needs them
		void LoadResourcesInGroup(const string& group);
		// delete all resources in this group
		void ClearGroup(const string& group);
		void UnloadUnusedResources(void);

		void SetLoadingListener(ResourceLoadingListener* listener);

		// name = group name/resource's filename
		ResourcePtr GetResource(const string& name);
		ResourcePtr GetResource(const string& group, name);

	private:
		map<string, ResourceGroup*> mResourceGroups;
		ResourceLoadingListener* mListener;

		typedef ResourcePtr (*ResourceCreationMethod)();
		ResourceCreationMethod mResourceCreationMethods[NUM_RESOURCE_TYPES];
		void RegisterResourceCreationMethods(void);


	};
}

#endif