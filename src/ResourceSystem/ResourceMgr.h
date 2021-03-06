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
	/// This class manages all resources of the program. A resource is everything what is loaded from disk or any
	///	other persistent medium (or even network stream).
	/// Resources are coupled into groups for easier manipulation. Resources are unloaded and reloaded
	/// as the system sees fit. There are different states a resource can be in. It can either not exist
	/// in the system, or it can exist in the system but not loaded, or it can be loaded in the memory.
	class ResourceMgr : public Singleton<ResourceMgr>
	{
	public:

		static ResourceTypeMap NullResourceTypeMap;

		/// Default constructor.
		ResourceMgr(void);

		/// Default destructor.
		~ResourceMgr(void);

		/// All resource types must be registered inside this ctor, so don't forget to add new types there!
		/// @param systemPath Path to System directory where all System resources can be found if readed from disk.
		void Init(const string& systemPath);

		/// Assings resources in a directory to a group. In this case the resource types will be autodetected.
		/// The regular expression use the syntax as defined here:
		/// http://www.boost.org/doc/libs/1_40_0/libs/regex/doc/html/boost_regex/syntax/basic_extended.html
		/// The matching is not case sensitive!
		/// @param basePathType Type of base path to which is the "path" parameter related.
		/// @param resourceType Type of resources in the directory.
		bool AddResourceDirToGroup(const eBasePathType basePathType, const string& path, const StringKey& group,
			const string& includeRegexp = ".*", const string& excludeRegexp = "", eResourceType resourceType =
			RESTYPE_AUTODETECT, const ResourceTypeMap& resourceTypeMap = NullResourceTypeMap, bool recursive = true);

		/// Assigns a resource to a group.
		/// The resource type if autodetected if you don't specify it.
		/// Returns true if the resource was successfully added.
		bool AddResourceFileToGroup(const string& filepath, const StringKey& group, eResourceType type = RESTYPE_AUTODETECT, const eBasePathType basePathType = BPT_SYSTEM);

		/// Assigns a resource to a group. Note that if you create the resource this way you must manually delete it later.
		bool AddManualResourceToGroup(const StringKey& name, const StringKey& group, eResourceType type);

		/// Refreshes given base path, resources are added to given group.
		/// Return true if something was added.
		bool RefreshBasePathToGroup(const eBasePathType basePathType, const StringKey& group);

		/// Loads all resources in the specified group.
		/// It doesn't need to be called, resources are loaded on-the-fly if someone needs them. But it's
		/// always better to preload them.
		void LoadResourcesInGroup(const StringKey& group);

		/// Unloads all resources in the specified group, but they can be still reloaded.
		///	@param allowManual If true, manually created resources will be unloaded as well. It is not recommended to do that!
		void UnloadResourcesInGroup(const StringKey& group, bool allowManual = false);

		/// Unloads and then deletes all resources in the specified group. They can't be reloaded.
		void DeleteGroup(const StringKey& group);

		/// Unloads and then deletes all resources in project. They can't be reloaded.
		void DeleteProjectResources();

		/// Unloads and deletes one specific resource. The resource can't be reloaded.
		void DeleteResource(const StringKey& group, const StringKey& name);

		/// Unloads all resources from all groups. Resources can be still reloaded.
		void UnloadAllResources(void);

		/// Deletes everything. Nothing can be reloaded.
		void DeleteAllResources(void);

		/// Makes sure all loaded resources are up to date.
		/// Return true if some resource file has been deleted and thus resource unloaded and deleted.
		bool RefreshAllResources(void);

		/// Reloads all textures. Needed when recreating drawing context.
		void RefreshAllTextures(void);

		/// Renames the given resource. It renames the file on the disk as well if it belongs to the resource.
		void RenameResource(ResourcePtr res, const string& newName, const string& newFilePath);

		/// Performs a periodic test on resources to determine if they're up to date. The function is non-blocking.
		/// The function is meant to be called each frame.
		/// Returns true, if some resource was deleted.
		bool CheckForResourcesUpdates(void);
		
		/// Performs a periodic test on resource path to determine if some resource has been added. 
		/// The function is non-blocking. The function is meant to be called each frame.
		/// Returns true, if something was added.
		bool CheckForRefreshPath(void);

		/// Loading listener receives callbacks from the manager when a resource is being loaded.
		void SetLoadingListener(IResourceLoadingListener* listener);

		/// Returns true if the resource exists in the manager.
		bool ResourceExists(const StringKey& group, const StringKey& name);

		/// Retrieves a resource from the manager. If the resource can't be found, null ResourcePtr is returned.
		/// @param groupSlashName Full name of the resource including group name/resource's filename.
		ResourcePtr GetResource(const char* groupSlashName);

		/// Retrieves a resource from the manager. If the resource can't be found, null ResourcePtr is returned.
		ResourcePtr GetResource(const StringKey& group, const StringKey& name);

		/// Retrieves a specified group of resources from the manager.
		/// If the group can't be found, empty vector is returned.
		void GetResourceGroup(const StringKey& group, vector<ResourcePtr>& output);

		/// Retrieves all resources in given path.
		void GetResources(vector<ResourcePtr>& output, eBasePathType basePathType, const string& path = "", bool recursive = true);

		/// Changes the type of the resource to a new one. Returns pointer to the new resource.
		ResourcePtr ChangeResourceType(ResourcePtr resPointer, eResourceType newType);

		/// Sets the memory limit the resource manager should try to keep.
		/// The limit is given in bytes.
		void SetMemoryLimit(const size_t newLimit);

		/// Returns the current memory limit.
		inline size_t GetMemoryLimit(void) const { return mMemoryLimit; }

		/// Enables unloading of resources when they're over the memory limit.
		void EnableMemoryLimitEnforcing(void);

		/// Enables unloading of resources when they're over the memory limit.
		inline void DisableMemoryLimitEnforcing(void) { mEnforceMemoryLimit = false; }

		/// Gets base path of given type.
		inline const string& GetBasePath(const eBasePathType pathType) const { return mBasePath[pathType]; }
		
		/// Sets new base path of given type.
		inline void SetBasePath(const eBasePathType newPathType, const string& newPath) { mBasePath[newPathType] = newPath; }

	public:

		/// Callback from a resource after it was loaded.
		void _NotifyResourceLoaded(const Resource* loadedResource);

		/// Callback from a resource after it was unloaded.
		void _NotifyResourceUnloaded(const Resource* unloadedResource);

		/// Callback from a resource before it was loaded.
		void _NotifyResourceLoadingStarted(const Resource* loadingResource);

	private:

		typedef ResourcePtr (*ResourceCreationMethod)();
		typedef map<StringKey, ResourcePtr> ResourceMap;
		typedef map<StringKey, ResourceMap*> ResourceGroupMap;
		typedef map<StringKey, eResourceType> ExtToTypeMap;

		string mBasePath[NUM_BASEPATHTYPES];
		ResourceGroupMap mResourceGroups;
		ExtToTypeMap mExtToTypeMap;
		IResourceLoadingListener* mListener;
		ResourceCreationMethod mResourceCreationMethods[NUM_RESTYPES];
		Utils::Timer mResourceUpdatesTimer;
		uint64 mLastResourceRefreshTime;
		uint64 mLastPathRefreshTime;
		size_t mMemoryLimit;
		size_t mMemoryUsage;
		bool mEnforceMemoryLimit;

		/// Adds a resource to a group given by iterator.
		void AddResourceToGroup(const ResourceGroupMap::iterator& groupIt, const StringKey& name, const ResourcePtr res);
		
		/// Adds a resource to a group given a resource pointer.
		void AddResourceToGroup(const StringKey& group, const StringKey& name, const ResourcePtr res);

		/// Refreshes given path, resources are added to given group.
		/// Returns true if something was added.
		bool RefreshPathToGroup(const string& path, const eBasePathType basePathType, const StringKey& group);

		/// Checks if the memory usage is within limits. If not, some of the resources will be freed.
		/// @param resourceToKeep This resource (if valid) will be preserved at any case.
		void CheckMemoryUsage(const Resource* resourceToKeep = 0);
	};
}

#endif
