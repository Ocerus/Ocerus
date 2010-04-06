/// @file
/// Handling GUI of editor.

#ifndef _EDITORGUI_H_
#define _EDITORGUI_H_

#include "Base.h"
#include "EntitySystem/ComponentMgr/ComponentID.h"

/// Forwarded classes from CEGUI
namespace CEGUI
{
	class String;
	class Window;
	class EventArgs;
}

namespace GUISystem
{
	class VerticalLayout;
	class ViewportWindow;
}

namespace Editor
{
	class AbstractValueEditor;

	/// The EditorGUI class manages the editor GUI.
	class EditorGUI
	{
	public:

		/// Constructor.
		EditorGUI();

		/// Destructor.
		~EditorGUI();

		/// Loads editor GUI.
		void LoadGUI();

		/// Updates the GUI.
		void Update(float32 delta);

		/// Draws viewports managed by Editor
		void Draw(float32 delta);
		
		/// Updates the entity editor.
		void UpdateEntityEditorWindow();

		/// @name CEGUI Callbacks
		//@{
			bool OnComponentAddClicked(const CEGUI::EventArgs&);
			bool OnComponentRemoveClicked(const CEGUI::EventArgs&);
			bool OnComponentMenuOpened(const CEGUI::EventArgs&);
			bool OnMouseEntersMenuItem(const CEGUI::EventArgs&);
			bool OnMouseLeavesMenuItem(const CEGUI::EventArgs&);
			bool OnMouseLeavesPopupMenuItem(const CEGUI::EventArgs&);
			bool OnMenuItemClicked(const CEGUI::EventArgs&);
		//@}

		
	private:
		/// Recursively subscribes every menu item to the mouse enters/leaves/clicked events
		void ConfigureMenu(CEGUI::Window* parent, bool isMenubar);

		
		int32 mPropertyItemHeight;
		int32 mComponentGroupHeight;
		float32 mPropertyUpdateTimer;

		typedef vector<AbstractValueEditor*> PropertyEditors;
		PropertyEditors mPropertyEditors;
		GUISystem::VerticalLayout* mEntityEditorLayout;
		GUISystem::ViewportWindow* mTopViewport;
		GUISystem::ViewportWindow* mBottomViewport;
	};
}

#endif // _EDITORGUI_H_