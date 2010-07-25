#include "Common.h"
#include "ResourcePointers.h"
#include "StringMgr.h"
#include "TextResource.h"


using namespace StringSystem;

StringMgr* StringMgr::msSystem = 0;
StringMgr* StringMgr::msProject = 0;
const TextData StringMgr::NullTextData = TextData("");

StringMgr::StringMgr(const ResourceSystem::eBasePathType basePathType, const string& basePath)
{
	mBasePathType = basePathType;
	mBasePath = basePath;
	ocInfo << "*** " << GetNameOfManager() << " init ***";
}

StringMgr::~StringMgr(void)
{
	UnloadData();
	ocInfo << "*** " << GetNameOfManager() << " unloaded ***";
}

string StringMgr::GetNameOfManager()
{
  return string("StringMgr (") + ResourceSystem::GetBasePathTypeName(mBasePathType) + " - " + mBasePath + ")";
}

bool StringMgr::LoadLanguagePack(const string& lang, const string& country)
{
	bool result = true;
	ocInfo << GetNameOfManager() << ": Changing language to " << lang << "-" << country << ".";
	UnloadData();
	if (!lang.empty())
	{
	  if (!country.empty())
	  {
	    // Load country specific texts.
	    if (!LoadDataFromDir(lang + "/" + country)) { result = false; }
	  }
	  // Load language specific texts.
	  if (!LoadDataFromDir(lang)) { result = false; }
	}
	// Load default texts.
	if (!LoadDataFromDir("")) { result = false; }
	mLanguage = lang;
	mCountry = country;
	return result;
}

bool StringMgr::LoadDataFromDir(const string& path, const string& includeRegexp, const string& excludeRegexp, 
  bool recursive)
{
	bool result = true;
	result = gResourceMgr.AddResourceDirToGroup(mBasePathType, mBasePath + path, "strings", includeRegexp, 
	  excludeRegexp, ResourceSystem::RESTYPE_TEXTRESOURCE, ResourceSystem::ResourceMgr::NullResourceTypeMap, recursive);
	gResourceMgr.LoadResourcesInGroup("strings");

	vector<ResourceSystem::ResourcePtr> resourceGroup;
	gResourceMgr.GetResourceGroup("strings", resourceGroup);

	vector<ResourceSystem::ResourcePtr>::iterator it;

	for (it = resourceGroup.begin(); it != resourceGroup.end(); it++)
	{
		TextResourcePtr tp = (TextResourcePtr)(*it);
		const TextDataMap* dm = tp->GetTextDataMap();
		mGroupTextDataMap[tp->GetGroupName()].insert(dm->begin(), dm->end());
	}

	gResourceMgr.DeleteGroup("strings");
	return result;
}

bool StringMgr::LoadDataFromFile(const string& filepath, bool pathRelative)
{
	ResourceSystem::eBasePathType pathType;
	if (pathRelative) pathType = ResourceSystem::BPT_SYSTEM;
	else pathType = ResourceSystem::BPT_ABSOLUTE;

	bool result = true;
	result = gResourceMgr.AddResourceFileToGroup(mBasePath + filepath, "strings", ResourceSystem::RESTYPE_TEXTRESOURCE, pathType);
	gResourceMgr.LoadResourcesInGroup("strings");

	vector<ResourceSystem::ResourcePtr> resourceGroup;
	gResourceMgr.GetResourceGroup("strings", resourceGroup);

	vector<ResourceSystem::ResourcePtr>::iterator it;

	for (it = resourceGroup.begin(); it != resourceGroup.end(); it++)
	{
		TextResourcePtr tp = (TextResourcePtr)(*it);
		const TextDataMap* dm = tp->GetTextDataMap();
		mGroupTextDataMap[tp->GetGroupName()].insert(dm->begin(), dm->end());
	}

	gResourceMgr.DeleteGroup("strings");
	return result;
}

bool StringMgr::UnloadData(void)
{
	mGroupTextDataMap.clear();
	return true;
}


const TextData* StringMgr::GetTextDataPtr(const StringKey& group, const StringKey& key)
{
	GroupTextDataMap::const_iterator gtIt = mGroupTextDataMap.find(group);
	if (gtIt != mGroupTextDataMap.end())
	{
	  TextDataMap::const_iterator tIt = gtIt->second.find(key);
	  if (tIt != gtIt->second.end())
	  {
	    return &(tIt->second);
	  } else {
	    ocInfo << GetNameOfManager() << ": Index " << key << (group.ToString().empty()?"":string(" in group ") + group.ToString())
	      << " doesn't exist. Return value set to empty TextData.";
	  }
	} else {
	  ocInfo << GetNameOfManager() << ": Group " << (group.ToString().empty()?"<default>":group.ToString())
	      << " doesn't exist. Return value set to empty TextData.";
	}
	return &NullTextData;
}

const TextData StringMgr::GetTextData(const StringKey& group, const StringKey& key)
{
	return *GetTextDataPtr(group, key);
}

const TextData* StringMgr::GetTextDataPtr(const StringKey& key)
{
  return GetTextDataPtr("", key);
}

const TextData StringMgr::GetTextData(const StringKey& key)
{
  return *GetTextDataPtr("", key);
}

void StringMgr::Init(const string& systemBasePath, const string& projectBasePath)
{
  OC_ASSERT(msSystem == 0 && msProject == 0);
  msSystem = new StringMgr(ResourceSystem::BPT_SYSTEM, systemBasePath);
  msProject = new StringMgr(ResourceSystem::BPT_PROJECT, projectBasePath);
}

bool StringMgr::IsInited()
{
  return msSystem != 0 && msProject != 0;
}

void StringMgr::Deinit()
{
  OC_ASSERT(msSystem != 0 && msProject != 0);
  delete msSystem;
  msSystem = 0;
  delete msProject;
  msProject = 0;
}

StringMgr& StringMgr::GetSystem()
{
  OC_ASSERT(msSystem != 0);
  return *msSystem;
}

StringMgr& StringMgr::GetProject()
{
  OC_ASSERT(msProject != 0);
  return *msProject;
}