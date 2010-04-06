#ifndef _CEGUITools_H_
#define _CEGUITools_H_

#include "Base.h"

#include "CEGUI.h"

#define CEGUI_EXCEPTION_BEGIN try {
#define CEGUI_EXCEPTION_END } catch(const CEGUI::Exception& e) { \
		ocError << "CEGUI Error: " << e.getMessage(); \
	}
#define CEGUI_EXCEPTION_END_CRITICAL } catch(const CEGUI::Exception& e) { \
		ocError << "CEGUI Error: " << e.getMessage(); \
		ocInfo << "Critical failure, shutting down..."; \
		exit(1); \
	}

#define gCEGUIWM CEGUI::WindowManager::getSingleton()

namespace CEGUI
{
	class String;
}

namespace GUISystem
{
	///
	CEGUI::Window* LoadWindowLayout(const CEGUI::String& filename, const CEGUI::String& name_prefix = "", const CEGUI::String& resourceGroup = "");

	void ShowPopup(CEGUI::PopupMenu* popupMenu, const CEGUI::MouseEventArgs& e);
}


#endif // _CEGUITools_H_