#include "Common.h"
#include "Config.h"
#include "rudeconfig/config.h"

using namespace Core;

Config::Config(const String& filePath): mFilePath(filePath)
{
	gLogMgr.LogMessage("Loading config file '" + mFilePath + "'");

	BS_ASSERT(mFilePath.length()>0);

	// attempt to load the file
	mRudeConfig = DYN_NEW rude::Config();
	if (mRudeConfig->load(mFilePath.c_str()))
		gLogMgr.LogMessage("Config file loaded");
	else
		gLogMgr.LogMessage("Config file NOT loaded");
}

Config::~Config() 
{
	BS_ASSERT(mRudeConfig);
	Save();
	DYN_DELETE mRudeConfig;
}


bool Core::Config::Save( void )
{
	gLogMgr.LogMessage("Saving config file '"+ mFilePath +"'");

	if (mRudeConfig->save(mFilePath.c_str()))
	{
		gLogMgr.LogMessage("Config file saved");
		return true;
	}
	gLogMgr.LogMessage("Config file NOT saved");
	return false;
}

String Core::Config::GetString( const String& key, const String& defaultValue, const String& section )
{
	mRudeConfig->setSection(section.c_str()); // sets working section...all subsequent calls are related to this section.
	if (mRudeConfig->exists(key.c_str()))
		return mRudeConfig->getStringValue(key.c_str());
	return defaultValue;
}

int32 Core::Config::GetInt32( const String& key, const int32 defaultValue, const String& section )
{
	mRudeConfig->setSection(section.c_str());
	if (mRudeConfig->exists(key.c_str()))
		return mRudeConfig->getIntValue(key.c_str());
	return defaultValue;
}

bool Core::Config::GetBool( const String& key, const bool defaultValue, const String& section )
{
	mRudeConfig->setSection(section.c_str());
	if (mRudeConfig->exists(key.c_str()))
		return mRudeConfig->getBoolValue(key.c_str());
	return defaultValue;
}

void Core::Config::SetString( const String& key, const String& value, const String& section )
{
	mRudeConfig->setSection(section.c_str());
	mRudeConfig->setStringValue(key.c_str(), value.c_str());
}

void Core::Config::SetInt32( const String& key, const int32 value, const String& section )
{
	mRudeConfig->setSection(section.c_str());
	mRudeConfig->setIntValue(key.c_str(), value);
}

void Core::Config::SetBool( const String& key, const bool value, const String& section )
{
	mRudeConfig->setSection(section.c_str());
	mRudeConfig->setBoolValue(key.c_str(), value);
}

void Core::Config::RemoveKey( const String& key, const String& section )
{
	mRudeConfig->setSection(section.c_str());
	mRudeConfig->deleteData(key.c_str());
}

void Core::Config::RemoveSection( const String& section )
{
	mRudeConfig->deleteSection(section.c_str());
}