/// @file
/// Contains common includes and macros that are useful for writing GUI code.

#ifndef _GUISYSTEM_CEGUICOMMON_H_
#define _GUISYSTEM_CEGUICOMMON_H_

#include <CEGUI.h>

/// @name CEGUI exception handling
/// When calling CEGUI methods, it is often necessary to catch possible CEGUI exceptions. To do so,
/// use the following macros.
//@{
	/// The beginning of the CEGUI exception handling block.
	#define OC_CEGUI_TRY try {

	/// The end of the CEGUI exception handling block that handles non-critical exceptions.
	#define OC_CEGUI_CATCH \
		} catch(const CEGUI::Exception& e) { \
			ocError << "CEGUI Error: " << e.getMessage(); \
		}

	/// The end of the CEGUI exception handling block that handles critical exceptions.
	#define OC_CEGUI_CATCH_CRITICAL \
		} catch(const CEGUI::Exception& e) { \
			ocError << "CEGUI Error: " << e.getMessage(); \
			CRITICAL_FAILURE("Last error was critical"); \
		}
//@}

/// This function converts standard string to CEGUI string. It is necessary to use this function
/// in favor to the implicit conversion if your strings are UTF8 encoded.
inline CEGUI::String utf8StringToCEGUI(const string& utf8String)
{
	return CEGUI::String((CEGUI::utf8*)(utf8String.c_str()));
}


#endif // _GUISYSTEM_CEGUICOMMON_H_
