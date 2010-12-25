#include "Common.h"
#include "VerticalLayout.h"

#include "CEGUICommon.h"

using namespace GUISystem;

GUISystem::VerticalLayout::VerticalLayout(CEGUI::Window* managedWindow, CEGUI::Window* contentPane, bool resizeParent):
		mManagedWindow(managedWindow),
		mContentPane(contentPane),
		mResizeParent(resizeParent),
		mSpacing(5),
		mLockedUpdates(false)
{
	if (mContentPane == 0) mContentPane = mManagedWindow;
}

GUISystem::VerticalLayout::~VerticalLayout()
{
	ClearEventConnections();
}

void GUISystem::VerticalLayout::AddChildWindow(CEGUI::Window* window)
{
	mManagedWindow->addChildWindow(window);
	mEventConnections[window].push_back(window->subscribeEvent(CEGUI::Window::EventSized, CEGUI::Event::Subscriber(&GUISystem::VerticalLayout::OnChildWindowSized, this)));
	mEventConnections[window].push_back(window->subscribeEvent(CEGUI::Window::EventDestructionStarted, CEGUI::Event::Subscriber(&GUISystem::VerticalLayout::OnChildWindowDestroyed, this)));
	mChildWindows.push_back(window);
}

void GUISystem::VerticalLayout::RemoveChildWindow(CEGUI::Window* window)
{
	WindowList::iterator windowIt = Containers::find(mChildWindows.begin(), mChildWindows.end(), window);
	if (windowIt != mChildWindows.end()) mChildWindows.erase(windowIt);

	EventConnections::iterator eventIt = mEventConnections.find(window);
	if (eventIt != mEventConnections.end())
	{
		for (EventConnectionsList::iterator it=eventIt->second.begin(); it!=eventIt->second.end(); ++it)
		{
			(*it)->disconnect();
		}
		mEventConnections.erase(eventIt);
	}
}

void GUISystem::VerticalLayout::UpdateLayout()
{
	PROFILE_FNC();

	if (LockedUpdates()) return;
	LockUpdates();

	int currentY = 0;
	for (WindowList::iterator it = mChildWindows.begin(); it != mChildWindows.end(); ++it)
	{
		CEGUI::Window* currentChild = *it;
		if (!currentChild->isVisible())
		{
			currentChild->setPosition(CEGUI::UVector2(cegui_absdim(0), cegui_absdim(0)));
			continue;
		}
		currentChild->setPosition(CEGUI::UVector2(cegui_absdim(0), cegui_absdim((float32)currentY)));
		if (currentChild->getWidth() != CEGUI::UDim(1, 0))
			currentChild->setWidth(CEGUI::UDim(1, 0));
		currentY += (int)currentChild->getHeight().d_offset;

		currentY += mSpacing;

	}
	currentY -= mSpacing;
	
	if (mResizeParent)
	{
		float32 resizeOffset = mManagedWindow->getHeight().d_offset - mContentPane->getHeight().d_offset;
		mManagedWindow->setHeight(CEGUI::UDim(0, currentY + resizeOffset));
	}

	UnlockUpdates();
}

size_t GUISystem::VerticalLayout::GetChildCount() const
{
	return mChildWindows.size();
}

void GUISystem::VerticalLayout::Clear()
{
	ClearEventConnections();
	mChildWindows.clear();
	mManagedWindow->setArea(CEGUI::URect(CEGUI::UDim(0,0),CEGUI::UDim(0,0),CEGUI::UDim(0,0),CEGUI::UDim(0,0)));
}

void GUISystem::VerticalLayout::ClearEventConnections()
{
	for (EventConnections::iterator eventListIt=mEventConnections.begin(); eventListIt!=mEventConnections.end(); ++eventListIt)
	{
		EventConnectionsList& list = eventListIt->second;
		for (EventConnectionsList::iterator eventIt=list.begin(); eventIt!=list.end(); ++eventIt)
		{
			(*eventIt)->disconnect();
		}
	}
	mEventConnections.clear();
}

bool GUISystem::VerticalLayout::OnChildWindowSized( const CEGUI::EventArgs& e )
{
	OC_UNUSED(e);
	if (LockedUpdates()) return false;
	UpdateLayout();
	return true;
}

bool GUISystem::VerticalLayout::OnChildWindowDestroyed(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs args = static_cast<const CEGUI::WindowEventArgs&>(e);
	WindowList::iterator it = Containers::find(mChildWindows.begin(), mChildWindows.end(), args.window);
	if (it != mChildWindows.end())
	{
		mChildWindows.erase(it);
	}
	return true;
}
