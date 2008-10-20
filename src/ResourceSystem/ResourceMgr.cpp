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
  /*if ( !exists( dir_path ) ) return false;
  directory_iterator end_itr; // default construction yields past-the-end
  for ( directory_iterator itr( dir_path );
        itr != end_itr;
        ++itr )
  {
    if ( is_directory(itr->status()) )
    {
      if ( find_file( itr->path(), file_name, path_found ) ) return true;
    }
    else if ( itr->leaf() == file_name ) // see below
    {
      path_found = itr->path();
      return true;
    }
  }
  return false;*/
  //boost::filesystem::directory_iterator 
	return true;
}

bool ResourceMgr::AddResourceFileToGroup(const string& filepath, const string& group, Resource::eType type)
{
	if (!boost::filesystem::exists(mBaseDir + filepath))
		return false;
	if (type == Resource::TYPE_AUTODETECT)
	{
		ExtToTypeMap::const_iterator i = mExtToTypeMap.find(boost::filesystem::extension(filepath));
		if (i != mExtToTypeMap.end())
			type = i->second;
	}
	if (type == Resource::TYPE_AUTODETECT)
		return false;
	return true;
}

