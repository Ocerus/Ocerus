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

namespace Editor
{
	class IValueEditor;

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
		void CreateValueEditorWidgets(IValueEditor* editor, CEGUI::Window* componentGroup, int32& currentY);

		int32 mPropertyItemHeight;
		int32 mComponentGroupHeight;
		float32 mPropertyUpdateTimer;

		typedef vector<IValueEditor*> PropertyEditors;
		PropertyEditors mPropertyEditors;
	};
}

#endif // _EDITORGUI_H_