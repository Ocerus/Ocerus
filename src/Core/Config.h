#ifndef Config_h__
#define Config_h__

#include "../Utility/Settings.h"

/// @name  Forward declarations
//@{
namespace rude { class Config; }
//@}

namespace Core
{
	/** Allows storing config informations needed to be saved by various parts of the program.
	*/
	class Config
	{
	public:
		Config(const String& filePath);
		~Config(void);

		/// @name Forces the config to be saved. However, it is saved automatically when deleted.
		bool Save(void);

		/// @name  Value getters.
		//@{
		String GetString(const String& key, const String& defaultValue = "", const String& section = "General" );
		int32 GetInt32(const String& key, const int32 defaultValue = 0, const String& section = "General" );
		bool GetBool(const String& key, const bool defaultValue = false, const String& section = "General" );
		//@}

		/// @name  Value setters.
		//@{
		void SetString(const String& key, const String& value, const String& section = "General" );
		void SetInt32(const String& key, const int32 value, const String& section = "General" );
		void SetBool(const String& key, const bool value, const String& section = "General" );
		//@}

		/// @name Remove whole section from the config.
		void RemoveSection(const String& section);
		/// @name Remove one key and its value from the config.
		void RemoveKey(const String& key, const String& section = "General" );

	private:
		/// @name Where is the config stored.
		String mFilePath;

		/// @name Helper.
		rude::Config* mRudeConfig;

	};
}

#endif // Config_h__