/// @file
/// Provides Tab and Shift+Tab navigation through a list of widgets.
#ifndef _GUISYSTEM_TABNAVIGATION_H_
#define _GUISYSTEM_TABNAVIGATION_H_

#include "Base.h"
#include "CEGUIForwards.h"

namespace GUISystem
{
	/// The TabNavigation class provides Tab and Shift+Tab navigation through a list of widgets.
	class TabNavigation
	{
	public:
		/// Constructs a TabNavigation instance.
		/// @param scrollablePane If scrollablePane is passed, TabNavigation ensures, that pane will be
		/// automatically scrolled so the new active widget will be visible.
		TabNavigation(CEGUI::ScrollablePane* scrollablePane = 0);

		/// Destroys the TabNavigation.
		~TabNavigation();

		/// Adds a widget to the end of the tab order.
		void AddWidget(CEGUI::Window* widget);

		/// Clears the tab order list.
		void Clear();

	private:
		bool OnEventKeyDown(const CEGUI::EventArgs& e);
		bool OnWidgetDestroyed(const CEGUI::EventArgs& e);

		typedef vector<CEGUI::Window*> WidgetList;
		WidgetList mTabOrder;
		CEGUI::ScrollablePane* mScrollablePane;
	};
}

#endif // _GUISYSTEM_TABNAVIGATION_H_
