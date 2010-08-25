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

		/// Returns the currently selected item.
		EntitySystem::EntityHandle GetSelectedItem();

		/// Returns the item at the given position in the tree.
		EntitySystem::EntityHandle GetItemAtIndex(size_t index);

		/// Sets the currently selected entity prototype.
		void SetSelectedEntity(const EntitySystem::EntityHandle entity);

		/// Creates a new prototype. A prompt for typing the name of the prototype is shown.
		void NewPrototype();

		/// Deletes the specified prototype.
		void DeletePrototype(EntitySystem::EntityHandle entity);

		/// Instantiates the specified prototype.
		void InstantiatePrototype(EntitySystem::EntityHandle entity);

		/// @name CEGUI Callbacks
		//@{
		bool OnDragContainerMouseButtonUp(const CEGUI::EventArgs&);
		bool OnWindowMouseButtonUp(const CEGUI::EventArgs&);
		//@}

	private:
		enum ePopupItem
		{
			PI_INVALID = 0,
			PI_ADD_PROTOTYPE,
			PI_DELETE_PROTOTYPE,
			PI_INSTANTIATE_PROTOTYPE
		};

		/// Creates the list of prototypes.
		void RebuildTree();

		void CreatePopupMenu();
		void DestroyPopupMenu();
		void OnPopupMenuItemClicked(CEGUI::Window* menuItem);
		void NewPrototypePromptCallback(bool clickedOK, const string& text, int32 tag);

		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mTree;
		int32 mSelectedIndex;
		EntitySystem::EntityList mItems;
		
		CEGUI::Window* mPopupMenu;
		EntitySystem::EntityHandle mCurrentPopupPrototype;
	};
}

#endif 
