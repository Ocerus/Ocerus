#include "Common.h"
#include "Config.h"
#include "rudeconfig/config.h"
#include <boost-1_41/boost/concept_check.hpp>

using namespace Core;

Config::Config(const string& filePath): mFilePath(filePath)
{
	ocInfo << "Loading config file '" << mFilePath << "'";

	OC_ASSERT(mFilePath.length()>0);

	// attempt to load the file
	mRudeConfig = new rude::Config();
	if (mRudeConfig->load(mFilePath.c_str()))
		ocInfo << "Config file loaded";
	else
		ocWarning << "Config file NOT loaded";
}

Config::~Config() 
{
	Save();
	delete mRudeConfig;
}


bool Core::Config::Save( void )
{
	ocInfo << "Saving config file '" << mFilePath << "'";

	if (mRudeConfig->save(mFilePath.c_str()))
	{
		ocInfo << "Config file saved";
		return true;
	}
	ocInfo << "Config file NOT saved";
	return false;
}

string Core::Config::GetString( const string& key, const string& defaultValue, const string& section )
{
	mRudeConfig->setSection(section.c_str()); // sets working section...all subsequent calls are related to this section.
	if (mRudeConfig->exists(key.c_str()))
		return mRudeConfig->getStringValue(key.c_str());
	return defaultValue;
}

int32 Core::Config::GetInt32( const string& key, const int32 defaultValue, const string& section )
{
	mRudeConfig->setSection(section.c_str());
	if (mRudeConfig->exists(key.c_str()))
		return mRudeConfig->getIntValue(key.c_str());
	return defaultValue;
}

bool Core::Config::GetBool( const string& key, const bool defaultValue, const string& section )
{
	mRudeConfig->setSection(section.c_str());
	if (mRudeConfig->exists(key.c_str()))
		return mRudeConfig->getBoolValue(key.c_str());
	return defaultValue;
}

void Core::Config::SetString( const string& key, const string& value, const string& section )
{
	mRudeConfig->setSection(section.c_str());
	mRudeConfig->setStringValue(key.c_str(), value.c_str());
}

void Core::Config::SetInt32( const string& key, const int32 value, const string& section )
{
	mRudeConfig->setSection(section.c_str());
	mRudeConfig->setIntValue(key.c_str(), value);
}

void Core::Config::SetBool( const string& key, const bool value, const string& section )
{
	mRudeConfig->setSection(section.c_str());
	mRudeConfig->setBoolValue(key.c_str(), value);
}

void Core::Config::GetSectionKeys(const string& section, vector< string >& keys)
{
	keys.clear();
	mRudeConfig->setSection(section.c_str());
	int dataCount = mRudeConfig->getNumDataMembers();
	keys.reserve(dataCount);
	for (int i = 0; i < dataCount; ++i)
		keys.push_back(mRudeConfig->getDataNameAt(i));
}

void Core::Config::RemoveKey( const string& key, const string& section )
{
	mRudeConfig->setSection(section.c_str());
	mRudeConfig->deleteData(key.c_str());
}

void Core::Config::RemoveSection( const string& section )
{
	mRudeConfig->deleteSection(section.c_str());
}