/// @file
/// Displays a list of prototypes in the project.
#ifndef _EDITOR_PROTOTYPEWINDOW_H_
#define _EDITOR_PROTOTYPEWINDOW_H_

#include "Base.h"
#include "GUISystem/CEGUIForwards.h"

namespace Editor
{
	/// Displays a list of prototypes in the project.
	class PrototypeWindow
	{
	public:

		/// Constructs a PrototypeWindow..
		PrototypeWindow();

		/// Destroys the PrototypeWindow.
		~PrototypeWindow();

		/// Initializes the PrototypeWindow.
		void Init();

		/// Deinitializes the PrototypeWindow.
		void Deinit();

		/// Updates the PrototypeWindow.
		void Update();

		/// Sets the currently selected entity prototype.
		void SetSelectedEntity(EntitySystem::EntityHandle entity);

		/// Shows a prompt for creating a new prototype.
		void ShowCreatePrototypePrompt();

		/// Creates a new prototype with specified name.
		void CreatePrototype(const string& prototypeName);

		/// Deletes the specified prototype.
		void DeletePrototype(EntitySystem::EntityHandle entity);

		/// Instantiates the specified prototype.
		void InstantiatePrototype(EntitySystem::EntityHandle entity);

	private:
		/// @name CEGUI Callbacks
		//@{
			bool OnListItemClicked(const CEGUI::EventArgs&);
			bool OnListItemDoubleClicked(const CEGUI::EventArgs&);
			bool OnListClicked(const CEGUI::EventArgs&);
		//@}

		/// @name ItemEntry caching
		//@{
			CEGUI::ItemEntry* CreatePrototypeItem();
			void SetupPrototypeItem(CEGUI::ItemEntry* prototypeItem, EntitySystem::EntityHandle mPrototype);
			void StorePrototypeItem(CEGUI::ItemEntry* prototypeItem);
			CEGUI::ItemEntry* RestorePrototypeItem();
			void DestroyItemCache();

			typedef vector<CEGUI::ItemEntry*> ItemCache;

			ItemCache mPrototypeItemCache;
		//@}

		/// @name Popup menu
		//@{
			enum ePopupItem
			{
				PI_INVALID = 0,
				PI_ADD_PROTOTYPE,
				PI_DELETE_PROTOTYPE,
				PI_INSTANTIATE_PROTOTYPE
			};

			void CreatePopupMenu();
			void DestroyPopupMenu();
			void OnPopupMenuItemClicked(CEGUI::Window* menuItem);

			CEGUI::Window* mPopupMenu;
		//@}

		void CreatePrototypePromptCallback(bool clickedOK, const string& text, int32 tag);

		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mList;
		EntitySystem::EntityHandle mCurrentPrototype;
	};
}

#endif // _EDITOR_PROTOTYPEWINDOW_H_