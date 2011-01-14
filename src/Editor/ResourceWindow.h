/// @file
/// Manages GUI window for listing and manipulating resources.

#ifndef _EDITOR_RESOURCEWINDOW_H_
#define _EDITOR_RESOURCEWINDOW_H_

#include "Base.h"
#include "GUISystem/CEGUIForwards.h"
#include "GUISystem/MessageBox.h"

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
		void Update();

		/// Returns the ResourcePtr for the specified ItemEntry window. It can be used to retrieve
		/// ResourcePtr for drag 'n' drop source when receiving drag 'n' drop event.
		ResourceSystem::ResourcePtr ItemEntryToResourcePtr(const CEGUI::Window* itemEntry);

	private:
		/// @name CEGUI Callbacks
		//@{
			bool OnTreeItemClicked(const CEGUI::EventArgs&);
			bool OnTreeItemDoubleClicked(const CEGUI::EventArgs&);
		//@}

		/// @name Popup
		//@{
			enum ePopupItem
			{
				PI_INVALID = 0,
				PI_OPEN_SCENE,
				PI_RENAME_SCENE
			};

			void CreatePopupMenu();
			void DestroyPopupMenu();
			void OpenPopupMenu(ResourceSystem::ResourcePtr resource, float32 posX, float32 posY);
			void OnPopupMenuItemClicked(CEGUI::Window* menuItem);
			void OnRenameScenePromtboxConfirmed(bool clickedOK, const string& text, int32 tag);

			CEGUI::Window* mPopupMenu;
			CEGUI::Window* mResourceTypesPopupMenu;
			ResourceSystem::ResourcePtr mCurrentPopupResource;	
		//@}

		/// @name ItemEntry creation, setting and caching
		//@{

			/// Creates an ItemEntry for resource.
			CEGUI::Window* CreateResourceItemEntry();

			/// Creates an ItemEntry for directory.
			CEGUI::Window* CreateDirectoryItemEntry();

			/// Sets the resource ItemEntry according to specified resource.
			/// @param itemEntry resource ItemEntry.
			/// @param resourceIndex index to ResourcePool
			void SetupResourceItemEntry(CEGUI::Window* itemEntry, uint32 resourceIndex);

			/// Sets the directory ItemEntry.
			void SetupDirectoryItemEntry(CEGUI::Window* itemEntry, const string& parentPath, const string& directory);

			/// Stores a resource/directory ItemEntry to cache.
			void StoreItemEntry(CEGUI::Window* itemEntry);

			/// Restores a resource ItemEntry from cache. If cache is empty, a newly created ItemEntry is returned.
			CEGUI::Window* RestoreResourceItemEntry();

			/// Restores a directory ItemEntry from cache. If cache is empty, a newly created ItemEntry is returned.
			CEGUI::Window* RestoreDirectoryItemEntry();

			/// Updates an ItemEntry.
			bool UpdateItemEntry(CEGUI::Window* itemEntry);

			/// Destroys the resource ItemEntry cache.
			void DestroyItemEntryCache();

			typedef vector<CEGUI::Window*> ItemEntryCache;

			ItemEntryCache mResourceItemEntryCache;
			ItemEntryCache mDirectoryItemEntryCache;
		//@}

		/// @name Tree manipulation
		//@{

			/// Updates resource pool. Returns true if resource pool was modified.
			bool UpdateResourcePool();

			/// Adds a resource to the tree.
			void AddResourceToTree(const ResourceSystem::ResourcePtr& resource);

			/// Updates all ItemEntries in the tree.
			void UpdateTree();

			/// Returns the indentation level for specified path.
			uint32 GetPathIndentLevel(const string& path);

			/// Sorting callback function. This function provides an order of items in the list
			/// for the sake of simulating tree widget.
			static bool SortCallback(const CEGUI::ItemEntry* first, const CEGUI::ItemEntry* second);
		//@}

		/// Invalid ResourceIndex. It's used to distinguish resource and directory MenuEntries.
		static uint32 InvalidResourceIndex;

		typedef vector<ResourceSystem::ResourcePtr> ResourceList;

		/// The pool of ResourcePtrs. EntryItems use their IDs as indexes to this pool.
		ResourceList mResourcePool;

		/// List of directory paths that are already present in the tree.
		vector<string> mDirectoryList;

		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mTree;
	};
}

#endif // _EDITOR_RESOURCEWINDOW_H_
