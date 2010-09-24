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
	if (mContentPane) gGUIMgr.DestroyWindowChildren(mContentPane);
	gGUIMgr.DestroyWindow(mManagedWindow);
}

void GUISystem::VerticalLayout::AddChildWindow(CEGUI::Window* window)
{
	mManagedWindow->addChildWindow(window);
	mEventConnections.push_back(new CEGUI::Event::Connection(window->subscribeEvent(CEGUI::Window::EventSized, CEGUI::Event::Subscriber(&GUISystem::VerticalLayout::OnChildWindowSized, this))));
	mEventConnections.push_back(new CEGUI::Event::Connection(window->subscribeEvent(CEGUI::Window::EventDestructionStarted, CEGUI::Event::Subscriber(&GUISystem::VerticalLayout::OnChildWindowDestroyed, this))));
	mChildWindows.push_back(window);
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

		// If not last child window
//		if (it != mChildWindows.end() - 1)
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
}

void GUISystem::VerticalLayout::ClearEventConnections()
{
	for (size_t i = 0; i < mEventConnections.size(); ++i)
	{
		CEGUI::Event::Connection* conn = (CEGUI::Event::Connection*)(mEventConnections.at(i));
		(*conn)->disconnect();
		delete conn;
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
		mChildWindows.erase(it);
	return true;
}
