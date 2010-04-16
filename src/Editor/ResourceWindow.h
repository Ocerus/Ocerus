/// @file
/// Manages GUI for listing resources.

#ifndef EDITOR_RESOURCEWINDOW_H
#define EDITOR_RESOURCEWINDOW_H

#include "Base.h"
#include "EditorMenu.h"

namespace CEGUI
{
	class ItemListbox;
	class Window;
}

namespace Editor
{

	///
	class ResourceWindow
	{
	public:

		///
		ResourceWindow();

		///
		~ResourceWindow();

		///
		void Init();

		/// @name CEGUI Callbacks
		//@{
			bool OnDragContainerMouseButtonUp(const CEGUI::EventArgs&);
		//@}

		
	private:

		///
		void BuildResourceTree();

		/// Cached list of resources
		vector<ResourceSystem::ResourcePtr> mResources;

		/// ResourceWindow widget
		CEGUI::Window* mWindow;

		CEGUI::ItemListbox* mTree;
	};
}

#endif // EDITOR_RESOURCEWINDOW_H
