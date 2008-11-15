#ifndef _STRINGMGR_H_
#define _STRINGMGR_H_

#include "../Utility/Singleton.h"
#include "TextResource.h"
//#include "StringKey.h"
#include <map>
#include <string>

/// Macro for easier use
#define gStringMgr StringSystem::StringMgr::GetSingleton()

namespace StringSystem
{
	#define BASE_PATH "strings/"
	class StringMgr : public Singleton<StringMgr>
	{
	public:
		StringMgr();
		~StringMgr();

		/* Load all strings for a given language.
		   Every language's strings should be stored in a separate directory, preferabely using 
		   ISO 639-1 codes (http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes)
		   
		   This simply calls LoadDataFromDir, however it's possible to load other strings with this method later on
		   (not necessarily from the same language pack -- maybe there should be some language check, dunno)
		*/
		bool LoadStrings(const string& lang = "en");

		/// Change language -- unload all strings and load them again with LoadStrings(lang)
		/// bool ChangeLanguage(const string& lang);

		/// wrapper around gResourceMgr.AddResourceDirToGroup / LoadResourcesInGroup
		bool LoadDataFromDir(const string& path, const string& includeRegexp = "*.*", const string& excludeRegexp = "");
		/// wrapper around gResourceMgr.AddResourceFileToGroup
		bool LoadDataFromFile(const string& filepath, ResourceSystem::Resource::eType type = ResourceSystem::Resource::TYPE_AUTODETECT, bool pathRelative = true);
		
		/// unload all the data (using UnloadResourcesInGroup)
		bool UnloadData();

		/* Actually, you can use any key you want, we may add some check if needed.
		   When loaded, strings are stored in map<string, TextData>
		   Data file format is simple
		   :BLOCK determines new block, loading starts at next line and continues
		   until next section is found
		*/
		/// return pointer to text data
		TextData* GetTextDataPtr(const string& key);
		/// return whole data structure
		TextData GetTextData(const string& key);
	private:
		TextDataMap mTextDataMap;
		string mLanguage;
	};
}



#endif