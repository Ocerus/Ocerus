#include "Common.h"
#include "TabNavigation.h"
#include "CEGUITools.h"

using namespace GUISystem;

TabNavigation::TabNavigation()
{
}

TabNavigation::~TabNavigation()
{
}

void TabNavigation::AddWidget(CEGUI::Window* widget)
{
	mTabOrder.push_back(widget);
	widget->subscribeEvent(CEGUI::Window::EventDestructionStarted, CEGUI::Event::Subscriber(&GUISystem::TabNavigation::OnWidgetDestroyed, this));
	//widget->subscribeEvent(CEGUI::Window::EventCharacterKey, CEGUI::Event::Subscriber(&GUISystem::TabNavigation::OnEventKeyDown, this));
	widget->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&GUISystem::TabNavigation::OnEventKeyDown, this));
}

void TabNavigation::Clear()
{
	mTabOrder.clear();
}

bool TabNavigation::OnWidgetDestroyed(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	WidgetList::iterator it = Containers::find(mTabOrder.begin(), mTabOrder.end(), args.window);
	if (it != mTabOrder.end())
		mTabOrder.erase(it);
	return true;
}

bool TabNavigation::OnEventKeyDown(const CEGUI::EventArgs& e)
{
	const CEGUI::KeyEventArgs& args = static_cast<const CEGUI::KeyEventArgs&>(e);
	if (args.scancode == CEGUI::Key::Tab) // Tab or Shift+Tab
	{
		WidgetList::iterator itCurrent = Containers::find(mTabOrder.begin(), mTabOrder.end(), args.window);
		OC_ASSERT(itCurrent != mTabOrder.end());
		WidgetList::iterator itFocus = itCurrent;
		CEGUI::Window* newWidget = 0;
		do
		{
			if (args.sysKeys & CEGUI::Shift)
			{
				// Set previous
				if (itFocus == mTabOrder.begin())
					itFocus = mTabOrder.end();
				--itFocus;
			}
			else
			{
				// Set next
				++itFocus;
				if (itFocus == mTabOrder.end())
					itFocus = mTabOrder.begin();
			}
			newWidget = *itFocus;
		}
		while ((!newWidget->isVisible() || newWidget->isDisabled() || newWidget->getProperty("ReadOnly") == "True" ) && itFocus != itCurrent);
		newWidget->activate();
		return true;
	}
	return false;
}



