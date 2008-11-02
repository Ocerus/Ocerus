#ifndef Config_h__
#define Config_h__

#include "../Utility/Settings.h"
#include "rudeconfig/config.h"


namespace Core
{
	/** Allows storing config informations needed to be saved by various parts of the program.
	*/
	class Config
	{
	public:
		Config(const string& filePath);
		~Config(void);

		/// Forces the config to be saved. However, it is saved automatically when deleted.
		bool Save(void);

		/// Value getters.
		//@{
		string GetString(const string& key, const string& defaultValue = "", const string& section = "General" );
		int32 GetInt32(const string& key, const int32 defaultValue = 0, const string& section = "General" );
		bool GetBool(const string& key, const bool defaultValue = false, const string& section = "General" );
		//@}

		/// Value setters.
		//@{
		void SetString(const string& key, const string& value, const string& section = "General" );
		void SetInt32(const string& key, const int32 value, const string& section = "General" );
		void SetBool(const string& key, const bool value, const string& section = "General" );
		//@}

		/// Remove whole section from the config.
		void RemoveSection(const string& section);
		/// Remove one key and its value from the config.
		void RemoveKey(const string& key, const string& section = "General" );

	private:
		/// Where is the config stored.
		string mFilePath;

		/// Helper.
		rude::Config mRudeConfig;

	};
}

#endif // Config_h__