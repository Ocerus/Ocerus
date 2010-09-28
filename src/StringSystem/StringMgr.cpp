#include "Common.h"
#include "ResourcePointers.h"
#include "StringMgr.h"
#include "TextResource.h"
#include "EntitySystem/EntityMgr/EntityMgr.h"


using namespace StringSystem;

StringMgr* StringMgr::msSystem = 0;
StringMgr* StringMgr::msProject = 0;
const TextData StringMgr::NullTextData = TextData("");
const string StringMgr::ResourceGroup = "Strings";

StringMgr::StringMgr(const ResourceSystem::eBasePathType basePathType, const string& basePath)
{
	mBasePathType = basePathType;
	mBasePath = basePath;
	mUpdating = false;
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

bool StringMgr::AddDataFromDir(const string& path)
{
  return gResourceMgr.AddResourceDirToGroup(mBasePathType, mBasePath + path, ResourceSystem::GetBasePathTypeName(mBasePathType) + ResourceGroup, ".*", 
	  "", ResourceSystem::RESTYPE_TEXTRESOURCE, ResourceSystem::ResourceMgr::NullResourceTypeMap, false);
}

bool StringMgr::LoadLanguagePack(const string& lang, const string& country)
{	
	bool result = true;
	ocInfo << GetNameOfManager() << ": Loading language " << lang << "-" << country << ".";
	UnloadData();
	if (!lang.empty())
	{
	  if (!country.empty())
	  {
	    // Add country specific texts.
	    if (!AddDataFromDir(lang + "/" + country)) { result = false; }
	  }
	  // Add language specific texts.
	  if (!AddDataFromDir(lang)) { result = false; }
	}
	// Add default texts.
	if (!AddDataFromDir("")) { result = false; }
	
	// Load texts to the memory
	gResourceMgr.LoadResourcesInGroup(ResourceSystem::GetBasePathTypeName(mBasePathType) + ResourceGroup);
	vector<ResourceSystem::ResourcePtr> resourceGroup;
	gResourceMgr.GetResourceGroup(ResourceSystem::GetBasePathTypeName(mBasePathType) + ResourceGroup, resourceGroup);
	
	vector<ResourceSystem::ResourcePtr>::iterator it;

	for (it = resourceGroup.begin(); it != resourceGroup.end(); it++)
	{
		TextResourcePtr tp = (TextResourcePtr)(*it);
		const TextDataMap* dm = tp->GetTextDataMap();
		mGroupTextDataMap[tp->GetGroupName()].insert(dm->begin(), dm->end());
	}
	
	mLanguage = lang;
	mCountry = country;
	return result;
}

bool StringMgr::Refresh()
{ 
  if (mUpdating) return false;
  return LoadLanguagePack(mLanguage, mCountry);
}


bool StringMgr::UnloadData(void)
{
	mUpdating = true;
	mGroupTextDataMap.clear();
	gResourceMgr.DeleteGroup(ResourceSystem::GetBasePathTypeName(mBasePathType) + ResourceGroup);
	mUpdating = false;
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

void ResourceUnloadCallback(TextResource* resource)
{
	if (!resource) return;
	if (resource->GetBasePathType() == ResourceSystem::BPT_SYSTEM) gStringMgrSystem.Refresh();
	else if (resource->GetBasePathType() == ResourceSystem::BPT_PROJECT)
	{	
	  // If text resources with GUI texts are unloaded, broadcast message to all GUILayout components
	  if (gStringMgrProject.Refresh() && resource->GetGroupName() == GUISystem::GUIMgr::GUIGroup.ToString())
	  {
	    EntitySystem::EntityList entities;
	    gEntityMgr.GetEntitiesWithComponent(entities, EntitySystem::CT_GUILayout);
	    for (EntitySystem::EntityList::const_iterator it = entities.begin(); it != entities.end(); ++it)
	    {
	      gEntityMgr.PostMessage(*it, EntitySystem::EntityMessage::RESOURCE_UPDATE);
	    }
	  }
	}
}

void StringMgr::Init(const string& systemBasePath, const string& projectBasePath)
{
  OC_ASSERT(msSystem == 0 && msProject == 0);
  msSystem = new StringMgr(ResourceSystem::BPT_SYSTEM, systemBasePath);
  msProject = new StringMgr(ResourceSystem::BPT_PROJECT, projectBasePath);
  
  // Set text resource unload callback
	TextResource::SetUnloadCallback(&ResourceUnloadCallback);
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