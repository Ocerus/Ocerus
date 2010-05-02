/// @file
/// Displays a list of prototypes in the project.

#ifndef PrototypeWindow_h__
#define PrototypeWindow_h__

#include "Base.h"
#include "EditorMenu.h"
#include "GUISystem/CEGUIForwards.h"

namespace Editor
{
	/// Displays a list of prototypes in the project.
	class PrototypeWindow
	{
	public:

		/// Constructs a ResourceWindow.
		PrototypeWindow();

		/// Destroys the ResourceWindow.
		~PrototypeWindow();

		/// Initializes the ResourceWindow.
		void Init();

		/// Refreshes the tree.
		inline void Refresh() { RebuildTree(); }

		/// Returns the item at the given position in the tree.
		EntitySystem::EntityHandle GetItemAtIndex(size_t index);

		/// @name CEGUI Callbacks
		//@{
		bool OnDragContainerMouseButtonUp(const CEGUI::EventArgs&);
		bool OnWindowMouseButtonUp(const CEGUI::EventArgs&);
		//@}

	private:

		/// Creates the list of prototypes.
		void RebuildTree();

		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mTree;
		EntitySystem::EntityList mItems;
	};
}

#endif 
