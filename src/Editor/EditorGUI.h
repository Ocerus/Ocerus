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
		void Update(const float32 delta);

		/// Updates the entity editor.
		void UpdateEntityEditorWindow();

		/// TO BE DELETED
		bool EntityPickerHandler(const CEGUI::EventArgs&);


	private:
		int32 mPropertyItemHeight;
		int32 mComponentGroupHeight;
		float32 mPropertyUpdateTimer;

		typedef vector<AbstractValueEditor*> PropertyEditors;
		PropertyEditors mPropertyEditors;
		GUISystem::VerticalLayout* mEntityEditorLayout;
		GUISystem::ViewportWindow* mViewport;
	};
}

#endif // _EDITORGUI_H_