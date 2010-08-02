#include "Common.h"
#include "VerticalLayout.h"
#include "CEGUITools.h"

#include "CEGUI.h"

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
		currentChild->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, (float32)currentY)));
		currentChild->setWidth(CEGUI::UDim(1, 0));
		currentY += (int)currentChild->getHeight().d_offset;

		// If not last child window
		if (it != mChildWindows.end() - 1)
			currentY += mSpacing;
	}
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

	size_t childCount = mContentPane->getChildCount();
	for (int i = (childCount - 1); i >= 0; --i)
	{
		gGUIMgr.DestroyWindow(mContentPane->getChildAtIdx(i));
	}
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

bool GUISystem::VerticalLayout::OnChildWindowSized( const CEGUI::EventArgs& )
{
	if (LockedUpdates()) return false;
	UpdateLayout();
	return true;
}