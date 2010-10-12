/// @file
/// Manages the layer manager window.
#ifndef _EDITOR_LAYERWINDOW_H_
#define _EDITOR_LAYERWINDOW_H_

#include "Base.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"
#include "GUISystem/CEGUIForwards.h"

namespace Editor
{
	/// The LayerWindow class manages the layer manager window.
	class LayerWindow
	{
	public:

		/// Constructs a LayerWindow.
		LayerWindow();

		/// Destroys the LayerWindow.
		~LayerWindow();

		/// Initializes the LayerWindow.
		void Init();

		/// Deinitializes the LayerWindow.
		void Deinit();

		/// Clears the items.
		void Clear();

		/// Updates the LayerWindow.
		void Update();

		/// Sets the currently selected entity.
		void SetSelectedEntity(EntitySystem::EntityHandle selectedEntity);

		/// Moves the specified layer up.
		void MoveLayerUp(EntitySystem::LayerID layerID);

		/// Moves the specified layer down.
		void MoveLayerDown(EntitySystem::LayerID layerID);

		/// Shows a prompt for new layer name. If the user confirms the prompt
		/// a new layer will be created just under specified layer.
		void ShowNewLayerPrompt(EntitySystem::LayerID layerID);

		/// Creates a new layer just under the specified layer.
		void CreateLayer(EntitySystem::LayerID layerID, const string& layerName);

		/// Shows a prompt for renaming the specified layer. If the user confirms the prompt
		/// the specified layer will be renamed.
		void ShowRenameLayerPrompt(EntitySystem::LayerID layerID);

		/// Renames the specified layer.
		void RenameLayer(EntitySystem::LayerID layerID, const string& newLayerName);

		/// Removes the specified layer.
		void RemoveLayer(EntitySystem::LayerID layerID);

		/// Moves the specified entity to the layer above the current layer of the entity.
		void MoveEntityUp(EntitySystem::EntityHandle entity);

		/// Moves the specified entity to the layer below the current layer of the entity.
		void MoveEntityDown(EntitySystem::EntityHandle entity);

	private:

		enum eLayerPopupItem
		{
			LPI_MOVE_UP = 0,
			LPI_MOVE_DOWN,
			LPI_NEW,
			LPI_RENAME,
			LPI_REMOVE
		};

		enum eEntityPopupItem
		{
			EPI_MOVE_UP = 0,
			EPI_MOVE_DOWN
		};

		/// @name CEGUI Callbacks
		//@{
			bool OnTreeClicked(const CEGUI::EventArgs&);
			bool OnLayerItemExpandClick(const CEGUI::EventArgs&);
			bool OnLayerItemVisibilityToggleClick(const CEGUI::EventArgs&);
			bool OnItemClicked(const CEGUI::EventArgs&);
			bool OnItemDoubleClicked(const CEGUI::EventArgs&);
			bool OnDragDropItemDropped(const CEGUI::EventArgs&);
		//@}

		void UpdateLayerItem(CEGUI::ItemEntry* layerItem, EntitySystem::LayerID layerID);
		void UpdateEntityItem(CEGUI::ItemEntry* entityItem, EntitySystem::EntityHandle entity);

		void NewLayerPromptCallback(bool clickedOK, const string& text, int32 tag);
		void RenameLayerPromptCallback(bool clickedOK, const string& text, int32 tag);

		static bool SortCallback(const CEGUI::ItemEntry* first, const CEGUI::ItemEntry* second);

		void CreatePopupMenus();
		void DestroyPopupMenus();
		void OnLayerPopupMenuItemClicked(CEGUI::Window* menuItem);
		void OnEntityPopupMenuItemClicked(CEGUI::Window* menuItem);

		/// @name ItemEntry caching
		//@{
			CEGUI::ItemEntry* CreateLayerItem();
			CEGUI::ItemEntry* CreateEntityItem();
			
			void SetupLayerItem(CEGUI::ItemEntry* layerItem, EntitySystem::LayerID layerID);
			void SetupEntityItem(CEGUI::ItemEntry* entityItem, EntitySystem::EntityHandle entityHandle);
			
			void StoreItem(CEGUI::ItemEntry* item);
			
			CEGUI::ItemEntry* RestoreLayerItem();
			CEGUI::ItemEntry* RestoreEntityItem();

			/// Destroys the resource ItemEntry cache.
			void DestroyItemCache();

			typedef vector<CEGUI::ItemEntry*> ItemCache;

			ItemCache mLayerItemCache;
			ItemCache mEntityItemCache;
		//@}
		
		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mTree;

		CEGUI::Window* mLayerPopupMenu;
		CEGUI::Window* mEntityPopupMenu;
		EntitySystem::LayerID mCurrentPopupLayerID;
		EntitySystem::EntityHandle mCurrentPopupEntity;

		set<string> mExpandedLayers;
		set<EntitySystem::LayerID> mExpandedLayerIDs;
	};
}

#endif // _EDITOR_LAYERWINDOW_H_
