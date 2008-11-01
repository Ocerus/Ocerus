#include <stdio.h>
#include <boost/filesystem.hpp>
#include "ResourceMgr.h"
#include "../Common.h"
#include "../GfxSystem/Texture.h"
#include "IResourceLoadingListener.h"

#pragma warning(disable: 4996)

using namespace ResourceSystem;

ResourceMgr::ResourceMgr(const string& basedir): 
	mListener(0), mBaseDir(basedir)
{
	gLogMgr.LogMessage("*** ResourceMgr init ***");
	gLogMgr.LogMessage("Base directory  = " + basedir);

	mResourceCreationMethods[Resource::NUM_TYPES-1] = 0; // safety reasons

	// register resource types
	mResourceCreationMethods[Resource::TYPE_TEXTURE] = GfxSystem::Texture::CreateMe;
	mExtToTypeMap["png"] = Resource::TYPE_TEXTURE;
	mExtToTypeMap["bmp"] = Resource::TYPE_TEXTURE;
	mExtToTypeMap["jpg"] = Resource::TYPE_TEXTURE;
	mExtToTypeMap["gif"] = Resource::TYPE_TEXTURE;

	assert(mResourceCreationMethods[Resource::NUM_TYPES-1] && "Not all resource types are registered");

	gLogMgr.LogMessage("All resource types registered");
}

ResourceMgr::~ResourceMgr()
{
	for (ResourceGroupMap::const_iterator i=mResourceGroups.begin(); i!=mResourceGroups.end(); ++i)
		UnloadResourcesInGroup(i->first);
}

bool ResourceMgr::AddResourceDirToGroup(const string& path, const string& group, const string& includeRegexp, const string& excludeRegexp)
{
	//TODO add support for regexps

	gLogMgr.LogMessage("Adding dir '" + path+"' to group '"+ group +"'");

	bool result = true;
	boost::filesystem::directory_iterator iend;
	for (boost::filesystem::directory_iterator i(mBaseDir + path); i!=iend; ++i)
	{
		if (boost::filesystem::is_directory(i->status()))
		{
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
		boostPath = mBaseDir + filepath;
	else
		boostPath = filepath;
	if (!boost::filesystem::exists(boostPath))
		return false;
	if (type == Resource::TYPE_AUTODETECT)
	{
		ExtToTypeMap::const_iterator i = mExtToTypeMap.find(boostPath.extension());
		if (i != mExtToTypeMap.end())
			type = i->second;
	}
	if (type == Resource::TYPE_AUTODETECT)
		return false;
	ResourcePtr r = mResourceCreationMethods[type]();
	r->SetState(Resource::STATE_INITIALIZED);
	string name = boostPath.filename();
	r->SetName(name);
	r->SetFilepath(boostPath.string());
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
			gLogMgr.LogMessage("Loading resource '"+ri->second->GetName()+"'");
			ri->second->Load();
			gLogMgr.LogMessage("Resource loaded");
			if (mListener)
				mListener->ResourceLoadEnded();
		}
	if (mListener)
		mListener->ResourceGroupLoadEnded();

	gLogMgr.LogMessage("Resource group loaded '"+group+"'");
}

void ResourceMgr::UnloadResourcesInGroup(const string& group)
{
	gLogMgr.LogMessage("Unloading resource group '"+group+"'");

	ResourceGroupMap::const_iterator gi = mResourceGroups.find(group);

	if (gi==mResourceGroups.end())
		gLogMgr.LogMessage("Unknown group '"+group+"'", LOG_ERROR);
	assert(gi != mResourceGroups.end() && "Unknown group");

	const ResourceMap& resmap = gi->second;
	for (ResourceMap::const_iterator ri = resmap.begin(); ri != resmap.end(); ++ri)
		if (ri->second->GetState() >= Resource::STATE_LOADING)
			ri->second->Unload();

	gLogMgr.LogMessage("Resource group '"+group+"' unloaded");
}

void ResourceMgr::ClearGroup(const string& group)
{
	if (mResourceGroups.find(group) == mResourceGroups.end())
		return;
	UnloadResourcesInGroup(group);
	mResourceGroups.erase(group);
}

void ResourceMgr::SetLoadingListener(IResourceLoadingListener* listener)
{
	mListener = listener;
}

ResourcePtr ResourceMgr::GetResource(const string& groupSlashName)
{
	string::size_type slashPos = groupSlashName.find('/');
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
	ResourceGroupMap::iterator gi = mResourceGroups.find(group);

	if (gi==mResourceGroups.end())
		gLogMgr.LogMessage("Unknown group '"+group+"'", LOG_ERROR);
	assert(gi != mResourceGroups.end() && "Unknown group");

	ResourceMap& resmap = gi->second;
	ResourceMap::const_iterator ri = resmap.find(name);
	if (ri==resmap.end())
		gLogMgr.LogMessage("Unknown resource '"+name+"' in group '"+group+"'", LOG_ERROR);
	assert(ri != resmap.end() && "Unknown resource");
	if (ri->second->GetState() >= Resource::STATE_LOADING)
		ri->second->Unload();
	resmap.erase(ri);
}