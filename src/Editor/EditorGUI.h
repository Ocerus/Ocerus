#ifndef EditorGUI_h__
#define EditorGUI_h__

#include "Base.h"

namespace CEGUI
{
	class String;
	class Window;
	class EventArgs;
}


namespace Editor
{

	/// The EditorGUI class provides an interface to work with editor GUI.
	class EditorGUI
	{
	public:

		/// Constructor.
		EditorGUI();

		/// Destructor.
		~EditorGUI();

		/// Loads editor GUI.
		void LoadGUI();

		/// Sets the active entity to newActiveEntity.
		void SetCurrentEntity(const EntitySystem::EntityHandle* newActiveEntity);

		bool EntityPickerHandler(const CEGUI::EventArgs&);


	private:

		void SetText(const CEGUI::String& windowName, const CEGUI::String& text);

		void UpdateEntityEditorWindow(const EntitySystem::EntityHandle* newActiveEntity);

		void appendEntityEditorItem(CEGUI::Window* componentGroup, const string& itemName, const string& itemValue, int itemPos, const string& itemTooltip, bool readOnly = false);

		typedef vector<CEGUI::Window*> CEGUIWindows;

		CEGUIWindows mComponents;
	};
}

#endif // EditorGUI_h__