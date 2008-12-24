#ifndef _STRINGMGR_H_
#define _STRINGMGR_H_

#include "../Utility/Singleton.h"
#include "TextResource.h"
#include <map>
#include <string>

/// @name Macro for easier use
#define gStringMgr StringSystem::StringMgr::GetSingleton()

namespace StringSystem
{
	/** This class is a special type of resource manager, designed to work with strings (text resources)
		Its main purpose is to store and index text data for further use. You should always load string
		data via this class.
	*/
	class StringMgr : public Singleton<StringMgr>
	{
	public:
		/// @name @param basepath for string data. Root is in the ResMgr's basepath
		StringMgr(const string& basepath = "strings/");
		~StringMgr(void);

		/** Load all strings for a given language.
		    Every language's strings should be stored in a separate directory, preferabely using 
		    ISO 639-1 codes (http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes)
		   
		    This simply calls LoadDataFromDir. It's possible to load other strings with this method later on
		    (not necessarily from the same language pack -- maybe there should be some language check, dunno)
		*/
		bool LoadLanguagePack(const string& lang = "en");

		// Change language -- unload all strings and load them again with LoadStrings(lang)
		// bool ChangeLanguage(const string& lang);

		/// @name We can use these to load strings that are not defined by/for language pack (ie common names for weapons?)
		//@{
		/// @name wrapper around gResourceMgr.AddResourceDirToGroup / LoadResourcesInGroup
		bool LoadDataFromDir(const string& path, const string& includeRegexp = "*.*", const string& excludeRegexp = "");
		/// @name wrapper around gResourceMgr.AddResourceFileToGroup
		bool LoadDataFromFile(const string& filepath, ResourceSystem::Resource::eType type = ResourceSystem::Resource::TYPE_AUTODETECT, bool pathRelative = true);
		//@}

		/// @name unload all the data (using UnloadResourcesInGroup)
		bool UnloadData(void);

		/// @name return pointer to text data
		const TextData* GetTextDataPtr(const StringKey& key);
		/** Return text data. Note that this may be slow if strings are long. (It has to copy
		    whole TextData structure). Returning a ptr should be preffered way to access data.
		*/
		const TextData GetTextData(const StringKey& key);
	private:
		/// @name container used to store data (same type used in TextRes!)
		TextDataMap mTextDataMap;
		/// @name actual language
		string mLanguage;
		/// @name Basepath for string data. Root is in the ResMgr's basepath
		string mBasePath;
	};
}

#endif