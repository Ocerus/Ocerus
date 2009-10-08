/// @file
/// Entry point to the ResourceSystem.

#ifndef _RESOURCEMGR_H_
#define _RESOURCEMGR_H_

#include "Base.h"
#include "Singleton.h"
#include "ResourceTypes.h"

/// Macro for easier use
#define gResourceMgr ResourceSystem::ResourceMgr::GetSingleton()

/// %Resource system manages all data which has to be loaded from files or other external storage media.
namespace ResourceSystem
{
	/// @brief This class manages all resources of the program. A resource is everything what is loaded from disk or any
	///	other persistent medium (or even network stream).
	/// @remarks Resources are coupled into groups for easier manipulation. Resources are unloaded and reloaded as the system sees fit. There are different states a resource can be in. It can either not exist in the system, or it can
	/// exist in the system but not loaded, or it can be loaded in the memory.
	class ResourceMgr : public Singleton<ResourceMgr>
	{
	public:

		ResourceMgr(void);
		~ResourceMgr(void);

		/// @brief All resource types must be registered inside this ctor, so don't forget to add new types there!
		/// @param basepath Path to directory where all resources can be found if readed from disk.
		void Init(const string& basepath);

		/// Assings resources in a directory to a group. In this case the resource types will be autodetected.
		bool AddResourceDirToGroup(const string& path, const StringKey& group, const string& includeRegexp = "*.*", const string& excludeRegexp = "");

		/// @brief Assigns a resource to a group.
		/// @remarks The resource type if autodetected if you don't specify it.
		bool AddResourceFileToGroup(const string& filepath, const StringKey& group, eResourceType type = RESTYPE_AUTODETECT, bool pathRelative = true);

		/// Assigns a resource to a group. Note that if you create the resource this way you must manually delete it later.
		bool AddManualResourceToGroup(const StringKey& name, const StringKey& group, eResourceType type);

		/// @brief Loads all resources in the specified group.
		/// @remarks It doesn't need to be called, resources are loaded on-the-fly if someone needs them. But it's
		/// always better to preload them.
		void LoadResourcesInGroup(const StringKey& group);

		/// @brief Unloads all resources in the specified group, but they can be still reloaded.
		///	@param allowManual If true, manually created resources will be unloaded as well. It is not recommended to do that!
		void UnloadResourcesInGroup(const StringKey& group, bool allowManual = false);

		/// Unloads and then deletes all resources in the specified group. They can't be reloaded.
		void DeleteGroup(const StringKey& group);

		/// Unloads and deletes one specific resource. The resource can't be reloaded.
		void DeleteResource(const StringKey& group, const StringKey& name);

		/// Unloads all resources from all groups. Resources can be still reloaded.
		void UnloadAllResources(void);

		/// Deletes everything. Nothing can be reloaded.
		void DeleteAllResources(void);

		/// Loading listener receives callbacks from the manager when a resource is being loaded.
		void SetLoadingListener(IResourceLoadingListener* listener);

		/// @brief Retrieves a resource from the manager. If the resource can't be found, null ResourcePtr is returned.
		/// @param groupSlashName Full name of the resource including group name/resource's filename
		ResourcePtr GetResource(char* groupSlashName);

		/// @brief Retrieves a resource from the manager. If the resource can't be found, null ResourcePtr is returned.
		ResourcePtr GetResource(const StringKey& group, const StringKey& name);

		/// @brief Retrieves a specified group of resources from the manager.
		/// @remarks If the group can't be found, empty vector is returned.
		void GetResourceGroup(const StringKey& group, vector<ResourcePtr>& output);

	private:

		typedef ResourcePtr (*ResourceCreationMethod)();
		typedef map<StringKey, ResourcePtr> ResourceMap;
		typedef map<StringKey, ResourceMap*> ResourceGroupMap;
		typedef map<StringKey, eResourceType> ExtToTypeMap;

		string mBasePath;
		ResourceGroupMap mResourceGroups;
		ExtToTypeMap mExtToTypeMap;
		IResourceLoadingListener* mListener;
		ResourceCreationMethod mResourceCreationMethods[NUM_RESTYPES];

		void AddResourceToGroup(const StringKey& group, const StringKey& name, const ResourcePtr res);
	};
}

#endif