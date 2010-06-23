/// @file
/// Manages top menu in editor.

#ifndef _EDITOR_EDITORMENU_H_
#define _EDITOR_EDITORMENU_H_

#include "Base.h"
#include "GUISystem/MessageBox.h"

namespace CEGUI
{
	class EventArgs;
	class Window;
	class RadioButton;
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
			bool OnToolbarButtonStateChanged(const CEGUI::EventArgs&);
		//@}

		/// MessageBox callback
		void OnMessageBoxClicked(GUISystem::MessageBox::eMessageBoxButton button, int32 tag);

		/// FolderSelector callback
		void OnFolderSelected(const string& path, const string& editboxValue, bool canceled, int32 t);

		/// Updates the list of scenes in scene menu.
		void UpdateSceneMenu();

		/// Switches the currently selected tool button.
		void SwitchToolButton(uint32 selectedButtonIndex);

		/// Switches the currently selected action button.
		void SwitchActionButton(uint32 selectedButtonIndex);

		enum eMessageBoxTags { MBT_QUIT };
		enum eFolderSelectorTags { FST_CREATEPROJECT, FST_OPENPROJECT };

	private:

		typedef vector<CEGUI::RadioButton*> RadioButtonList;
		RadioButtonList mToolButtons;
		RadioButtonList mActionButtons;

		/// Creates menu items in AddComponent menu.
		void InitComponentMenu();

		/// Recursively subscribes every menu item's events to event handlers.
		void ConfigureMenu(CEGUI::Window* parent);

		/// Recursively subscribes every toolbar button's events to event handlers.
		void ConfigureToolbar(CEGUI::Window* parent);
	};

}

#endif // _EDITOR_EDITORMENU_H_
