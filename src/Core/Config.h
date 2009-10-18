/// @file
/// System for storing configuration data.

#ifndef Config_h__
#define Config_h__

#include "Base.h"

namespace rude { class Config; }

namespace Core
{
	/// Allows storing configuration data needed to be saved by various parts of the program.
	class Config
	{
	public:

		/// Constructs the config to operate on the specified file.
		Config(const string& filePath);

		/// Destructs the config and closes all files open.
		~Config(void);

		/// Forces the config to be saved. However, it is saved automatically when the instance is being destroyed.
		bool Save(void);

		/// Returns string data based on the provided search key and (optional) section.
		/// @remarks
		/// If the key doesn't exist, defaultValue is returned.
		string GetString(const string& key, const string& defaultValue = "", const string& section = "General" );

		/// Returns integer data based on the provided search key and (optional) section.
		/// @remarks
		/// If the key doesn't exist, defaultValue is returned.
		int32 GetInt32(const string& key, const int32 defaultValue = 0, const string& section = "General" );

		/// Returns boolean data based on the provided search key and (optional) section.
		/// @remarks
		/// If the key doesn't exist, defaultValue is returned.
		bool GetBool(const string& key, const bool defaultValue = false, const string& section = "General" );

		/// Saves string data to the specified key and (optional) section.
		void SetString(const string& key, const string& value, const string& section = "General" );

		/// Saves integer data to the specified key and (optional) section.
		void SetInt32(const string& key, const int32 value, const string& section = "General" );

		/// Saves boolean data to the specified key and (optional) section.
		void SetBool(const string& key, const bool value, const string& section = "General" );

		/// Removes the whole section from the config.
		void RemoveSection(const string& section);

		/// Removes one key and its value from the config.
		void RemoveKey(const string& key, const string& section = "General" );

	private:

		/// Where is the config stored.
		string mFilePath;

		/// Implementation details.
		rude::Config* mRudeConfig;

	};
}

#endif // Config_h__