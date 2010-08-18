/// @file
/// Manages the window for editing entity properties.
#ifndef _EDITOR_ENTITYWINDOW_H_
#define _EDITOR_ENTITYWINDOW_H_

#include "Base.h"
#include "GUISystem/CEGUIForwards.h"

namespace Editor
{
	/// The EntityWindow class manages the entity window, that shows and allows to modify properties of the current entity.
	class EntityWindow
	{
	public:
		/// Constructs an EntityWindow instance.
		EntityWindow();

		/// Destroys the EntityWindow instance.
		~EntityWindow();

		/// Initializes the window.
		void Init();

		/// Updates values of the properties in widgets.
		void Update(float32 delta);
		
		/// Rebuilds the window.
		void Rebuild();

		/// Rebuilds the window in the next call of Update();
		inline void RebuildLater() { mNeedsRebuild = true; }

		/// Clears the window.
		void Clear();

		/// Adds a widget to tab navigation of this window.
		void AddWidgetToTabNavigation(CEGUI::Window* widget);

	private:
		/// Manages clicking remove component button.
		bool OnRemoveComponentButtonClicked(const CEGUI::EventArgs&);
		
		CEGUI::Window* mWindow;
		CEGUI::ScrollablePane* mScrollablePane;

		typedef vector<AbstractValueEditor*> PropertyEditors;
		typedef vector<GUISystem::VerticalLayout*> VerticalLayouts;

		PropertyEditors mPropertyEditors;
		VerticalLayouts mVerticalLayouts;
		GUISystem::VerticalLayout* mVerticalLayout;
		GUISystem::TabNavigation* mTabNavigation;

		float32 mPropertyUpdateTimer;
		bool mNeedsRebuild;
	};
}

#endif // _EDITOR_ENTITYWINDOW_H_
