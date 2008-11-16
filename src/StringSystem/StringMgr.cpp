#include "Common.h"
#include "StringMgr.h"

using namespace StringSystem;


StringMgr::StringMgr(const string& basepath)
{
	mBasePath = basepath;
	gLogMgr.LogMessage("*** StringMgr init ***");
}

StringMgr::~StringMgr()
{
	gLogMgr.LogMessage("*** StringMgr deinit ***");
	UnloadData();
	gLogMgr.LogMessage("*** StringMgr unloaded ***");
}

bool StringMgr::LoadLanguagePack(const string& lang)
{
	bool result = true;
	result = LoadDataFromDir(lang);
	mLanguage = lang;
	return result;
}

bool StringMgr::LoadDataFromDir(const string& path, const string& includeRegexp, const string& excludeRegexp)
{
	bool result = true;
	result = gResourceMgr.AddResourceDirToGroup(mBasePath + path, "strings", includeRegexp, excludeRegexp);
	gResourceMgr.LoadResourcesInGroup("strings");
	
	gLogMgr.LogMessage("StringMgr: Loading data from resource");
	std::vector<ResourceSystem::ResourcePtr> ResourceGroup = gResourceMgr.GetResourceGroup("strings");
	assert(ResourceGroup.size());

	std::vector<ResourceSystem::ResourcePtr>::iterator it;
	
	for (it = ResourceGroup.begin(); it != ResourceGroup.end(); it++)
	{
		TextResourcePtr tp = static_cast<TextResourcePtr>(*it);
		TextDataMap dm = tp->GetTextDataMap();
		mTextDataMap.insert(dm.begin(), dm.end());
	}

	gResourceMgr.UnloadResourcesInGroup("strings");
	return result;
}

bool StringMgr::LoadDataFromFile(const string& filepath, ResourceSystem::Resource::eType type, bool pathRelative)
{
	bool result = true;
	result = gResourceMgr.AddResourceFileToGroup(mBasePath + filepath, "strings", type, pathRelative);
	gResourceMgr.LoadResourcesInGroup("strings");

	gLogMgr.LogMessage("StringMgr: Loading data from resource");
	std::vector<ResourceSystem::ResourcePtr> ResourceGroup = gResourceMgr.GetResourceGroup("strings");
	assert(ResourceGroup.size());

	std::vector<ResourceSystem::ResourcePtr>::iterator it;

	for (it = ResourceGroup.begin(); it != ResourceGroup.end(); it++)
	{
		TextResourcePtr tp = static_cast<TextResourcePtr>(*it);
		TextDataMap dm = tp->GetTextDataMap();
		mTextDataMap.insert(dm.begin(), dm.end());
	}

	gResourceMgr.UnloadResourcesInGroup("strings");
	return result;
}

bool StringMgr::UnloadData()
{
	mTextDataMap.clear();
	return true;
}


TextData* StringMgr::GetTextDataPtr(const StringKey& key)
{
	return &mTextDataMap[key];
}

TextData StringMgr::GetTextData(const StringKey& key)
{
	return mTextDataMap[key];
}