/// @file
/// Manages GUI for listing resources.

#ifndef EDITOR_RESOURCEWINDOW_H
#define EDITOR_RESOURCEWINDOW_H

#include "Base.h"
#include "EditorMenu.h"
#include "GUISystem/CEGUIForwards.h"

namespace Editor
{

	/// The ResourceWindow class manages GUI widget for listing resources.
	class ResourceWindow
	{
	public:

		/// Constructs a ResourceWindow.
		ResourceWindow();

		/// Destroys the ResourceWindow.
		~ResourceWindow();

		/// Initializes the ResourceWindow.
		void Init();

		/// Refreshes the tree.
		inline void Refresh() { RebuildTree(); }

		/// Returns the ResourcePtr to resource in ResourceWindow's cached list of resources on given index.
		ResourceSystem::ResourcePtr GetItemAtIndex(size_t index);

		/// @name CEGUI Callbacks
		//@{
		bool OnDragContainerMouseButtonDown(const CEGUI::EventArgs&);
		bool OnDragContainerMouseButtonUp(const CEGUI::EventArgs&);
		bool OnDragContainerMouseDoubleClick(const CEGUI::EventArgs&);
		bool OnRefreshButtonClicked(const CEGUI::EventArgs&);
		//@}

	private:
		enum ePopupItem
		{
			PI_INVALID = 0,
			PI_OPEN_SCENE
		};

		/// Refreshes cached list of resources and builds resource tree according to the refreshed list.
		void RebuildTree();

		void CreatePopupMenu();
		void DestroyPopupMenu();
		void SetCurrentResourceType(ResourceSystem::eResourceType type);
		void OnPopupMenuItemClicked(CEGUI::Window* menuItem);
		
		/// Cached list of resources
		vector<ResourceSystem::ResourcePtr> mItems;

		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mTree;

		CEGUI::Window* mPopupMenu;
		CEGUI::Window* mResourceTypesPopupMenu;
		ResourceSystem::ResourcePtr mCurrentPopupResource;
	};
}

#endif // EDITOR_RESOURCEWINDOW_H
