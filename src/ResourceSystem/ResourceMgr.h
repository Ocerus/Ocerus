#ifndef _RESOURCEMGR_H_
#define _RESOURCEMGR_H_

#include "../Common.h"
#include "Resource.h"

#define gResourceMgr ResourceSystem::ResourceMgr::GetSingleton()

namespace ResourceSystem
{
	class IResourceLoadingListener;

	class ResourceMgr : public Singleton<ResourceMgr>
	{
	public:
		ResourceMgr(const string& basedir);
		~ResourceMgr();

		// in this case the resource types will be autodetected
		bool AddResourceDirToGroup(const string& path, const string& group, const string& includeRegexp = "*.*", const string& excludeRegexp = "");
		bool AddResourceFileToGroup(const string& filepath, const string& group, Resource::eType type = Resource::TYPE_AUTODETECT, bool pathRelative = true);
		// load all resources in this group
		// doesn't need to be called, resources are loaded on-the-fly if someone needs them
		void LoadResourcesInGroup(const string& group);
		// delete all resources in this group
		void ClearGroup(const string& group);
		void UnloadUnusedResources(void);

		void SetLoadingListener(IResourceLoadingListener* listener);

		// name = group name/resource's filename
		ResourcePtr GetResource(const string& name);
		ResourcePtr GetResource(const string& group, const string& name);

	private:
		typedef ResourcePtr (*ResourceCreationMethod)();
		typedef map<string, map<string, ResourcePtr> > ResourceGroupMap;
		typedef map<string, Resource::eType> ExtToTypeMap;

		string mBaseDir;
		ResourceGroupMap mResourceGroups;
		ExtToTypeMap mExtToTypeMap;
		IResourceLoadingListener* mListener;
		ResourceCreationMethod mResourceCreationMethods[Resource::NUM_TYPES];
	};
}

#endif