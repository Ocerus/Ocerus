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

		void SetEntityID(const string& entityID);
		void SetEntityName(const string& entityName);
		void SetText(const CEGUI::String& windowName, const CEGUI::String& text);

		void UpdatePropertiesWindow(const EntitySystem::EntityHandle* newActiveEntity);

		typedef vector<CEGUI::Window*> CEGUIWindows;

		CEGUIWindows mComponents;
	};
}

#endif // EditorGUI_h__