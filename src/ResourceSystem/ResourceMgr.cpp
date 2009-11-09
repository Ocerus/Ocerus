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
#include "../ScriptSystem/ScriptResource.h"

#ifdef __WIN__
#pragma warning(disable: 4996)
#endif

using namespace ResourceSystem;

ResourceMgr::ResourceMgr( void ):
	mBasePath(), mListener(0)
{

}

void ResourceSystem::ResourceMgr::Init( const string& basepath )
{
	mBasePath = basepath;

	ocInfo << "*** ResourceMgr init ***";
	ocInfo << "Base directory = " << basepath;

	mResourceCreationMethods[NUM_RESTYPES-1] = 0; // safety reasons

	// register resource types
	mResourceCreationMethods[RESTYPE_TEXTURE] = GfxSystem::Texture::CreateMe;
	mResourceCreationMethods[RESTYPE_CEGUIRESOURCE] = GUISystem::CEGUIResource::CreateMe;
	mResourceCreationMethods[RESTYPE_TEXTRESOURCE] = StringSystem::TextResource::CreateMe;
	mResourceCreationMethods[RESTYPE_XMLRESOURCE] = XMLResource::CreateMe;
	mResourceCreationMethods[RESTYPE_PARTICLERESOURCE] = GfxSystem::ParticleResource::CreateMe;
	mResourceCreationMethods[RESTYPE_SCRIPTRESOURCE] = ScriptSystem::ScriptResource::CreateMe;
	mExtToTypeMap["png"] = RESTYPE_TEXTURE;
	mExtToTypeMap["bmp"] = RESTYPE_TEXTURE;
	mExtToTypeMap["jpg"] = RESTYPE_TEXTURE;
	mExtToTypeMap["gif"] = RESTYPE_TEXTURE;
	mExtToTypeMap["layout"] = RESTYPE_CEGUIRESOURCE;
	mExtToTypeMap["imageset"] = RESTYPE_CEGUIRESOURCE;
	mExtToTypeMap["font"] = RESTYPE_CEGUIRESOURCE;
	mExtToTypeMap["scheme"] = RESTYPE_CEGUIRESOURCE;
	mExtToTypeMap["tga"] = RESTYPE_TEXTURE;
	mExtToTypeMap["ttf"] = RESTYPE_CEGUIRESOURCE;
	mExtToTypeMap["looknfeel"] = RESTYPE_CEGUIRESOURCE;
	mExtToTypeMap["str"] = RESTYPE_TEXTRESOURCE;
	mExtToTypeMap["xml"] = RESTYPE_XMLRESOURCE;
	mExtToTypeMap["psi"] = RESTYPE_PARTICLERESOURCE;
	mExtToTypeMap["as"] = RESTYPE_SCRIPTRESOURCE;

	OC_ASSERT_MSG(mResourceCreationMethods[NUM_RESTYPES-1], "Not all resource types are registered");

	ocInfo << "All resource types registered";
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
	ocInfo << "Adding dir '" << path << "' to group '" << group << "'";

	boost::filesystem::path boostPath = mBasePath + path;
	if (!boost::filesystem::exists(boostPath))
	{
		ocError << "Path does not exist '" << boostPath.string() << "'";
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
			if (!AddResourceFileToGroup(i->path().string(), group, RESTYPE_AUTODETECT, false))
				result = false;
		}
	}
	return result;
}

bool ResourceMgr::AddResourceFileToGroup(const string& filepath, const StringKey& group, eResourceType type, bool pathRelative)
{
	ocInfo << "Adding resource '" << filepath << "' to group '" << group << "'";

	boost::filesystem::path boostPath;
	if (pathRelative)
		boostPath = mBasePath + filepath;
	else
		boostPath = filepath;
	if (!boost::filesystem::exists(boostPath)){
		ocError << "Resource located at '" << boostPath.string() << "' not found";
		return false;
	}

	// detect resource type
	if (type == RESTYPE_AUTODETECT)
	{
		ExtToTypeMap::const_iterator i = mExtToTypeMap.find(boostPath.extension().substr(1));
		if (i != mExtToTypeMap.end())
			type = i->second;
	}
	// error
	if (type == RESTYPE_AUTODETECT)
	{
		ocError << "Can't detect type of resource '" << filepath << "'";
		return false;
	}

	string name = boostPath.filename();
	ResourceGroupMap::const_iterator groupIt = mResourceGroups.find(group);
	if (mResourceGroups.find(group) != mResourceGroups.end() && groupIt->second->find(name) != groupIt->second->end())
	{
		ocWarning << "Resource '" << name << "' already exists";
		return true;
	}

	ResourcePtr r = mResourceCreationMethods[type]();
	r->SetState(Resource::STATE_INITIALIZED);
	r->SetName(name);
	r->SetFilepath(boostPath.string());

	AddResourceToGroup(group, name, r);

	ocInfo << "Resource '" << name << "' added";
	return true;
}

bool ResourceSystem::ResourceMgr::AddManualResourceToGroup( const StringKey& name, const StringKey& group, eResourceType type )
{
    ocInfo << "Manually adding resource '" << name << "' to group '" << group << "'";

	OC_ASSERT_MSG(type != RESTYPE_AUTODETECT, "Must specify resource type when creating it manually");

	ResourcePtr r = mResourceCreationMethods[type]();
	r->SetState(Resource::STATE_INITIALIZED);
	r->SetName(name.ToString());
	r->SetManual(true);

	AddResourceToGroup(group, name, r);

	ocInfo << "Resource '" << name << "' added";
	return true;
}

void ResourceSystem::ResourceMgr::AddResourceToGroup( const StringKey& group, const StringKey& name, const ResourcePtr res )
{
	ResourceGroupMap::iterator groupIt = mResourceGroups.find(group);
	if (groupIt == mResourceGroups.end())
	{
		mResourceGroups[group] = new ResourceMap();
		groupIt = mResourceGroups.find(group);
	}
	(*groupIt->second)[name] = res;
}

void ResourceMgr::LoadResourcesInGroup(const StringKey& group)
{
	ocInfo << "Loading resource group '" << group << "'";

	ResourceGroupMap::const_iterator gi = mResourceGroups.find(group);

	if (gi==mResourceGroups.end())
	{
		ocError << "Unknown group '" << group << "'";
		return;
	}

	const ResourceMap& resmap = *gi->second;
	if (mListener)
		mListener->ResourceGroupLoadStarted(group.ToString(), static_cast<uint32>(resmap.size()));
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

	ocInfo << "Resource group loaded '" << group << "'";
}

void ResourceMgr::UnloadResourcesInGroup(const StringKey& group, bool allowManual)
{
	ocInfo << "Unloading resource group '" << group << "'";

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

	ocInfo << "Resource group '" << group << "' unloaded";
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
	for (ResourceMap::const_iterator ri = resmap.begin(); ri != resmap.end(); ri++)
		output.push_back(ri->second);
	return;
}

void ResourceSystem::ResourceMgr::DeleteResource( const StringKey& group, const StringKey& name )
{
	ocInfo << "Deleting resource '" << name << "' in group '" << group << "'";

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
		delete i->second;
	}
	mResourceGroups.clear();
}
