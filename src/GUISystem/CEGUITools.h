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
		CRITICAL_FAILURE("Last error was critical"); \
	}

namespace CEGUI
{
	class String;
}

namespace GUISystem
{
	void ShowPopup(CEGUI::PopupMenu* popupMenu, const CEGUI::MouseEventArgs& e);
}


#endif // _CEGUITools_H_