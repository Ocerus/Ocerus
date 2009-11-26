#include "Common.h"
#include "ResourcePointers.h"
#include "StringMgr.h"
#include "TextResource.h"


using namespace StringSystem;


StringMgr::StringMgr(const string& basepath)
{
	ocInfo << "*** StringMgr init ***";
	mBasePath = basepath;
}

StringMgr::~StringMgr(void)
{
	ocInfo << "*** StringMgr deinit ***";
	UnloadData();
	ocInfo << "*** StringMgr unloaded ***";
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

	ocInfo << "StringMgr: Loading data from resource group ""strings""";
	vector<ResourceSystem::ResourcePtr> resourceGroup;
	gResourceMgr.GetResourceGroup("strings", resourceGroup);

	vector<ResourceSystem::ResourcePtr>::iterator it;

	for (it = resourceGroup.begin(); it != resourceGroup.end(); it++)
	{
		TextResourcePtr tp = (TextResourcePtr)(*it);
		const TextDataMap* dm = tp->GetTextDataMap();
		mTextDataMap.insert(dm->begin(), dm->end());
	}

	gResourceMgr.UnloadResourcesInGroup("strings");
	return result;
}

bool StringMgr::LoadDataFromFile(const string& filepath, bool pathRelative)
{
	bool result = true;
	result = gResourceMgr.AddResourceFileToGroup(mBasePath + filepath, "strings", ResourceSystem::RESTYPE_TEXTRESOURCE, pathRelative);
	gResourceMgr.LoadResourcesInGroup("strings");

	ocInfo << "StringMgr: Loading data from resource group ""strings""";
	vector<ResourceSystem::ResourcePtr> resourceGroup;
	gResourceMgr.GetResourceGroup("strings", resourceGroup);

	vector<ResourceSystem::ResourcePtr>::iterator it;

	for (it = resourceGroup.begin(); it != resourceGroup.end(); it++)
	{
		TextResourcePtr tp = (TextResourcePtr)(*it);
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
		ocError << "StringMgr: Index " << key << " doesn't exist. Return value set to empty TextData";
	}
	return returnValue;
}

const TextData StringMgr::GetTextData(const StringKey& key)
{
	const TextData returnValue = mTextDataMap[key];
	if (returnValue == "") {
		ocError << "StringMgr: Index " << key << " doesn't exist. Return value set to empty TextData";
	}
	return returnValue;
}
