/// @file
/// Manages GUI window for listing and manipulating resources.

#ifndef _EDITOR_RESOURCEWINDOW_H_
#define _EDITOR_RESOURCEWINDOW_H_

#include "Base.h"
#include "GUISystem/CEGUIForwards.h"

namespace Editor
{
	/// The ResourceWindow class manages GUI widget for listing and manipulating resources.
	class ResourceWindow
	{
	public:

		/// Constructs a ResourceWindow.
		ResourceWindow();

		/// Destroys the ResourceWindow.
		~ResourceWindow();

		/// Initializes the ResourceWindow.
		void Init();

		/// Deinitializes the ResourceWindow.
		void Deinit();

		/// Clears the items.
		void Clear();

		/// Updates the ResourceWindow.
		void Update(float32 delta);

		/// Refreshes the ResourceWindow, all resources get updated.
		void Refresh();

		/// Returns the ResourcePtr for the specified MenuItem window.
		ResourceSystem::ResourcePtr MenuItemToResourcePtr(const CEGUI::Window* menuItem);

		/// @name CEGUI Callbacks
		//@{
			bool OnDragContainerMouseButtonDown(const CEGUI::EventArgs&);
			bool OnDragContainerMouseButtonUp(const CEGUI::EventArgs&);
			bool OnDragContainerMouseDoubleClick(const CEGUI::EventArgs&);
		//@}

	private:
		enum ePopupItem
		{
			PI_INVALID = 0,
			PI_OPEN_SCENE
		};

		
		/// Updates resource pool. Returns true if resource pool was modified.
		bool UpdateResourcePool();
		void AddResourceToTree(const ResourceSystem::ResourcePtr& resource);
		void UpdateTree();

		/// @name Popup
		//@{
			void CreatePopupMenu();
			void DestroyPopupMenu();
			void OpenPopupMenu(ResourceSystem::ResourcePtr resource, float32 posX, float32 posY);
			void OnPopupMenuItemClicked(CEGUI::Window* menuItem);

			CEGUI::Window* mPopupMenu;
			CEGUI::Window* mResourceTypesPopupMenu;
			ResourceSystem::ResourcePtr mCurrentPopupResource;	
		//@}

		/// @name ItemEntry creation and caching
		//@{

			/// Creates an ItemEntry for resource.
			CEGUI::Window* CreateResourceItemEntry();

			/// Creates an ItemEntry for directory.
			CEGUI::Window* CreateDirectoryItemEntry();

			/// Sets the resource ItemEntry according to specified resource.
			/// @param itemEntry resource ItemEntry.
			/// @param resourceIndex index to ResourcePool
			void SetupResourceItemEntry(CEGUI::Window* itemEntry, uint resourceIndex);

			/// Sets the directory ItemEntry.
			void SetupDirectoryItemEntry(CEGUI::Window* itemEntry, const string& parentPath, const string& directory);

			/// Updates an ItemEntry.
			bool UpdateItemEntry(CEGUI::Window* itemEntry);

			/// Stores a resource/directory ItemEntry to cache.
			void StoreItemEntry(CEGUI::Window* itemEntry);

			/// Restores a resource ItemEntry from cache. If cache is empty, a newly created ItemEntry is returned.
			CEGUI::Window* RestoreResourceItemEntry();

			/// Restores a directory ItemEntry from cache. If cache is empty, a newly created ItemEntry is returned.
			CEGUI::Window* RestoreDirectoryItemEntry();

			/// Destroys the resource ItemEntry cache.
			void DestroyItemEntryCache();

			typedef vector<CEGUI::Window*> ItemEntryCache;

			ItemEntryCache mResourceItemEntryCache;
			ItemEntryCache mDirectoryItemEntryCache;
		//@}

		uint GetPathIndentLevel(const string& path);

		/// Sorting callback function. This function provides an order of items in the list
		/// for the sake of simulating tree widget.
		static bool SortCallback(const CEGUI::ItemEntry* first, const CEGUI::ItemEntry* second);

		static uint InvalidResourceIndex;

		float mUpdateTimer;
		
		typedef vector<ResourceSystem::ResourcePtr> ResourceList;
		ResourceList mResourcePool;
		vector<string> mDirectoryList;

		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mTree;
	};
}

#endif // EDITOR_RESOURCEWINDOW_H
