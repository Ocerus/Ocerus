/// @file
/// Manages the Layer Manager Window.

#ifndef _LAYERMGRWIDGET_H_
#define _LAYERMGRWIDGET_H_

#include "Base.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"

namespace CEGUI
{
	class EventArgs;
	class Tree;
	class Window;
}

namespace Editor
{
	/// The LayerMgrWidget class manages the Layer Manager Window.
	class LayerMgrWidget
	{
	public:
		/// Constructs the LayerMgrWidget that will manage specified Layer Manager Window.
		LayerMgrWidget(CEGUI::Window* layerMgrWindow);

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
		void UpdateTree();

		CEGUI::Window* mLayerMgrWindow;
		CEGUI::Window* mNewLayerEditbox;
		CEGUI::Window* mUpButton;
		CEGUI::Window* mDownButton;
		CEGUI::Tree* mTreeWindow;
		float32 mUpdateTimer;
	};
}

#endif // _LAYERMGRWIDGET_H_
