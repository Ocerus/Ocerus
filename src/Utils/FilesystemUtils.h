/// @file
/// Utilities for file and directory processing.

#ifndef FilesystemUtils_h__
#define FilesystemUtils_h__

#include "../Setup/Settings.h"

namespace Utils
{
	/// Utility functions for working with files and directories.
	namespace FilesystemUtils
	{
		/// Copies the source directory to another. The new directory structure is created automatically.
		/// The destination directory mustn't be inside the source directory.
		void CopyDirectory(const string& source, const string& destination, const string& includeRegexp = ".*", const string& excludeRegexp = "");
	}
}

#endif // FilesystemUtils_h__
