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
		void Update(float32 delta);

		/// Moves the specified layer up.
		void MoveLayerUp(EntitySystem::LayerID layerID);

		/// Moves the specified layer down.
		void MoveLayerDown(EntitySystem::LayerID layerID);

		/// Creates a new layer just under the specified layer. A prompt for typing
		/// the name of the layer is shown.
		void NewLayer(EntitySystem::LayerID layerID);

		/// Renames the specified layer. A prompt for typing new name of the layer is shown.
		void RenameLayer(EntitySystem::LayerID layerID);

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
			bool OnDragContainerMouseButtonDown(const CEGUI::EventArgs&);
			bool OnDragContainerMouseButtonUp(const CEGUI::EventArgs&);
			bool OnTreeMouseButtonUp(const CEGUI::EventArgs&);
			bool OnLayerItemExpandClick(const CEGUI::EventArgs&);
			bool OnLayerItemVisibilityToggleClick(const CEGUI::EventArgs&);
			bool OnLayerItemDoubleClick(const CEGUI::EventArgs&);
			bool OnDragDropItemDropped(const CEGUI::EventArgs&);
		//@}

		void UpdateTree();
		void UpdateLayerItem(CEGUI::Window* layerItem, EntitySystem::LayerID layerID);
		void UpdateEntityItem(CEGUI::Window* entityItem, EntitySystem::EntityHandle entity);

		void NewLayerPromptCallback(bool clickedOK, const string& text, int32 tag);
		void RenameLayerPromptCallback(bool clickedOK, const string& text, int32 tag);

		void CreatePopupMenus();
		void DestroyPopupMenus();
		void OnLayerPopupMenuItemClicked(CEGUI::Window* menuItem);
		void OnEntityPopupMenuItemClicked(CEGUI::Window* menuItem);

		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mTree;

		CEGUI::Window* mLayerPopupMenu;
		CEGUI::Window* mEntityPopupMenu;
		EntitySystem::LayerID mCurrentPopupLayerID;
		EntitySystem::EntityHandle mCurrentPopupEntity;

		set<string> mExpandedLayers;
		set<EntitySystem::LayerID> mExpandedLayerIDs;

		float32 mUpdateTimer;
	};
}

#endif // _EDITOR_LAYERWINDOW_H_
