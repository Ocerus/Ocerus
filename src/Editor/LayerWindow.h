/// @file
/// Manages the layers window.

#ifndef _EDITOR_LAYERWINDOW_H_
#define _EDITOR_LAYERWINDOW_H_

#include "Base.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"
#include "GUISystem/CEGUIForwards.h"

namespace Editor
{
	/// The LayerWindow class manages the layers window.
	class LayerWindow
	{
	public:

		/// Constructs a LayerWindow.
		LayerWindow();

		/// Destroys the LayerWindow.
		~LayerWindow();

		/// Initialize the Layer Manager Window.
		void Init();

		/// Updates the Layer Manager Window.
		void Update(float32 delta);

		void MoveLayerUp(EntitySystem::LayerID layerID);

		void MoveLayerDown(EntitySystem::LayerID layerID);
		
		void RenameLayer(EntitySystem::LayerID layerID);
		
		void RemoveLayer(EntitySystem::LayerID layerID);

		/// @name CEGUI Callbacks
		//@{
		
			
		bool OnDragContainerMouseButtonDown(const CEGUI::EventArgs&);
		bool OnDragContainerMouseButtonUp(const CEGUI::EventArgs&);
		bool OnLayerItemExpandClick(const CEGUI::EventArgs&);
		bool OnLayerItemVisibilityToggleClick(const CEGUI::EventArgs&);
		bool OnLayerItemDoubleClick(const CEGUI::EventArgs&);
		bool OnEntityItemDoubleClick(const CEGUI::EventArgs&);
		bool OnDragDropItemDropped(const CEGUI::EventArgs&);
		//bool OnDragContainerMouseDoubleClick(const CEGUI::EventArgs&);
		//bool OnRefreshButtonClicked(const CEGUI::EventArgs&);
		//@}


	private:

		/// Refreshes cached list of resources and builds resource tree according to the refreshed list.
		void BuildTree();
		
		void UpdateTree();

		void UpdateLayerItem(CEGUI::Window* layerItem, EntitySystem::LayerID layerID);
		
		void UpdateEntityItem(CEGUI::Window* entityItem, EntitySystem::EntityHandle entity);

		void PromptCallback(bool clickedOK, string text, int32 tag);
		
		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mTree;

		CEGUI::Window* mNewLayerEditbox;
		CEGUI::Window* mUpButton;
		CEGUI::Window* mDownButton;

		set<string> mExpandedLayers;
		set<EntitySystem::LayerID> mExpandedLayerIDs;

		float32 mUpdateTimer;
		
		EntitySystem::LayerID mSavedLayerID;
	};
}

#endif // _EDITOR_LAYERWINDOW_H_
