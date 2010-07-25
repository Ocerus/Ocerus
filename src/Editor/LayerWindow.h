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

		/// @name CEGUI Callbacks
		//@{
		bool OnDragContainerMouseButtonDown(const CEGUI::EventArgs&);
		bool OnLayerExpandClick(const CEGUI::EventArgs&);
		bool OnLayerMouseDoubleClick(const CEGUI::EventArgs&);
		//bool OnDragContainerMouseButtonUp(const CEGUI::EventArgs&);
		//bool OnDragContainerMouseDoubleClick(const CEGUI::EventArgs&);
		//bool OnRefreshButtonClicked(const CEGUI::EventArgs&);
		//@}


	private:

		/// Refreshes cached list of resources and builds resource tree according to the refreshed list.
		void BuildTree();
		
		void UpdateTree();

		void UpdateLayerItem(CEGUI::Window* layerItem, EntitySystem::LayerID layerID);
		
		void UpdateEntityItem(CEGUI::Window* entityItem, EntitySystem::EntityHandle entity);

		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mTree;

		CEGUI::Window* mNewLayerEditbox;
		CEGUI::Window* mUpButton;
		CEGUI::Window* mDownButton;

		set<string> mExpandedLayers;
		set<EntitySystem::LayerID> mExpandedLayerIDs;

		float32 mUpdateTimer;
	};
}

#endif // _EDITOR_LAYERWINDOW_H_
