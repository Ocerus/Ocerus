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

		/// Updates the entity editor.
		void UpdateEntityEditorWindow();

		/// TO BE DELETED
		bool EntityPickerHandler(const CEGUI::EventArgs&);


	private:

		void AppendPropertyItem(CEGUI::Window* componentGroup, EntitySystem::ComponentID componentID, const string& propertyName, const string& propertyValue, const string& propertyKey, int propertyPos, const string& propTooltip, bool readOnly = false);

		bool OnEntityEditorItemChanged(const CEGUI::EventArgs& e);


		int mPropertyItemHeight;
		int mComponentGroupHeight;

	};
}

#endif // _EDITORGUI_H_