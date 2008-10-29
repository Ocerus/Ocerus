#ifndef Config_h__
#define Config_h__

#include "../Utility/Settings.h"
#include "rudeconfig/config.h"


namespace Core
{
	class Config
	{
	public:
		Config(const string& filePath);
		~Config(void);

		bool Save(void);

		string GetString(const string& key, const string& defaultValue = "", const string& section = "General" );
		int32 GetInt32(const string& key, const int32 defaultValue = 0, const string& section = "General" );
		bool GetBool(const string& key, const bool defaultValue = false, const string& section = "General" );

		void SetString(const string& key, const string& value, const string& section = "General" );
		void SetInt32(const string& key, const int32 value, const string& section = "General" );
		void SetBool(const string& key, const bool value, const string& section = "General" );

		void RemoveSection(const string& section);
		void RemoveKey(const string& key, const string& section = "General" );

	private:
		string mFilePath;

		rude::Config mRudeConfig;

	};
}

#endif // Config_h__