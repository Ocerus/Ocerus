#ifndef _RESOURCEMGR_H_
#define _RESOURCEMGR_H_

#include <map>
#include "Resource.h"
#include "../Utility/Singleton.h"

/// Macro for easier use
#define gResourceMgr ResourceSystem::ResourceMgr::GetSingleton()

namespace ResourceSystem
{
	/// Forward declarations
	//@{
	class IResourceLoadingListener;
	//@}

	/** This class manages all resources of the program. A resource is everything what is loaded from disk or any
		other persistent medium (or even network stream). Resources are coupled into groups for easier manipulation.
	*/
	class ResourceMgr : public Singleton<ResourceMgr>
	{
	public:
		/** All resource types must be registered inside this ctor, so don't forget to add new types there!
			Basepath is a path to directory where all resources can be found if readed from disk.
		*/
		ResourceMgr(const string& basepath);
		~ResourceMgr();

		/// Assings resources in a directory to a group. In this case the resource types will be autodetected.
		bool AddResourceDirToGroup(const string& path, const string& group, const string& includeRegexp = "*.*", const string& excludeRegexp = "");
		/// Assigns a resource to a group.
		bool AddResourceFileToGroup(const string& filepath, const string& group, Resource::eType type = Resource::TYPE_AUTODETECT, bool pathRelative = true);
		/// Assigns a resource to a group.
		bool AddResourceMemoryToGroup(void* memoryLocation, uint32 memoryLength, const string& name, const string& group, Resource::eType type = Resource::TYPE_AUTODETECT);
		/** Loads all resources in the specified group.
			Doesn't need to be called, resources are loaded on-the-fly if someone needs them.
		*/
		void LoadResourcesInGroup(const string& group);
		/// Unloads all resources in the specified group, but they can be still reloaded.
		void UnloadResourcesInGroup(const string& group);
		/// Unloads and then deletes all resources in the specified group. They can't be reloaded.
		void ClearGroup(const string& group);
		/// Unloads and deletes one specific resource.
		void DeleteResource(const string& group, const string& name);
		//TODO think about this method. It can be dangerous to release resources with UseCount==1.
		// void UnloadUnusedResources(void);

		void SetLoadingListener(IResourceLoadingListener* listener);

		/** Retrieves a resource from the manager. If the resource can't be found, null ResourcePtr is returned.
		*/
		//@{
		/// @param groupSlashName group name/resource's filename
		ResourcePtr GetResource(const string& groupSlashName);
		ResourcePtr GetResource(const string& group, const string& name);
		//@}

	private:
		typedef ResourcePtr (*ResourceCreationMethod)();
		typedef std::map<string, ResourcePtr> ResourceMap;
		typedef std::map<string, ResourceMap> ResourceGroupMap;
		typedef std::map<string, Resource::eType> ExtToTypeMap;

		string mBasePath;
		ResourceGroupMap mResourceGroups;
		ExtToTypeMap mExtToTypeMap;
		IResourceLoadingListener* mListener;
		ResourceCreationMethod mResourceCreationMethods[Resource::NUM_TYPES];
	};
}

#endif