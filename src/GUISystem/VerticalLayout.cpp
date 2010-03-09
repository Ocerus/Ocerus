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

	OC_ASSERT(mContentPane->getChildCount() == 0);
}

GUISystem::VerticalLayout::~VerticalLayout()
{
}

void GUISystem::VerticalLayout::AddChildWindow(CEGUI::Window* window)
{
	mManagedWindow->addChildWindow(window);
	window->subscribeEvent(CEGUI::Window::EventSized, CEGUI::Event::Subscriber(&GUISystem::VerticalLayout::OnChildWindowSized, this));
}

bool GUISystem::VerticalLayout::OnManagedWindowDestructionStarted(const CEGUI::EventArgs& event)
{
	OC_UNUSED(event);
	delete this;
	return true;
}

void GUISystem::VerticalLayout::UpdateLayout()
{
	if (LockedUpdates()) return;
	LockUpdates();

	int childCount = mContentPane->getChildCount();
	int currentY = 0;
	for (int i = 0; i < childCount; ++i)
	{
		CEGUI::Window* currentChild = mContentPane->getChildAtIdx(i);
		currentChild->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, (float32)currentY)));
		currentChild->setWidth(CEGUI::UDim(1, 0));
		currentY += (int)currentChild->getHeight().d_offset;
		if (i != childCount - 1)
			currentY += mSpacing;
	}
	if (mResizeParent)
	{
		float32 resizeOffset = mManagedWindow->getHeight().d_offset - mContentPane->getHeight().d_offset;
		mManagedWindow->setHeight(CEGUI::UDim(0, currentY + resizeOffset));
	}	
	UnlockUpdates();
}
