#include "Common.h"
#include "ResourcePointers.h"
#include "StringMgr.h"
#include "TextResource.h"
#include "EntitySystem/EntityMgr/EntityMgr.h"


using namespace StringSystem;

StringMgr* StringMgr::msSystem = 0;
StringMgr* StringMgr::msProject = 0;
const TextData StringMgr::NullTextData = TextData("");

StringMgr::StringMgr(const ResourceSystem::eBasePathType basePathType, const string& basePath)
{
	mBasePathType = basePathType;
	mBasePath = basePath;
	mNeedsUpdate = false;
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
	vector<ResourceSystem::ResourcePtr> resources;
	gResourceMgr.GetResources(resources, mBasePathType, mBasePath + path, false);

	for (vector<ResourceSystem::ResourcePtr>::iterator it = resources.begin(); it != resources.end(); it++)
	{
		if ((*it)->GetType() != ResourceSystem::RESTYPE_TEXTRESOURCE) continue;
		TextResourcePtr tp = (TextResourcePtr)(*it);
		const TextDataMap* dm = tp->GetTextDataMap();
		mGroupTextDataMap[tp->GetGroupName()].insert(dm->begin(), dm->end());
	}
	
	return true;
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
			if (!AddDataFromDir(lang + '/' + country + '/')) { result = false; }
		}
		// Add language specific texts.
		if (!AddDataFromDir(lang + '/')) { result = false; }
	}
	// Add default texts.
	if (!AddDataFromDir("")) { result = false; }
	
	mLanguage = lang;
	mCountry = country;
	return result;
}

bool StringMgr::Update()
{ 
	bool result = mNeedsUpdate && LoadLanguagePack(mLanguage, mCountry);
	mNeedsUpdate = false;
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

void ResourceUnloadCallback(TextResource* resource)
{
	if (!resource) return;
	if (resource->GetBasePathType() == ResourceSystem::BPT_SYSTEM) gStringMgrSystem.NeedsUpdate();
	else if (resource->GetBasePathType() == ResourceSystem::BPT_PROJECT) gStringMgrProject.NeedsUpdate();
}

void StringMgr::Init(const string& systemBasePath, const string& projectBasePath)
{
	OC_ASSERT(msSystem == 0 && msProject == 0);
	msSystem = new StringMgr(ResourceSystem::BPT_SYSTEM, systemBasePath);
	msProject = new StringMgr(ResourceSystem::BPT_PROJECT, projectBasePath);

	if (GlobalProperties::Get<bool>("DevelopMode"))
	{
		gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, systemBasePath, "Strings", ".+\\.str");
	}
  
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