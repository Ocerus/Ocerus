/// @file
/// Manages the layers window.

#ifndef _EDITOR_LAYERWINDOW_H_
#define _EDITOR_LAYERWINDOW_H_

#include "Base.h"
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
		bool OnEditNewLayerKeyDown(const CEGUI::EventArgs&);
		bool OnButtonSetActiveLayerClicked(const CEGUI::EventArgs&);
		bool OnButtonAddLayerClicked(const CEGUI::EventArgs&);
		bool OnButtonUpDownClicked(const CEGUI::EventArgs&);
		bool OnButtonEditEntityClicked(const CEGUI::EventArgs&);
		bool OnButtonToggleLayerVisibilityClicked(const CEGUI::EventArgs&);
		//@}

	private:

		/// Refreshes cached list of resources and builds resource tree according to the refreshed list.
		void BuildTree();
		
		void UpdateTree();

		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mTree;
		
		
		CEGUI::Window* mNewLayerEditbox;
		CEGUI::Window* mUpButton;
		CEGUI::Window* mDownButton;

		float32 mUpdateTimer;
	};
}

#endif // _EDITOR_LAYERWINDOW_H_
