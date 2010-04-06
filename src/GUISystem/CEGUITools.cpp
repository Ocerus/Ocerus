#include "Common.h"

#include "GUIMgr.h"
#include "CEGUITools.h"

using namespace GUISystem;

/**
 * The PropertyCallback function is a callback used to translate textual data
 * from window layout. This callback should be used when loading window layouts
 * in CEGUI::WindowManager::loadWindowLayout.
 */
bool PropertyCallback(CEGUI::Window* window, CEGUI::String& propname, CEGUI::String& propvalue, void* userdata)
{
	OC_UNUSED(userdata);
	if (propname == "Text" &&
		propvalue.size() > 2 &&
		propvalue.at(0) == '$' &&
		propvalue.at(propvalue.size() - 1) == '$')
	{
		/// @todo Use StringMgr to translate textual data in GUI.
		CEGUI::String translatedText = "_" + propvalue.substr(1, propvalue.size() - 2);
		window->setProperty(propname, translatedText);
		return false;
	}
	return true;
}

CEGUI::Window* GUISystem::LoadWindowLayout(const CEGUI::String& filename, const CEGUI::String& name_prefix, const CEGUI::String& resourceGroup)
{
	CEGUI::Window* result = 0;
	CEGUI_EXCEPTION_BEGIN
	{
		result = CEGUI::WindowManager::getSingleton().loadWindowLayout(filename, name_prefix, resourceGroup, PropertyCallback);
	}
	CEGUI_EXCEPTION_END;
	return result;
}

/// Based on code from http://www.cegui.org.uk/wiki/index.php/MenuAndPopup
void GUISystem::ShowPopup(CEGUI::PopupMenu* popupMenu, const CEGUI::MouseEventArgs& me)
{
	//-------------------------------------------------------------------
	// Offset to ensure that the mouse cursor will be within the popup menu
	// This will automatically close the popup menu whe the mouse is moved;
	// without the offset the mouse is not really within the popup menu.
	static const float mouseOffset = 5.0f;

	//-------------------------------------------------------------------
	// Specifies whether the popup menu will open to the right (default) or
	// to the left of the mouse.  Opening to the left gives better results
	// with the Taharez scheme since the mouse cursor is not covering the
	// menu item.
	static const bool mouseToLeftOfPopupMenu = false;

	// Make the window the popup's parent
	//me.window->addChildWindow(popupMenu);

	// Position of the popup menu
	CEGUI::UVector2 popupPosition;

	// Ensure the popup menu will be fully displayed horizontally on the screen
	float screenWidth = CEGUI::System::getSingleton().getRenderer()->getDisplaySize().d_width;

	float popupMenuWidth = popupMenu->getWidth().d_offset;
	if(popupMenuWidth < screenWidth)
	{
		// The x coordinate is either the mouse position or the right side of the
		// screen minus the width of the popup menu.  This ensures that the popup
		// menu is fully displayed rather than clipped.
		float x;
		if(mouseToLeftOfPopupMenu)
		{
			x = (me.position.d_x + popupMenuWidth > screenWidth) ?
					(screenWidth - popupMenuWidth) : (me.position.d_x - mouseOffset);
		}
		else
		{
			x = (me.position.d_x < popupMenuWidth) ? 0.0f : me.position.d_x - popupMenuWidth + mouseOffset;
		}
		popupPosition.d_x = cegui_absdim(CEGUI::CoordConverter::screenToWindowX(*me.window, x));
	}
	else
	{
		// The popup menu is too wide to fit within the screen
		popupPosition.d_x = CEGUI::UDim(0.0f, 0.0f);
	}

	// Ensure the popup menu will be fully displayed vertically on the screen
	float screenHeight = CEGUI::System::getSingleton().getRenderer()->getDisplaySize().d_height;
	float popupMenuHeight = popupMenu->getHeight().d_offset;
	if(popupMenuHeight < screenHeight)
	{
		// The y coordinate is either the mouse position or the bottom side of the
		// screen minus the height of the popup menu.  This ensures that the popup
		// menu is fully displayed rather than clipped.
		float y = (me.position.d_y + popupMenuHeight > screenHeight) ?
				(screenHeight - popupMenuHeight) : (me.position.d_y - mouseOffset);
		popupPosition.d_y = cegui_absdim(CEGUI::CoordConverter::screenToWindowY(*me.window, y));
	}
	else
	{
		// The popup menu is too tall to fit within the screen
		popupPosition.d_y = CEGUI::UDim(0.0f, 0.0f);
	}

	// Position the context menu
	popupMenu->setPosition(popupPosition);

	// Show the popup menu
	popupMenu->show();

	// Ensure it appears on top of the other widgets
	popupMenu->moveToFront();
}
