#include "TextResource.h"
#include "../Common.h"
#include "../Utility/DataContainer.h"
#include <string>

using namespace StringSystem;

ResourceSystem::ResourcePtr TextResource::CreateMe(void)
{
	return ResourceSystem::ResourcePtr(DYN_NEW TextResource());
}

bool TextResource::LoadImpl()
{
	InputStream& is = OpenInputStream(ISM_TEXT);
	char buf[4096];
	string key = "";
	string text = "";
	bool store = false;
	bool multiline = false;
	while (is.good())
	{
		is.getline(buf, 4096);
		/// gLogMgr.LogMessage(" !! RAW LINE DATA !! " + (string)buf);
		if (buf[0] == '{' && buf[1] == '-')
		{
			multiline = true;
		}
		else if (buf[0] == '-' && buf[1] == '}')
		{
			multiline = false;
		}
		else if ((buf[0] != '#') && !(!multiline && buf[0] == '\0'))
		{
			if (buf[0] == ':' && !multiline)
			{
				if (store) 
				{
					text.resize(text.size()-1);
					/// gLogMgr.LogMessage("StringMgr: Saving key: " + key + " and data: " + text);
					mTextDataMap.insert(std::pair<string, TextData>(key,text));
					text.clear();
					key.clear();
				}
				key = buf+1;
				/// gLogMgr.LogMessage("StringMgr: Loading key: " + key);
				store = true;
			}
			else
			{
				text = text + buf + '\n';
				/// gLogMgr.LogMessage("StringMgr: Loading data: " + text);
			}
		}
	}
	return true;
}

bool TextResource::UnloadImpl()
{
	mTextDataMap.clear();
	return true;
}

TextData TextResource::GetTextData(const string& key)
{
	return mTextDataMap[key];
}

TextDataMap TextResource::GetTextDataMap()
{
	return mTextDataMap;
}