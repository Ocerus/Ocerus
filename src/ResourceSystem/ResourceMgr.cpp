#include <stdio.h>
#include <boost/filesystem.hpp>
#include "ResourceMgr.h"
#include "../Common.h"
#include "ImageResource.h"

#pragma warning(disable: 4996)

using namespace ResourceSystem;

ResourceMgr::ResourceMgr(const string& basedir): 
	mListener(0), mBaseDir(basedir)
{
	mResourceCreationMethods[Resource::NUM_TYPES-1] = 0; // safety reasons

	// register resource types
	mResourceCreationMethods[Resource::TYPE_IMAGE] = ImageResource::CreateMe;
	mExtToTypeMap["png"] = Resource::TYPE_IMAGE;
	mExtToTypeMap["bmp"] = Resource::TYPE_IMAGE;
	mExtToTypeMap["jpg"] = Resource::TYPE_IMAGE;
	mExtToTypeMap["gif"] = Resource::TYPE_IMAGE;

	assert(mResourceCreationMethods[Resource::NUM_TYPES-1]);
}

ResourceMgr::~ResourceMgr() {}

bool ResourceMgr::AddResourceDirToGroup(const string& path, const string& group, const string& includeRegexp, const string& excludeRegexp)
{
	//TODO add support for regexps

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
	string name = boostPath.filename();
	r->SetName(name);
	r->SetFilepath(boostPath.string());
	mResourceGroups[group][name] = r;
	return true;
}

