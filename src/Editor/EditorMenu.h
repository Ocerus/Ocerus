/// @file
/// Manages top menu and toolbar in editor.

#ifndef _EDITOR_EDITORMENU_H_
#define _EDITOR_EDITORMENU_H_

#include "Base.h"
#include "GUISystem/CEGUIForwards.h"

namespace Editor
{
	/// The EditorMenu class manages the top menubar of the editor and editor toolbar.
	class EditorMenu
	{
	public:
		/// Constructs an EditorMenu object.
		EditorMenu();

		/// Destroys the EditorMenu object.
		~EditorMenu();

		/// Initializes the menu.
		void Init();

		/// Updates the list of scenes in scene menu.
		void UpdateSceneMenu();

		/// Updates the enabled/disabled state of items in menu.
		void UpdateItemsEnabled();

		/// Switches the currently selected tool button.
		void SwitchToolButton(uint32 selectedButtonIndex);

		/// Switches the currently selected action button.
		void SwitchActionButton(uint32 selectedButtonIndex);

	private:

		/// Items in top menu
		enum eMenuItem
		{
			MI_INVALID = 0,
			MI_FILE_CREATEPROJECT,
			MI_FILE_OPENPROJECT,
			MI_FILE_CLOSEPROJECT,
			MI_FILE_QUIT,
			MI_SCENE_NEW,
			MI_SCENE_OPEN,
			MI_SCENE_SAVE,
			MI_SCENE_CLOSE,
			MI_EDIT_NEWENTITY,
			MI_EDIT_NEWCOMPONENT,
			MI_EDIT_DUPLICATEENTITY,
			MI_EDIT_DUPLICATESELECTEDENTITIES,
			MI_EDIT_DELETEENTITY,
			MI_EDIT_DELETESELECTEDENTITIES,
			MI_EDIT_CREATEPROTOTYPE,
			MI_HELP_USERDOCUMENTATION,
			MI_HELP_SHORTCUTS,
			MI_HELP_ABOUT
		};

		/// Toolbar groups
		enum eToolbarGroup
		{
			TG_ACTION_TOOLS = 0,
			TG_EDIT_TOOLS
		};

		/// @name CEGUI Callbacks
		//@{
			bool OnTopMenuDeactivated(const CEGUI::EventArgs&);
			bool OnMenuItemClicked(const CEGUI::EventArgs&);
			bool OnToolbarButtonStateChanged(const CEGUI::EventArgs&);
		//@}

		void InitMenu();
		void InitToolbar();
		CEGUI::Window* CreateMenuItem(const CEGUI::String& name, const CEGUI::String& text, const CEGUI::String& tooltip, size_t tag, bool enabled = true);
		CEGUI::Window* CreatePopupMenu(const CEGUI::String& name);
		CEGUI::RadioButton* CreateToolbarItem(const CEGUI::String& name, const CEGUI::String& imageName, const CEGUI::String& tooltip, size_t tag, size_t groupID);

		CEGUI::PopupMenu* mTopMenu;
		CEGUI::Window* mOpenSceneSubmenu;
		CEGUI::Window* mComponentsSubmenu;
		CEGUI::Window* mDeploySubmenu;

		typedef vector<CEGUI::RadioButton*> RadioButtonList;
		RadioButtonList mToolButtons;
		RadioButtonList mActionButtons;
	};
}

#endif // _EDITOR_EDITORMENU_H_
