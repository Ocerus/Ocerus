#include "Common.h"
#include "StringMgr.h"

using namespace StringSystem;


StringMgr::StringMgr(const String& basepath)
{
	mBasePath = basepath;
	gLogMgr.LogMessage("*** StringMgr init ***");
}

StringMgr::~StringMgr(void)
{
	gLogMgr.LogMessage("*** StringMgr deinit ***");
	UnloadData();
	gLogMgr.LogMessage("*** StringMgr unloaded ***");
}

bool StringMgr::LoadLanguagePack(const String& lang)
{
	bool result = true;
	result = LoadDataFromDir(lang);
	mLanguage = lang;
	return result;
}

bool StringMgr::LoadDataFromDir(const String& path, const String& includeRegexp, const String& excludeRegexp)
{
	bool result = true;
	result = gResourceMgr.AddResourceDirToGroup(mBasePath + path, "strings", includeRegexp, excludeRegexp);
	gResourceMgr.LoadResourcesInGroup("strings");
	
	gLogMgr.LogMessage("StringMgr: Loading data from resource group ""strings""");
	Vector<ResourceSystem::ResourcePtr> resourceGroup;
	gResourceMgr.GetResourceGroup("strings", resourceGroup);
	
	Vector<ResourceSystem::ResourcePtr>::iterator it;
	
	for (it = resourceGroup.begin(); it != resourceGroup.end(); it++)
	{
		TextResourcePtr tp = static_cast<TextResourcePtr>(*it);
		const TextDataMap* dm = tp->GetTextDataMap();
		mTextDataMap.insert(dm->begin(), dm->end());
	}

	gResourceMgr.UnloadResourcesInGroup("strings");
	return result;
}

bool StringMgr::LoadDataFromFile(const String& filepath, ResourceSystem::Resource::eType type, bool pathRelative)
{
	bool result = true;
	result = gResourceMgr.AddResourceFileToGroup(mBasePath + filepath, "strings", type, pathRelative);
	gResourceMgr.LoadResourcesInGroup("strings");

	gLogMgr.LogMessage("StringMgr: Loading data from resource group ""strings""");
	Vector<ResourceSystem::ResourcePtr> resourceGroup;
	gResourceMgr.GetResourceGroup("strings", resourceGroup);

	Vector<ResourceSystem::ResourcePtr>::iterator it;

	for (it = resourceGroup.begin(); it != resourceGroup.end(); it++)
	{
		TextResourcePtr tp = static_cast<TextResourcePtr>(*it);
		const TextDataMap* dm = tp->GetTextDataMap();
		mTextDataMap.insert(dm->begin(), dm->end());
	}

	gResourceMgr.UnloadResourcesInGroup("strings");
	return result;
}

bool StringMgr::UnloadData(void)
{
	mTextDataMap.clear();
	return true;
}


const TextData* StringMgr::GetTextDataPtr(const StringKey& key)
{
	const TextData* returnValue = &mTextDataMap[key];
	if (*returnValue == "") {
		gLogMgr.LogMessage("StringMgr: Index ", key, " doesn't exist. Return value set to empty TextData", LOG_ERROR);
	}
	return returnValue;
}

const TextData StringMgr::GetTextData(const StringKey& key)
{
	const TextData returnValue = mTextDataMap[key];
	if (returnValue == "") {
		gLogMgr.LogMessage("StringMgr: Index ", key, " doesn't exist. Return value set to empty TextData", LOG_ERROR);
	}
	return returnValue;
}