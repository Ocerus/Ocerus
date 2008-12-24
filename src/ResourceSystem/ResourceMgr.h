#ifndef _RESOURCEMGR_H_
#define _RESOURCEMGR_H_

#include <map>
#include <vector>
#include "Resource.h"
#include "XMLResource.h"
#include "../Utility/StringKey.h"
#include "../Utility/Singleton.h"

/// @name Macro for easier use
#define gResourceMgr ResourceSystem::ResourceMgr::GetSingleton()

namespace ResourceSystem
{
	/// @name Forward declarations
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
		~ResourceMgr(void);

		/// @name Assings resources in a directory to a group. In this case the resource types will be autodetected.
		bool AddResourceDirToGroup(const string& path, const StringKey& group, const string& includeRegexp = "*.*", const string& excludeRegexp = "");
		/// @name Assigns a resource to a group.
		bool AddResourceFileToGroup(const string& filepath, const StringKey& group, Resource::eType type = Resource::TYPE_AUTODETECT, bool pathRelative = true);
		/// @name Assigns a resource to a group. Note that if you create the resource this way you must manually delete it later.
		bool AddManualResourceToGroup(const StringKey& name, const StringKey& group, Resource::eType type);
		/** Loads all resources in the specified group.
			Doesn't need to be called, resources are loaded on-the-fly if someone needs them.
		*/
		void LoadResourcesInGroup(const StringKey& group);
		/** Unloads all resources in the specified group, but they can be still reloaded.
			\param allowManual If allowManual is true, manually created resources will be unloaded as well. It is not recommended to do that!
		*/
		void UnloadResourcesInGroup(const StringKey& group, bool allowManual = false);
		/// @name Unloads and then deletes all resources in the specified group. They can't be reloaded.
		void DeleteGroup(const StringKey& group);
		/// @name Unloads and deletes one specific resource.
		void DeleteResource(const StringKey& group, const StringKey& name);
		//TODO think about this method. It can be dangerous to release resources with UseCount==1.
		// void UnloadUnusedResources(void);

		/// @name Unloads all resources from all groups.
		void UnloadAllResources(void);
		/// @name Deletes everything.
		void DeleteAllResources(void);

		/// @name Loading listener receives callbacks from the manager when a resource is being loaded.
		void SetLoadingListener(IResourceLoadingListener* listener);

		/** Retrieves a resource from the manager. If the resource can't be found, null ResourcePtr is returned.
		*/
		//@{
		/// @name @param groupSlashName group name/resource's filename
		ResourcePtr GetResource(char* groupSlashName);
		ResourcePtr GetResource(const StringKey& group, const StringKey& name);
		//@}

		/** Retrieves a specified group of resources from the manager. If the group can't be found,
		    empty vector is returned.
		*/
		void GetResourceGroup(const StringKey& group, std::vector<ResourcePtr>& output);

	private:
		typedef ResourcePtr (*ResourceCreationMethod)();
		typedef std::map<StringKey, ResourcePtr> ResourceMap;
		typedef std::map<StringKey, ResourceMap*> ResourceGroupMap;
		typedef std::map<StringKey, Resource::eType> ExtToTypeMap;

		string mBasePath;
		ResourceGroupMap mResourceGroups;
		ExtToTypeMap mExtToTypeMap;
		IResourceLoadingListener* mListener;
		ResourceCreationMethod mResourceCreationMethods[Resource::NUM_TYPES];

		void AddResourceToGroup(const StringKey& group, const StringKey& name, const ResourcePtr res);
	};
}

#endif