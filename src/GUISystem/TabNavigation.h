/// @file
/// Provides Tab and Shift+Tab navigation through a list of widgets.
#ifndef GUISYSTEM_TABNAVIGATION_H
#define GUISYSTEM_TABNAVIGATION_H

#include "Base.h"
#include "CEGUIForwards.h"

namespace GUISystem
{
	/**
	 * The TabNavigation class provides Tab and Shift+Tab navigation through a list of widgets.
	 */
	class TabNavigation
	{
	public:
		/// Constructs a TabNavigation instance.
		TabNavigation();

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
	};
}

#endif // GUISYSTEM_TABNAVIGATION_H
