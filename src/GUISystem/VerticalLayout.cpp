#include "Common.h"
#include "VerticalLayout.h"
#include "CEGUITools.h"

#include "CEGUI.h"

using namespace GUISystem;

GUISystem::VerticalLayout::VerticalLayout(CEGUI::Window* managedWindow, const CEGUI::Window* contentPane, bool resizeParent):
		mManagedWindow(managedWindow),
		mContentPane(contentPane),
		mResizeParent(resizeParent),
		mSpacing(5),
		mLockedUpdates(false)
{
	mManagedWindow->subscribeEvent(CEGUI::Window::EventDestructionStarted, CEGUI::Event::Subscriber(&GUISystem::VerticalLayout::OnManagedWindowDestructionStarted, this));

	if (mContentPane == 0)
		mContentPane = mManagedWindow;
}

GUISystem::VerticalLayout::~VerticalLayout()
{
	for (size_t i = 0; i < mEventConnections.size(); ++i)
	{
		CEGUI::Event::Connection* conn = (CEGUI::Event::Connection*)(mEventConnections.at(i));
		(*conn)->disconnect();
		delete conn;
	}
}

void GUISystem::VerticalLayout::AddChildWindow(CEGUI::Window* window)
{
	mManagedWindow->addChildWindow(window);
	mEventConnections.push_back(new CEGUI::Event::Connection(window->subscribeEvent(CEGUI::Window::EventSized, CEGUI::Event::Subscriber(&GUISystem::VerticalLayout::OnChildWindowSized, this))));
	mEventConnections.push_back(new CEGUI::Event::Connection(window->subscribeEvent(CEGUI::Window::EventDestructionStarted, CEGUI::Event::Subscriber(&GUISystem::VerticalLayout::OnChildWindowDestructionStarted, this))));
	mChildWindows.push_back(window);
}

bool GUISystem::VerticalLayout::OnManagedWindowDestructionStarted(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	delete this;
	return true;
}

bool GUISystem::VerticalLayout::OnChildWindowDestructionStarted(const CEGUI::EventArgs& args)
{
	const CEGUI::WindowEventArgs& windowEventArgs = static_cast<const CEGUI::WindowEventArgs&>(args);
	WindowList::iterator it = Containers::find(mChildWindows.begin(), mChildWindows.end(), windowEventArgs.window);
	if (it != mChildWindows.end())
	{
		mChildWindows.erase(it);
	}
	return true;
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
