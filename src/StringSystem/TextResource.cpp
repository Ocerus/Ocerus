#include "Common.h"
#include "TextResource.h"

using namespace StringSystem;

const string TextResource::GroupNameKey = "group";

ResourceSystem::ResourcePtr TextResource::CreateMe(void)
{
	return ResourceSystem::ResourcePtr(new TextResource());
}

TextResource::~TextResource(void)
{
	UnloadImpl();
}

size_t TextResource::LoadImpl()
{
	InputStream& is = OpenInputStream(ISM_TEXT);
	size_t dataSize = 0;
	string line = "";
	bool first = true;
	while (is.good())
	{
	  std::getline(is, line);
	  if (!line.empty())
	  {
	    if (line[0] == '#') { continue; }
	    string::size_type pos = line.find_first_of('=');
	    if (pos != string::npos && pos != 0)
	    {
	      string key = line.substr(0, pos);
	      string value = line.substr(pos + 1);
	      if (first && key == TextResource::GroupNameKey)
	      {
	        mGroupName = value;
	      } else { 
	        mTextDataMap.insert(pair<StringKey, TextData>(key, (const CEGUI::utf8*)value.c_str()));
	      }
	      dataSize += line.length();
	      first = false;
	    }
	  }
	}
	CloseInputStream();
	return dataSize;
}

bool TextResource::UnloadImpl()
{
	mTextDataMap.clear();
	return true;
}

const TextData* TextResource::GetTextDataPtr(const StringKey& key)
{
	EnsureLoaded();
	const TextData* returnValue = &mTextDataMap[key];
	if (*returnValue == "") {
		ocError << "TextResource: Index " << key << " doesn't exist. Return value set to empty TextData";
	}
	return returnValue;
}

const TextData TextResource::GetTextData(const StringKey& key)
{
	EnsureLoaded();
	const TextData returnValue = mTextDataMap[key];
	if (returnValue == "") {
		ocError << "TextResource: Index " << key << " doesn't exist. Return value set to empty TextData";
	}
	return returnValue;
}

const TextDataMap* TextResource::GetTextDataMap(void)
{
	EnsureLoaded();
	return &mTextDataMap;
}
