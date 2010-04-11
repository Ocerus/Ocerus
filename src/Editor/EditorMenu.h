/// @file
/// Manages top menu in editor

#ifndef _EDITOR_EDITORMENU_H_
#define _EDITOR_EDITORMENU_H_

#include "Base.h"

namespace CEGUI
{
	class EventArgs;
	class Window;
}

namespace Editor
{

	class EditorMenu
	{
	public:
		/// Constructs an EditorMenu object.
		EditorMenu();

		/// Initializes the menu.
		void Init();
		
		/// @name CEGUI Callbacks
		//@{
			bool OnMouseEntersMenuItem(const CEGUI::EventArgs&);
			bool OnMouseLeavesMenuItem(const CEGUI::EventArgs&);
			bool OnMenuItemClicked(const CEGUI::EventArgs&);
			bool OnToolbarButtonClicked(const CEGUI::EventArgs&);
		//@}

	private:
		/// Creates menu items in AddComponent menu.
		void InitComponentMenu();

		/// Recursively subscribes every menu item's events to event handlers.
		void ConfigureMenu(CEGUI::Window* parent);

		/// Recursively subscribes every toolbar button's events to event handlers.
		void ConfigureToolbar(CEGUI::Window* parent);
	};

}

#endif // _EDITOR_EDITORMENU_H_
