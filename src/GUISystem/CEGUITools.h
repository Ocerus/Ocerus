#ifndef _CEGUITools_H_
#define _CEGUITools_H_

#include "Base.h"

#include "CEGUI.h"

#define CEGUI_EXCEPTION_BEGIN try {
#define CEGUI_EXCEPTION_END } catch(const CEGUI::UnknownObjectException& e) { \
		ocError << "CEGUI Error: " << e.getMessage(); }

#define gCEGUIWM CEGUI::WindowManager::getSingleton()

namespace CEGUI
{
	class String;
}

namespace GUISystem
{
	///
	CEGUI::Window* LoadWindowLayout(const CEGUI::String& filename, const CEGUI::String& name_prefix = "", const CEGUI::String& resourceGroup = "");
}


#endif // _CEGUITools_H_