/// @file
/// Manages popup menus.
#ifndef _GUISYSTEM_POPUPMGR_H_
#define _GUISYSTEM_POPUPMGR_H_

#include "Base.h"
#include "CEGUIForwards.h"
#include "Utils/Callback.h"

/// Fast access to the popup manager.
#define gPopupMgr GUISystem::GUIMgr::GetSingleton().GetPopupMgr()

namespace GUISystem
{
	/// The PopupMgr class manages popup menus. To add a popup menu to your code, use the CreatePopupMenu
	/// and the CreateMenuItem method to create the menu and its items. When your popup menu is constructed,
	/// save a pointer to it and whenever you wish to display it, pass it to the ShowPopup() method, together
	/// with a callback to your callback method.
	class PopupMgr
	{
	public:
		/// Callback type that signals that user clicked a popup menu item. Parameters are:
		/// @param clickedItem The item that was clicked.
		typedef Callback1<void, CEGUI::Window*> Callback;

		/// Constructs a PopupMgr.
		PopupMgr();

		/// Destroys the PopupMgr.
		~PopupMgr();

		/// Shows a popup menu.
		/// @param popupWindow Popup menu window element. Note that it MUST be created using CreatePopupMenu() and CreateMenuItem().
		/// @param posX The x coordinate of the popup click.
		/// @param posY the y coordinate of the popup click.
		/// @param callback Callback that is called when an item is clicked.
		void ShowPopup(CEGUI::Window* popupWindow, float posX, float posY, Callback callback);

		/// Hides current popup menu.
		void HidePopup();

		/// Constructs an empty popup menu (or submenu) and returns it. The popup menu can be filled with
		/// menu items by passing menu items created with PopupMgr::CreateMenuItem() method to
		/// CEGUI::Window::addChild() method.
		/// @param windowName The CEGUI name of the window to be created.
		CEGUI::Window* CreatePopupMenu(const CEGUI::String& windowName);

		/// Destroys a popup menu. You should always use this method to destroy popup menus created by CreatePopupMenu(),
		void DestroyPopupMenu(CEGUI::Window* popupMenu);
		
		/// Constructs a menu item that can be inserted into a popup menu. Do not forget to insert it into your
		/// popup menu by calling the CEGUI::Window::addChild() method of the popup menu object.
		/// @param windowName The CEGUI name of the window to be created.
		/// @param text Text of the menu item.
		/// @param tooltip Tooltip of the menu item.
		/// @param id ID of the created window.
		CEGUI::Window* CreateMenuItem(const CEGUI::String& windowName, const CEGUI::String& text, const CEGUI::String& tooltip, CEGUI::uint id = 0);
		
		/// Destroys a menu item. You should always use this method to destroy menu items created by CreateMenuItem().
		void DestroyMenuItem(CEGUI::Window* menuItem);

		/// Hides the current popup if supplied coords do not hit the popup.
		void DoAutoHide(float posX, float posY);

	private:
		/// Called when a menuitem of the current popup menu is clicked.
		bool OnMenuItemClicked(const CEGUI::EventArgs&);

		/// Called when the popup menu is opened.
		bool OnPopupMenuOpened(const CEGUI::EventArgs&);

		/// Closes all submenus of the specified popupMenu.
		void CloseSubmenus(CEGUI::Window* popupMenu);

		CEGUI::Window* mCurrentPopupWindow;
		Callback mCurrentCallback;
	};
}

#endif // _GUISYSTEM_POPUPMGR_H_
