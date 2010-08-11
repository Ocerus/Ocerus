#include "Common.h"
#include <stdio.h>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include "ResourceMgr.h"
#include "IResourceLoadingListener.h"

#include "GfxSystem/Texture.h"
#include "GfxSystem/Mesh.h"
#include "GUISystem/CEGUIResource.h"
#include "StringSystem/TextResource.h"
#include "ResourceSystem/XMLResource.h"
#include "ScriptSystem/ScriptResource.h"
#include "Core/Project.h"
#include "UnknownResource.h"

#ifdef __WIN__
#pragma warning(disable: 4996)
#endif

using namespace ResourceSystem;

ResourceTypeMap ResourceMgr::NullResourceTypeMap;

const uint64 RESOURCE_UPDATES_DELAY_MILLIS = 500;
const uint64 REFRESH_PATH_DELAY_MILIS = 777;

ResourceMgr::ResourceMgr( void ):
	mBasePath(), mListener(0), mResourceUpdatesTimer(false), mMemoryLimit(0), mMemoryUsage(0), mEnforceMemoryLimit(true)
	//RES_NULL_TEXTURE("general/NullTexture.png"), RES_NULL_MODEL("general/NullModel.model")
{
}

void ResourceSystem::ResourceMgr::Init( const string& systemPath )
{
	mBasePath[BPT_SYSTEM] = systemPath;

	ocInfo << "*** ResourceMgr init ***";
	ocInfo << "System directory = " << systemPath;

	mResourceCreationMethods[NUM_RESTYPES-1] = 0; // safety reasons

	// register resource types
	mResourceCreationMethods[RESTYPE_TEXTURE] = GfxSystem::Texture::CreateMe;
	mResourceCreationMethods[RESTYPE_MESH] = GfxSystem::Mesh::CreateMe;
	mResourceCreationMethods[RESTYPE_CEGUIRESOURCE] = GUISystem::CEGUIResource::CreateMe;
	mResourceCreationMethods[RESTYPE_TEXTRESOURCE] = StringSystem::TextResource::CreateMe;
	mResourceCreationMethods[RESTYPE_XMLRESOURCE] = XMLResource::CreateMe;
	mResourceCreationMethods[RESTYPE_SCRIPTRESOURCE] = ScriptSystem::ScriptResource::CreateMe;
	mResourceCreationMethods[RESTYPE_UNKNOWN] = ResourceSystem::UnknownResource::CreateMe;
	mExtToTypeMap["png"] = RESTYPE_TEXTURE;
	mExtToTypeMap["bmp"] = RESTYPE_TEXTURE;
	mExtToTypeMap["jpg"] = RESTYPE_TEXTURE;
	mExtToTypeMap["gif"] = RESTYPE_TEXTURE;
	mExtToTypeMap["model"] = RESTYPE_MESH;
	mExtToTypeMap["layout"] = RESTYPE_CEGUIRESOURCE;
	mExtToTypeMap["imageset"] = RESTYPE_CEGUIRESOURCE;
	mExtToTypeMap["font"] = RESTYPE_CEGUIRESOURCE;
	mExtToTypeMap["scheme"] = RESTYPE_CEGUIRESOURCE;
	mExtToTypeMap["tga"] = RESTYPE_TEXTURE;
	mExtToTypeMap["ttf"] = RESTYPE_CEGUIRESOURCE;
	mExtToTypeMap["looknfeel"] = RESTYPE_CEGUIRESOURCE;
	mExtToTypeMap["str"] = RESTYPE_TEXTRESOURCE;
	mExtToTypeMap["xml"] = RESTYPE_XMLRESOURCE;
	mExtToTypeMap["as"] = RESTYPE_SCRIPTRESOURCE;

	OC_ASSERT_MSG(mResourceCreationMethods[NUM_RESTYPES-1], "Not all resource types are registered");

	mLastResourceRefreshTime = 0;
	mLastPathRefreshTime = 0;
	mMemoryLimit = std::numeric_limits<size_t>::max();
	mMemoryUsage = 0;
	Resource::ResetLastUsedTime();

	ocInfo << "All resource types registered";
}

ResourceMgr::~ResourceMgr()
{
	DeleteAllResources();
	OC_ASSERT_MSG(mMemoryUsage==0, "Seems like we didn't unload some resources");
}

void ResourceMgr::UnloadAllResources()
{
	for (ResourceGroupMap::const_iterator i=mResourceGroups.begin(); i!=mResourceGroups.end(); ++i)
	{
		UnloadResourcesInGroup(i->first);
	}
	Resource::ResetLastUsedTime();
}

bool ResourceMgr::AddResourceDirToGroup(const eBasePathType basePathType, const string& path, const StringKey& group, const string& includeRegexp, const string& excludeRegexp, eResourceType type, const ResourceTypeMap& resourceTypeMap, bool recursive)
{
	boost::filesystem::path boostPath;
	if (basePathType == BPT_ABSOLUTE) 
		boostPath = path;
	else 
		boostPath = mBasePath[basePathType] + path;

	ocInfo << "Adding dir '" << boostPath << "' to group '" << group << "'";

	// check the path
	if (!boost::filesystem::exists(boostPath))
	{
		ocError << "Path does not exist '" << boostPath.string() << "'";
		return false;
	}

	// if the group is empty, add at least something so that it actually exists
	if (mResourceGroups.find(group) == mResourceGroups.end())
	{
		mResourceGroups[group] = new ResourceMap();
	}

	boost::regex includeFilter;
	boost::regex excludeFilter;

	try
	{
		includeFilter.assign(includeRegexp, boost::regex::extended|boost::regex::icase);
	}
	catch (std::exception& e)
	{
		ocError << "includeRegexp '" << includeRegexp << "' is not valid: " << e.what();
		return false;
	}
	try
	{
		if (excludeRegexp.size()>0) excludeFilter.assign(excludeRegexp, boost::regex::extended|boost::regex::icase);
	}
	catch (std::exception& e)
	{
		ocError << "excludeRegexp '" << excludeRegexp << "' is not valid: " << e.what();
		return false;
	}

	bool result = true;

	boost::filesystem::directory_iterator iend;
	for (boost::filesystem::directory_iterator i(boostPath); i!=iend; ++i)
	{
		string filePath = i->path().string();
		string relativePath = filePath.substr(mBasePath[basePathType].length());
		ResourceTypeMap::const_iterator it = resourceTypeMap.find(relativePath);
		eResourceType resourceType = (it != resourceTypeMap.end()) ? it->second : type;

		if (boost::filesystem::is_directory(i->status()))
		{
			string dirStr = i->path().filename();
			if (recursive && dirStr.compare(".svn") != 0)
			{
				if (!AddResourceDirToGroup(basePathType, relativePath, group, includeRegexp, excludeRegexp, resourceType, resourceTypeMap))
				{
					result = false;
				}
			}
		}
		else if (regex_match(filePath, includeFilter) && (excludeFilter.empty() || !regex_match(filePath, excludeFilter)))
		{
			if (i->path().filename().compare(Core::Project::PROJECT_FILE_NAME) != 0)
			{
				if (!AddResourceFileToGroup(relativePath, group, resourceType, basePathType))
				{
					result = false;
				}
			}
		}
	}

	return result;
}

bool ResourceMgr::AddResourceFileToGroup(const string& filepath, const StringKey& group, eResourceType type, const eBasePathType basePathType)
{
	boost::filesystem::path boostPath;
	
	if (basePathType == BPT_ABSOLUTE)
		boostPath = filepath;
	else
		boostPath = mBasePath[basePathType] + filepath;

	string dirStr = boostPath.filename();
	if (dirStr.compare("Thumbs.db") == 0)
	{
		ocInfo << "Resource '" << boostPath << "' ignored";
		return false;
	}
	
	ocTrace << "Adding resource '" << boostPath << "' to group '" << group << "'";
	if (!boost::filesystem::exists(boostPath))
	{
		ocWarning << "Resource located at '" << boostPath.string() << "' not found";
		return false;
	}

	// detect resource type
	if (type == RESTYPE_AUTODETECT)
	{
		string extension = !boostPath.extension().empty() ? boostPath.extension().substr(1) : "";
		ExtToTypeMap::const_iterator i = mExtToTypeMap.find(extension);
		if (i != mExtToTypeMap.end())
			type = i->second;
	}
	// error
	if (type == RESTYPE_AUTODETECT)
	{
		type = RESTYPE_UNKNOWN;
	}

	string name = filepath; //boostPath.filename();
	ResourceGroupMap::const_iterator groupIt = mResourceGroups.find(group);
	if (mResourceGroups.find(group) != mResourceGroups.end() && groupIt->second->find(name) != groupIt->second->end())
	{
		// resource already exists
		return false;
	}

	ResourcePtr r = mResourceCreationMethods[type]();
	r->SetState(Resource::STATE_INITIALIZED);
	r->SetName(name);
	r->SetGroup(group);
	r->SetFilePath(boostPath.string());
	r->SetType(type);
	r->SetBasePathType(basePathType);

	AddResourceToGroup(group, name, r);

	ocInfo << "Resource '" << name << "' added to group '" << group << "'.";
	return true;
}

bool ResourceSystem::ResourceMgr::AddManualResourceToGroup( const StringKey& name, const StringKey& group, eResourceType type )
{
    ocTrace << "Manually adding resource '" << name << "' to group '" << group << "'";

	OC_ASSERT_MSG(type != RESTYPE_AUTODETECT, "Must specify resource type when creating it manually");

	ResourcePtr r = mResourceCreationMethods[type]();
	r->SetState(Resource::STATE_INITIALIZED);
	r->SetName(name.ToString());
	r->SetGroup(group);
	r->SetManual(true);
	r->SetBasePathType(BPT_ABSOLUTE);

	AddResourceToGroup(group, name, r);

	ocInfo << "Resource '" << name << "' added";
	return true;
}

void ResourceSystem::ResourceMgr::AddResourceToGroup( const ResourceGroupMap::iterator& groupIt, const StringKey& name, const ResourcePtr res )
{
	(*groupIt->second)[name] = res;
}

void ResourceSystem::ResourceMgr::AddResourceToGroup( const StringKey& group, const StringKey& name, const ResourcePtr res )
{
	ResourceGroupMap::iterator groupIt = mResourceGroups.find(group);
	if (groupIt == mResourceGroups.end())
	{
		mResourceGroups[group] = new ResourceMap();
		groupIt = mResourceGroups.find(group);
	}
	AddResourceToGroup(groupIt, name, res);
}

bool ResourceMgr::RefreshBasePathToGroup(const eBasePathType basePathType, const StringKey& group)
{
	OC_ASSERT(basePathType != BPT_ABSOLUTE);

	if (mBasePath[basePathType].empty())
		return false;

	return RefreshPathToGroup(mBasePath[basePathType], basePathType, group);
}

bool ResourceMgr::RefreshPathToGroup(const string& path, const eBasePathType basePathType, const StringKey& group)
{
	boost::filesystem::path boostPath = path;

	bool result = false;

	// check the path
	if (!boost::filesystem::exists(boostPath))
	{
		ocError << "Path does not exist '" << boostPath.string() << "'";
	}

	// if the group is empty, add at least something so that it actually exists
	if (mResourceGroups.find(group) == mResourceGroups.end())
	{
		mResourceGroups[group] = new ResourceMap();
	}

	boost::filesystem::directory_iterator iend;
	for (boost::filesystem::directory_iterator i(boostPath); i!=iend; ++i)
	{
		string filePath = i->path().string();
		string relativePath = filePath.substr(mBasePath[basePathType].length());
		if (boost::filesystem::is_directory(i->status()))
		{
			string dirStr = i->path().filename();
			if (dirStr.compare(".svn") != 0)
			{
				if (RefreshPathToGroup(filePath, basePathType, group))
					result = true;
			}
		}
		else if (i->path().filename().compare(Core::Project::PROJECT_FILE_NAME) != 0)
		{
			if (AddResourceFileToGroup(relativePath, group, RESTYPE_AUTODETECT, basePathType))
				result = true;
		}
	}
	return result;
}

void ResourceMgr::LoadResourcesInGroup(const StringKey& group)
{
	ocTrace << "Loading resource group '" << group << "'";

	ResourceGroupMap::const_iterator gi = mResourceGroups.find(group);

	if (gi==mResourceGroups.end())
	{
		ocError << "Unknown group '" << group << "'";
		return;
	}

	const ResourceMap& resmap = *gi->second;
	if (mListener) mListener->ResourceGroupLoadStarted(group.ToString(), static_cast<uint32>(resmap.size()));
	for (ResourceMap::const_iterator ri = resmap.begin(); ri != resmap.end(); ++ri)
	{
		if (ri->second->GetState() == Resource::STATE_INITIALIZED)
		{
			// callbacks to the listener are handled in the Load() method
			ri->second->Load();
		}
	}
	if (mListener) mListener->ResourceGroupLoadEnded();

	ocTrace << "Resource group loaded '" << group << "'";
}

void ResourceMgr::UnloadResourcesInGroup(const StringKey& group, bool allowManual)
{
	ocTrace << "Unloading resource group '" << group << "'";

	ResourceGroupMap::const_iterator gi = mResourceGroups.find(group);

	if (gi==mResourceGroups.end())
	{
		ocError << "Unknown group '" << group << "'";
		return;
	}

	const ResourceMap& resmap = *gi->second;
	for (ResourceMap::const_iterator ri = resmap.begin(); ri != resmap.end(); ++ri)
		if (ri->second->GetState() >= Resource::STATE_LOADING)
			ri->second->Unload(allowManual);

	ocTrace << "Resource group '" << group << "' unloaded";
}

void ResourceMgr::DeleteGroup(const StringKey& group)
{
	ResourceGroupMap::iterator groupIt = mResourceGroups.find(group);
	if (groupIt == mResourceGroups.end())
		return;
	UnloadResourcesInGroup(group, true);
	delete groupIt->second;
	mResourceGroups.erase(groupIt);
}

void ResourceMgr::DeleteProjectResources()
{
	DeleteGroup("Project");
	SetBasePath(BPT_PROJECT, "");
}

void ResourceMgr::SetLoadingListener(IResourceLoadingListener* listener)
{
	mListener = listener;
}

ResourcePtr ResourceMgr::GetResource(const StringKey& group, const StringKey& name)
{
	ResourceGroupMap::const_iterator gi = mResourceGroups.find(group);
	if (gi == mResourceGroups.end()){
		ocWarning << "Resource group '" << group << "' not found";
		return ResourcePtr(); // null
	}
	const ResourceMap& resmap = *gi->second;
	ResourceMap::const_iterator ri = resmap.find(name);
	if (ri == resmap.end())
	{
		ocWarning << "Resource '" <<  name << "' in group '" << group << "' not found";
		return ResourcePtr(); // null
	}
	return ri->second;
}

void ResourceMgr::GetResourceGroup(const StringKey& group, vector<ResourcePtr>& output)
{
	ResourceGroupMap::const_iterator gi = mResourceGroups.find(group);
	if (gi == mResourceGroups.end())
		return; // empty vector
	const ResourceMap& resmap = *gi->second;
	output.reserve(resmap.size());
	for (ResourceMap::const_iterator ri = resmap.begin(); ri != resmap.end(); ri++)
	{
		output.push_back(ri->second);
	}
	return;
}

void ResourceMgr::GetResources(vector<ResourcePtr>& output, eBasePathType basePathType)
{
	for (ResourceGroupMap::const_iterator gi = mResourceGroups.begin(); gi != mResourceGroups.end(); ++gi)
	{
		const ResourceMap& resmap = *gi->second;
		output.reserve(output.size() + resmap.size());
		for (ResourceMap::const_iterator ri = resmap.begin(); ri != resmap.end(); ri++)
		{
			if (ri->second->GetBasePathType() == basePathType)
				output.push_back(ri->second);
		}
	}
}

void ResourceSystem::ResourceMgr::DeleteResource( const StringKey& group, const StringKey& name )
{
	ocTrace << "Deleting resource '" << name << "' in group '" << group << "'";

	ResourceGroupMap::iterator gi = mResourceGroups.find(group);

	if (gi==mResourceGroups.end())
	{
		ocError << "Unknown group '" << group << "'";
		return;
	}

	ResourceMap& resmap = *gi->second;
	ResourceMap::iterator ri = resmap.find(name);
	if (ri==resmap.end())
	{
		ocError << "Unknown resource '" << name << "' in group '" << group << "'";
		return;
	}
	ri->second->Unload(true);
	resmap.erase(ri);

	ocInfo << "Resource deleted";
}

ResourceSystem::ResourcePtr ResourceSystem::ResourceMgr::GetResource( const char* groupSlashName )
{
	const char* lastSlashPos = 0;
	const char* str = groupSlashName;
	for(; *str; ++str)
	{
		if (*str == '/') lastSlashPos = str;
	}
	if (!lastSlashPos) return ResourcePtr();
	return GetResource(StringKey(groupSlashName, lastSlashPos-groupSlashName), StringKey(lastSlashPos+1, str-lastSlashPos-1));
}

void ResourceSystem::ResourceMgr::DeleteAllResources( void )
{
	for (ResourceGroupMap::iterator i=mResourceGroups.begin(); i!=mResourceGroups.end(); ++i)
	{
		UnloadResourcesInGroup(i->first, true);
		delete i->second;
	}
	mResourceGroups.clear();
}

bool ResourceSystem::ResourceMgr::RefreshAllResources( void )
{
	bool result = false;
	for (ResourceGroupMap::iterator groupIter=mResourceGroups.begin(); groupIter!=mResourceGroups.end(); ++groupIter)
	{
		ResourceMap* resMap = groupIter->second;
		OC_ASSERT(resMap);
		for (ResourceMap::iterator resIter=resMap->begin(); resIter!=resMap->end(); ++resIter)
		{
			if (!resIter->second->Refresh())
			{
				resIter->second->Unload(true);
				resMap->erase(resIter);
				result = true;
			}
		}
	}
	return result;
}

void ResourceSystem::ResourceMgr::RefreshAllTextures( void )
{
	for (ResourceGroupMap::iterator groupIter=mResourceGroups.begin(); groupIter!=mResourceGroups.end(); ++groupIter)
	{
		ResourceMap* resMap = groupIter->second;
		OC_ASSERT(resMap);
		for (ResourceMap::iterator resIter=resMap->begin(); resIter!=resMap->end(); ++resIter)
		{
			ResourcePtr res = resIter->second;
			if ((res->GetType() == RESTYPE_TEXTURE) && (res->GetState() == Resource::STATE_LOADED))
				resIter->second->Reload();
		}
	}
}

void ResourceSystem::ResourceMgr::ChangeResourceType(ResourcePtr resPointer, eResourceType newType)
{
	ocInfo << "Changing type of resource " << resPointer->GetName() << " to " << GetResourceTypeName(newType);

	// searching throught the groups
	for (ResourceGroupMap::iterator groupIter=mResourceGroups.begin(); groupIter!=mResourceGroups.end(); ++groupIter)
	{
		ResourceMap* resMap = groupIter->second;
		OC_ASSERT(resMap);
		// searching throught the resources in the group
		for (ResourceMap::iterator resIter=resMap->begin(); resIter!=resMap->end(); ++resIter)
		{
			if (resIter->second == resPointer)
			{
				// creating a resource with the new type
				ResourcePtr r = mResourceCreationMethods[newType]();
				r->SetState(Resource::STATE_INITIALIZED);
				r->SetName(resPointer->GetName());
				r->SetGroup(resPointer->GetGroup());
				r->SetFilePath(resPointer->GetFilePath());
				r->SetType(newType);
				r->SetBasePathType(resPointer->GetBasePathType());

				// deleting old resource
				resIter->second->Unload(true);
				resMap->erase(resIter);

				// adding new resource to manager
				AddResourceToGroup(groupIter, r->GetName(), r);
				return;
			}
		}
	}
	ocError << "Can't find resource '" << resPointer->GetName() << "'!!!";
}

bool ResourceSystem::ResourceMgr::CheckForResourcesUpdates( void )
{
	PROFILE_FNC();

	uint64 currentTime = mResourceUpdatesTimer.GetMilliseconds();
	if (currentTime - mLastResourceRefreshTime >= RESOURCE_UPDATES_DELAY_MILLIS)
	{
		mLastResourceRefreshTime = currentTime;
		return RefreshAllResources();
	}
	return false;
}

bool ResourceSystem::ResourceMgr::CheckForRefreshPath( void )
{
	PROFILE_FNC();

	uint64 currentTime = mResourceUpdatesTimer.GetMilliseconds();
	if (currentTime - mLastPathRefreshTime >= REFRESH_PATH_DELAY_MILIS)
	{
		mLastPathRefreshTime = currentTime;
		return RefreshBasePathToGroup(ResourceSystem::BPT_PROJECT, "Project");
	}
	return false;
}

void ResourceSystem::ResourceMgr::_NotifyResourceLoaded( const Resource* loadedResource )
{
	if (mListener) mListener->ResourceLoadEnded();
	mMemoryUsage += loadedResource->GetSize();
	CheckMemoryUsage(loadedResource);
}

void ResourceSystem::ResourceMgr::_NotifyResourceUnloaded( const Resource* unloadedResource )
{
	mMemoryUsage -= unloadedResource->GetSize();
}

void ResourceSystem::ResourceMgr::_NotifyResourceLoadingStarted( const Resource* loadingResource )
{
	if (mListener) mListener->ResourceLoadStarted(loadingResource);
}

void ResourceSystem::ResourceMgr::CheckMemoryUsage( const Resource* resourceToKeep )
{
	if (!mEnforceMemoryLimit) return;

	while (mMemoryUsage > mMemoryLimit)
	{
		// find a resource to unload using the LRU strategy
		ResourcePtr leastUsedResource(0);
		uint64 leastUsedTime = std::numeric_limits<uint64>::max();

		for (ResourceGroupMap::iterator gi=mResourceGroups.begin(); gi!=mResourceGroups.end(); ++gi)
		{
			ResourceMap& resmap = *gi->second;
			for (ResourceMap::iterator ri=resmap.begin(); ri!=resmap.end(); ++ri)
			{
				ResourcePtr testedResource = ri->second;
				bool notToKeep = testedResource.get() != resourceToKeep;
				bool loaded = testedResource->GetState() == Resource::STATE_LOADED;
				bool lessUpdated = testedResource->GetLastUsedTime() < leastUsedTime;
				if (notToKeep && loaded && lessUpdated)
				{
					leastUsedResource = testedResource;
					leastUsedTime = testedResource->GetLastUsedTime();
				}
			}
		}

		// nothing to unload
		if (!leastUsedResource) break;

		ocInfo << "Unloading resource '" << leastUsedResource->GetName() << "' to stay under memory limits";
		leastUsedResource->Unload();
	}
}

void ResourceSystem::ResourceMgr::SetMemoryLimit( const size_t newLimit )
{
	mMemoryLimit = newLimit;
	CheckMemoryUsage();
}

void ResourceSystem::ResourceMgr::EnableMemoryLimitEnforcing( void )
{
	mEnforceMemoryLimit = true;
	CheckMemoryUsage();
}