#ifndef _RESOURCEMGR_H_
#define _RESOURCEMGR_H_

#include <map>
#include "Resource.h"
#include "../Utility/Singleton.h"

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
		// unloads all resources in this group, but it can be reloaded
		void UnloadResourcesInGroup(const string& group);
		// delete all resources in this group
		void ClearGroup(const string& group);
		void DeleteResource(const string& group, const string& name);
		//TODO think about this method. It can be dangerous to release resources with UseCount==1.
		// void UnloadUnusedResources(void);

		void SetLoadingListener(IResourceLoadingListener* listener);

		// name = group name/resource's filename
		ResourcePtr GetResource(const string& groupSlashName);
		ResourcePtr GetResource(const string& group, const string& name);

	private:
		typedef ResourcePtr (*ResourceCreationMethod)();
		typedef std::map<string, ResourcePtr> ResourceMap;
		typedef std::map<string, ResourceMap> ResourceGroupMap;
		typedef std::map<string, Resource::eType> ExtToTypeMap;

		string mBaseDir;
		ResourceGroupMap mResourceGroups;
		ExtToTypeMap mExtToTypeMap;
		IResourceLoadingListener* mListener;
		ResourceCreationMethod mResourceCreationMethods[Resource::NUM_TYPES];
	};
}

#endif