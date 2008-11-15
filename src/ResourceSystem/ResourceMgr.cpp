#include <stdio.h>
#include <boost/filesystem.hpp>
#include "ResourceMgr.h"
#include "../Common.h"
#include "../GfxSystem/Texture.h"
#include "../GUISystem/CEGUIResource.h"
#include "IResourceLoadingListener.h"

#pragma warning(disable: 4996)

using namespace ResourceSystem;

ResourceMgr::ResourceMgr(const string& basepath): 
	mListener(0), mBasePath(basepath)
{
	gLogMgr.LogMessage("*** ResourceMgr init ***");
	gLogMgr.LogMessage("Base directory = " + basepath);

	mResourceCreationMethods[Resource::NUM_TYPES-1] = 0; // safety reasons

	// register resource types
	mResourceCreationMethods[Resource::TYPE_TEXTURE] = GfxSystem::Texture::CreateMe;
	mResourceCreationMethods[Resource::TYPE_CEGUIRESOURCE] = GUISystem::CEGUIResource::CreateMe;
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

	assert(mResourceCreationMethods[Resource::NUM_TYPES-1] && "Not all resource types are registered");

	gLogMgr.LogMessage("All resource types registered");
}

ResourceMgr::~ResourceMgr()
{
	//assert(mResourceGroups.empty());	
}

void ResourceMgr::UnloadAllResources()
{
	for (ResourceGroupMap::const_iterator i=mResourceGroups.begin(); i!=mResourceGroups.end(); ++i)
		UnloadResourcesInGroup(i->first);
}

bool ResourceMgr::AddResourceDirToGroup(const string& path, const string& group, const string& includeRegexp, const string& excludeRegexp)
{
	//TODO add support for regexps

	gLogMgr.LogMessage("Adding dir '" + path+"' to group '"+ group +"'");

	boost::filesystem::path boostPath = mBasePath + path;
	if (!boost::filesystem::exists(boostPath))
	{
		gLogMgr.LogMessage("Path does not exist '" + boostPath.string() + "'", LOG_ERROR);
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

bool ResourceMgr::AddResourceFileToGroup(const string& filepath, const string& group, Resource::eType type, bool pathRelative)
{
	gLogMgr.LogMessage("Adding resource '" + filepath+"' to group '"+ group +"'");

	boost::filesystem::path boostPath;
	if (pathRelative)
		boostPath = mBasePath + filepath;
	else
		boostPath = filepath;
	if (!boost::filesystem::exists(boostPath)){
		gLogMgr.LogMessage("Resource located at '" + boostPath.string() +"' not found", LOG_ERROR);
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
		gLogMgr.LogMessage("Can't detect type of resource '" + filepath + "'", LOG_ERROR);
		return false;
	}

	ResourcePtr r = mResourceCreationMethods[type]();
	r->SetState(Resource::STATE_INITIALIZED);
	string name = boostPath.filename();
	r->SetName(name);
	r->SetFilepath(boostPath.string());
	mResourceGroups[group][name] = r;

	gLogMgr.LogMessage("Resource added");
	return true;
}

bool ResourceSystem::ResourceMgr::AddManualResourceToGroup( const string& name, const string& group, Resource::eType type )
{
	gLogMgr.LogMessage("Adding resource '" + name +"' to group '"+ group +"'");

	assert(type != Resource::TYPE_AUTODETECT && "Must specify resource type when creating it manually");

	ResourcePtr r = mResourceCreationMethods[type]();
	r->SetState(Resource::STATE_INITIALIZED);
	r->SetName(name);
	r->SetManual(true);
	mResourceGroups[group][name] = r;

	gLogMgr.LogMessage("Resource added");
	return true;
}

void ResourceMgr::LoadResourcesInGroup(const string& group)
{
	gLogMgr.LogMessage("Loading resource group '"+group+"'");

	ResourceGroupMap::const_iterator gi = mResourceGroups.find(group);

	if (gi==mResourceGroups.end())
		gLogMgr.LogMessage("Unknown group '"+group+"'", LOG_ERROR);
	assert(gi != mResourceGroups.end() && "Unknown group");

	const ResourceMap& resmap = gi->second;
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

	gLogMgr.LogMessage("Resource group loaded '"+group+"'");
}

void ResourceMgr::UnloadResourcesInGroup(const string& group, bool allowManual)
{
	gLogMgr.LogMessage("Unloading resource group '"+group+"'");

	ResourceGroupMap::const_iterator gi = mResourceGroups.find(group);

	if (gi==mResourceGroups.end())
		gLogMgr.LogMessage("Unknown group '"+group+"'", LOG_ERROR);
	assert(gi != mResourceGroups.end() && "Unknown group");

	const ResourceMap& resmap = gi->second;
	for (ResourceMap::const_iterator ri = resmap.begin(); ri != resmap.end(); ++ri)
		if (ri->second->GetState() >= Resource::STATE_LOADING)
			ri->second->Unload(allowManual);

	gLogMgr.LogMessage("Resource group '"+group+"' unloaded");
}

void ResourceMgr::DeleteGroup(const string& group)
{
	if (mResourceGroups.find(group) == mResourceGroups.end())
		return;
	UnloadResourcesInGroup(group, true);
	mResourceGroups.erase(group);
}

void ResourceMgr::SetLoadingListener(IResourceLoadingListener* listener)
{
	mListener = listener;
}

ResourcePtr ResourceMgr::GetResource(const string& groupSlashName)
{
	string::size_type slashPos = groupSlashName.find_last_of('/');
	return GetResource(groupSlashName.substr(0, slashPos), groupSlashName.substr(slashPos+1));
}

ResourcePtr ResourceMgr::GetResource(const string& group, const string& name)
{
	ResourceGroupMap::const_iterator gi = mResourceGroups.find(group);
	if (gi == mResourceGroups.end()){

		return ResourcePtr(); // null
	}
	const ResourceMap& resmap = gi->second;
	ResourceMap::const_iterator ri = resmap.find(name);
	if (ri == resmap.end())
		return ResourcePtr(); // null
	return ri->second;
}

void ResourceSystem::ResourceMgr::DeleteResource( const string& group, const string& name )
{
	gLogMgr.LogMessage("Deleting resource '"+name+"' in group '"+group+"'");

	ResourceGroupMap::iterator gi = mResourceGroups.find(group);

	if (gi==mResourceGroups.end())
		gLogMgr.LogMessage("Unknown group '"+group+"'", LOG_ERROR);
	assert(gi != mResourceGroups.end() && "Unknown group");

	ResourceMap& resmap = gi->second;
	ResourceMap::const_iterator ri = resmap.find(name);
	if (ri==resmap.end())
		gLogMgr.LogMessage("Unknown resource '"+name+"' in group '"+group+"'", LOG_ERROR);
	assert(ri != resmap.end() && "Unknown resource");
	ri->second->Unload(true);
	resmap.erase(ri);

	gLogMgr.LogMessage("Resource deleted");
}

