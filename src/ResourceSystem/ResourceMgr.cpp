#include "Common.h"
#include <stdio.h>
#include <boost/filesystem.hpp>
#include "ResourceMgr.h"
#include "IResourceLoadingListener.h"

#include "../GfxSystem/Texture.h"
#include "../GUISystem/CEGUIResource.h"
#include "../StringSystem/TextResource.h"
#include "../GfxSystem/ParticleResource.h"
#include "../ResourceSystem/XMLResource.h"

#pragma warning(disable: 4996)

using namespace ResourceSystem;

ResourceMgr::ResourceMgr( void ): 
	mListener(0), mBasePath()
{

}

void ResourceSystem::ResourceMgr::Init( const string& basepath )
{
	mBasePath = basepath;

	gLogMgr.LogMessage("*** ResourceMgr init ***");
	gLogMgr.LogMessage("Base directory = ", basepath);

	mResourceCreationMethods[Resource::NUM_TYPES-1] = 0; // safety reasons

	// register resource types
	mResourceCreationMethods[Resource::TYPE_TEXTURE] = GfxSystem::Texture::CreateMe;
	mResourceCreationMethods[Resource::TYPE_CEGUIRESOURCE] = GUISystem::CEGUIResource::CreateMe;
	mResourceCreationMethods[Resource::TYPE_TEXTRESOURCE] = StringSystem::TextResource::CreateMe;
	mResourceCreationMethods[Resource::TYPE_XMLRESOURCE] = XMLResource::CreateMe;
	mResourceCreationMethods[Resource::TYPE_PARTICLERESOURCE] = GfxSystem::ParticleResource::CreateMe;
	mExtToTypeMap["png"] = Resource::TYPE_TEXTURE;
	mExtToTypeMap["bmp"] = Resource::TYPE_TEXTURE;
	mExtToTypeMap["jpg"] = Resource::TYPE_TEXTURE;
	mExtToTypeMap["gif"] = Resource::TYPE_TEXTURE;
	mExtToTypeMap["layout"] = Resource::TYPE_CEGUIRESOURCE;
	mExtToTypeMap["imageset"] = Resource::TYPE_CEGUIRESOURCE;
	mExtToTypeMap["font"] = Resource::TYPE_CEGUIRESOURCE;
	mExtToTypeMap["scheme"] = Resource::TYPE_CEGUIRESOURCE;
	mExtToTypeMap["tga"] = Resource::TYPE_TEXTURE;
	mExtToTypeMap["ttf"] = Resource::TYPE_CEGUIRESOURCE;
	mExtToTypeMap["looknfeel"] = Resource::TYPE_CEGUIRESOURCE;
	mExtToTypeMap["str"] = Resource::TYPE_TEXTRESOURCE;
	mExtToTypeMap["xml"] = Resource::TYPE_XMLRESOURCE;
	mExtToTypeMap["psi"] = Resource::TYPE_PARTICLERESOURCE;

	BS_ASSERT_MSG(mResourceCreationMethods[Resource::NUM_TYPES-1], "Not all resource types are registered");

	gLogMgr.LogMessage("All resource types registered");
}

ResourceMgr::~ResourceMgr() 
{
	DeleteAllResources();
}

void ResourceMgr::UnloadAllResources()
{
	for (ResourceGroupMap::const_iterator i=mResourceGroups.begin(); i!=mResourceGroups.end(); ++i)
		UnloadResourcesInGroup(i->first);
}

bool ResourceMgr::AddResourceDirToGroup(const string& path, const StringKey& group, const string& includeRegexp, const string& excludeRegexp)
{
	//TODO add support for regexps

	gLogMgr.LogMessage("Adding dir '", path, "' to group '", group, "'");

	boost::filesystem::path boostPath = mBasePath + path;
	if (!boost::filesystem::exists(boostPath))
	{
		gLogMgr.LogMessage("Path does not exist '", boostPath.string(), "'", LOG_ERROR);
		return false;
	}

	bool result = true;
	boost::filesystem::directory_iterator iend;
	for (boost::filesystem::directory_iterator i(boostPath); i!=iend; ++i)
	{
		if (boost::filesystem::is_directory(i->status()))
		{
			string dirStr = i->path().filename();
			if (dirStr.compare(".svn")!=0)
				if (!AddResourceDirToGroup(i->path().string(), group, includeRegexp, excludeRegexp))
					result = false;
		}
		else
		{
			if (!AddResourceFileToGroup(i->path().string(), group, Resource::TYPE_AUTODETECT, false))
				result = false;
		}
	}
	return result;
}

bool ResourceMgr::AddResourceFileToGroup(const string& filepath, const StringKey& group, Resource::eType type, bool pathRelative)
{
	gLogMgr.LogMessage("Adding resource '", filepath, "' to group '", group, "'");

	boost::filesystem::path boostPath;
	if (pathRelative)
		boostPath = mBasePath + filepath;
	else
		boostPath = filepath;
	if (!boost::filesystem::exists(boostPath)){
		gLogMgr.LogMessage("Resource located at '", boostPath.string(), "' not found", LOG_ERROR);
		return false;
	}

	// detect resource type
	if (type == Resource::TYPE_AUTODETECT)
	{
		ExtToTypeMap::const_iterator i = mExtToTypeMap.find(boostPath.extension().substr(1));
		if (i != mExtToTypeMap.end())
			type = i->second;
	}
	// error
	if (type == Resource::TYPE_AUTODETECT)
	{
		gLogMgr.LogMessage("Can't detect type of resource '", filepath, "'", LOG_ERROR);
		return false;
	}

	string name = boostPath.filename();
	ResourceGroupMap::const_iterator groupIt = mResourceGroups.find(group);
	if (mResourceGroups.find(group) != mResourceGroups.end() && groupIt->second->find(name) != groupIt->second->end())
	{
		gLogMgr.LogMessage("Resource already exists");
		return true;
	}

	ResourcePtr r = mResourceCreationMethods[type]();
	r->SetState(Resource::STATE_INITIALIZED);
	r->SetName(name);
	r->SetFilepath(boostPath.string());

	AddResourceToGroup(group, name, r);

	gLogMgr.LogMessage("Resource added");
	return true;
}

bool ResourceSystem::ResourceMgr::AddManualResourceToGroup( const StringKey& name, const StringKey& group, Resource::eType type )
{
	gLogMgr.LogMessage("Adding resource '", name, "' to group '", group, "'");

	BS_ASSERT_MSG(type != Resource::TYPE_AUTODETECT, "Must specify resource type when creating it manually");

	ResourcePtr r = mResourceCreationMethods[type]();
	r->SetState(Resource::STATE_INITIALIZED);
	r->SetName(name);
	r->SetManual(true);
	
	AddResourceToGroup(group, name, r);

	gLogMgr.LogMessage("Resource added");
	return true;
}

void ResourceSystem::ResourceMgr::AddResourceToGroup( const StringKey& group, const StringKey& name, const ResourcePtr res )
{
	ResourceGroupMap::iterator groupIt = mResourceGroups.find(group);
	if (groupIt == mResourceGroups.end())
	{
		mResourceGroups[group] = DYN_NEW ResourceMap();
		groupIt = mResourceGroups.find(group);
	}
	(*groupIt->second)[name] = res;
}

void ResourceMgr::LoadResourcesInGroup(const StringKey& group)
{
	gLogMgr.LogMessage("Loading resource group '", group, "'");

	ResourceGroupMap::const_iterator gi = mResourceGroups.find(group);

	if (gi==mResourceGroups.end())
		gLogMgr.LogMessage("Unknown group '", group, "'", LOG_ERROR);
	BS_ASSERT_MSG(gi != mResourceGroups.end(), "Unknown group");

	const ResourceMap& resmap = *gi->second;
	if (mListener)
		mListener->ResourceGroupLoadStarted(group, static_cast<uint32>(resmap.size()));
	for (ResourceMap::const_iterator ri = resmap.begin(); ri != resmap.end(); ++ri)
		if (ri->second->GetState() == Resource::STATE_INITIALIZED)
		{
			if (mListener)
				mListener->ResourceLoadStarted(ri->second);
			ri->second->Load();
			if (mListener)
				mListener->ResourceLoadEnded();
		}
	if (mListener)
		mListener->ResourceGroupLoadEnded();

	gLogMgr.LogMessage("Resource group loaded '", group, "'");
}

void ResourceMgr::UnloadResourcesInGroup(const StringKey& group, bool allowManual)
{
	gLogMgr.LogMessage("Unloading resource group '", group, "'");

	ResourceGroupMap::const_iterator gi = mResourceGroups.find(group);

	if (gi==mResourceGroups.end())
		gLogMgr.LogMessage("Unknown group '", group, "'", LOG_ERROR);
	BS_ASSERT_MSG(gi != mResourceGroups.end(), "Unknown group");

	const ResourceMap& resmap = *gi->second;
	for (ResourceMap::const_iterator ri = resmap.begin(); ri != resmap.end(); ++ri)
		if (ri->second->GetState() >= Resource::STATE_LOADING)
			ri->second->Unload(allowManual);

	gLogMgr.LogMessage("Resource group '", group, "' unloaded");
}

void ResourceMgr::DeleteGroup(const StringKey& group)
{
	ResourceGroupMap::const_iterator groupIt = mResourceGroups.find(group);
	if (groupIt == mResourceGroups.end())
		return;
	UnloadResourcesInGroup(group, true);
	DYN_DELETE groupIt->second;
	mResourceGroups.erase(groupIt);
}

void ResourceMgr::SetLoadingListener(IResourceLoadingListener* listener)
{
	mListener = listener;
}

ResourcePtr ResourceMgr::GetResource(const StringKey& group, const StringKey& name)
{
	ResourceGroupMap::const_iterator gi = mResourceGroups.find(group);
	if (gi == mResourceGroups.end()){
		//gLogMgr.LogMessage("Resource group '", group, "' not found", LOG_WARNING);
		return ResourcePtr(); // null
	}
	const ResourceMap& resmap = *gi->second;
	ResourceMap::const_iterator ri = resmap.find(name);
	if (ri == resmap.end())
	{
		//gLogMgr.LogMessage("Resource '",  name, "' in group '", group, "' not found", LOG_WARNING);
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
	for (ResourceMap::const_iterator ri = resmap.begin(); ri != resmap.end(); ri++)
		output.push_back(ri->second); 
	return;
}

void ResourceSystem::ResourceMgr::DeleteResource( const StringKey& group, const StringKey& name )
{
	gLogMgr.LogMessage("Deleting resource '", name, "' in group '", group, "'");

	ResourceGroupMap::iterator gi = mResourceGroups.find(group);

	if (gi==mResourceGroups.end())
		gLogMgr.LogMessage("Unknown group '", group, "'", LOG_ERROR);
	BS_ASSERT_MSG(gi != mResourceGroups.end(), "Unknown group");

	ResourceMap& resmap = *gi->second;
	ResourceMap::iterator ri = resmap.find(name);
	if (ri==resmap.end())
		gLogMgr.LogMessage("Unknown resource '", name, "' in group '", group, "'", LOG_ERROR);
	BS_ASSERT_MSG(ri != resmap.end(), "Unknown resource");
	ri->second->Unload(true);
	resmap.erase(ri);

	gLogMgr.LogMessage("Resource deleted");
}

ResourceSystem::ResourcePtr ResourceSystem::ResourceMgr::GetResource( char* groupSlashName )
{
	char* lastSlashPos = 0;
	char* str = groupSlashName;
	for(; *str; ++str)
		if (*str == '/')
			lastSlashPos = str;
	if (!lastSlashPos)
		return ResourcePtr();
	return GetResource(StringKey(groupSlashName, lastSlashPos-groupSlashName), StringKey(lastSlashPos+1, str-lastSlashPos-1));
}

void ResourceSystem::ResourceMgr::DeleteAllResources( void )
{
	for (ResourceGroupMap::const_iterator i=mResourceGroups.begin(); i!=mResourceGroups.end(); ++i)
	{
		UnloadResourcesInGroup(i->first, true);
		DYN_DELETE i->second;
	}
	mResourceGroups.clear();
}