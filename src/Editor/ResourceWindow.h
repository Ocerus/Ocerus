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
		inline void Refresh() { BuildResourceTree(); }

		/// Returns the ResourcePtr to resource in ResourceWindow's cached list of resources on given index.
		ResourceSystem::ResourcePtr GetResourceAtIndex(size_t index);

		/// @name CEGUI Callbacks
		//@{
			bool OnDragContainerMouseButtonUp(const CEGUI::EventArgs&);
			bool OnRefreshButtonClicked(const CEGUI::EventArgs&);
		//@}

	private:

		/// Refreshes cached list of resources and builds resource tree according to the refreshed list.
		void BuildResourceTree();

		/// Cached list of resources
		vector<ResourceSystem::ResourcePtr> mResources;

		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mTree;
	};
}

#endif // EDITOR_RESOURCEWINDOW_H
