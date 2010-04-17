/// @file
/// String system entry point.

#ifndef _STRINGMGR_H_
#define _STRINGMGR_H_

#include "Base.h"
#include "TextData.h"

/// Macro for easier use
#define gStringMgrSystem StringSystem::StringMgr::GetSystem()
#define gStringMgrProject StringSystem::StringMgr::GetProject()

/// String system manages all text data in the system.
/// However, only the text data presented to the end user is managed by this system. For example, debug strings
/// used by programmers are not included.
namespace StringSystem
{
	/// This class is a special type of resource manager, designed to work with strings (text resources).
	///	@remarks Its main purpose is to store and index text data for further use. You should always load string
	///	data via this class.
	class StringMgr
	{
	public:

		/// If the basepath parameter is provided, the manager will relate all files to this path.
		/// Global root is in the ResourceMgr's basepath (according to the first parameter)
		/// and this basepath is relative to it.
		StringMgr(const ResourceSystem::eBasePathType basePathType, const string& basePath = "strings/");

		~StringMgr(void);

		/// Loads all strings for a given language and unloads a previous data.
		/// Every language's strings should be stored in a separate directory, preferabely using 
		/// ISO 639-1 codes (http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes)
		/// and ISO 3166-1 alpha-2 codes (http://en.wikipedia.org/wiki/ISO_3166-1_alpha-2)
		bool LoadLanguagePack(const string& lang = "", const string& country = "");

		/// Adds all text resources in a given directory and loads them into the memory.
		bool LoadDataFromDir(const string& path, const string& includeRegexp = ".*", const string& excludeRegexp = "",
		  bool recursive = false);

		/// Loads the text resource from a provided file.
		bool LoadDataFromFile(const string& filepath, bool pathRelative = true);

		/// Unloads all data of the manager.
		bool UnloadData(void);

		/// Returns a pointer to a text data specified by a given search key and group.
		const TextData* GetTextDataPtr(const StringKey& group, const StringKey& key);

		/// Returns a text data specified by a given search key and group.
		/// Note that this may be quite slow if strings are long. Returning a ptr should be preffered.
		const TextData GetTextData(const StringKey& group, const StringKey& key);
		
		/// Returns a pointer to a text data specified by a given search key from a default group.
		const TextData* GetTextDataPtr(const StringKey& key);

		/// Returns a text data specified by a given search key from a default group.
		/// Note that this may be quite slow if strings are long. Returning a ptr should be preffered.
		const TextData GetTextData(const StringKey& key);
		
		/// Initializes the system and the project string managers.
		static void Init(const string& systemBasePath = "strings/", const string& projectBasePath = "strings/");
		
		/// Returns whether the system and the project string managers are initialized.
		static bool IsInited();
		
		/// Deinitializes the system and the project string managers.
		static void Deinit(); 
		
		/// Returns the system manager.
		static StringMgr& GetSystem();
		
		/// Returns the project manager.
		static StringMgr& GetProject();
		
		/// Invalid text data returned when the key of the text data is not found.
		static const TextData NullTextData;

	private:

		/// Container used to store the parsed data (the same type used is in TextRes!).
		GroupTextDataMap mGroupTextDataMap;

		/// Current language of loaded texts.
		string mLanguage;
		
		/// Current country of loaded texts.
		string mCountry; 

		/// Basepath for string data. The root is in the ResourceMgr's basepath.
		string mBasePath;
		
		/// Type of a base path which the ResourceMgr is called with.
		ResourceSystem::eBasePathType mBasePathType;
		
		/// Returns string reprezentation of a type of this manager instance.
		string GetNameOfManager();
		
		/// The system string manager.
		static StringMgr* msSystem;
		
		/// The project string manager.
		static StringMgr* msProject;

	};
}

#endif