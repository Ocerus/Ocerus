#include "Common.h"
#include "PopupMgr.h"
#include "CEGUICommon.h"

using namespace GUISystem;

PopupMgr::PopupMgr(): mCurrentPopupWindow(0)
{
}

PopupMgr::~PopupMgr()
{
}

void PopupMgr::ShowPopup(CEGUI::Window* popupWindow, float posX, float posY, PopupMgr::Callback callback)
{
	HidePopup();
	mCurrentPopupWindow = popupWindow;
	mCurrentCallback = callback;

	if (mCurrentPopupWindow->getParent() == 0)
		gGUIMgr.GetGUISheet()->addChildWindow(mCurrentPopupWindow);

	const CEGUI::Size& displaySize = CEGUI::System::getSingleton().getRenderer()->getDisplaySize();	

	if (posX + 12 + mCurrentPopupWindow->getWidth().d_offset > displaySize.d_width)
	{
		posX -= mCurrentPopupWindow->getWidth().d_offset;
	}
	else
	{
		posX += 12;
	}
		
	if (posY + mCurrentPopupWindow->getHeight().d_offset > displaySize.d_height)
	{
		posY -= mCurrentPopupWindow->getHeight().d_offset;
	}

	mCurrentPopupWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0, posX), CEGUI::UDim(0, posY)));
	mCurrentPopupWindow->show();
	mCurrentPopupWindow->activate();
	CEGUI::WindowManager::getSingleton().DEBUG_dumpWindowNames("GERE");
}

void PopupMgr::HidePopup()
{
	if (mCurrentPopupWindow)
	{
		mCurrentPopupWindow->hide();
		mCurrentPopupWindow = 0;
		//UnsubscribePopupEvents();
	}
}

CEGUI::Window* PopupMgr::CreatePopupMenu(const CEGUI::String& windowName)
{
	CEGUI::PopupMenu* popupMenu = static_cast<CEGUI::PopupMenu*>(gGUIMgr.CreateWindowDirectly("Editor/PopupMenu", windowName));
	popupMenu->setAlwaysOnTop(true);
	popupMenu->setVisible(false);
	popupMenu->setAutoResizeEnabled(true);
	popupMenu->setItemSpacing(2);
	popupMenu->setClippedByParent(false);
	return popupMenu;
}

void PopupMgr::DestroyPopupMenu(CEGUI::Window* popupMenu)
{
	gGUIMgr.DestroyWindowDirectly(popupMenu);
}

CEGUI::Window* PopupMgr::CreateMenuItem(const CEGUI::String& windowName, const CEGUI::String& text, const CEGUI::String& tooltip, uint id)
{
	CEGUI::Window* menuItem = gGUIMgr.CreateWindow("Editor/MenuItem", false, windowName);
	menuItem->setText(text);
	menuItem->setTooltipText(tooltip);
	menuItem->setID(id);
	menuItem->setWantsMultiClickEvents(false);
	menuItem->subscribeEvent(CEGUI::MenuItem::EventMouseClick, CEGUI::Event::Subscriber(&GUISystem::PopupMgr::OnMenuItemClicked, this));
	return menuItem;
}

void PopupMgr::DestroyMenuItem(CEGUI::Window* menuItem)
{
	gGUIMgr.DestroyWindowDirectly(menuItem);
}

bool PopupMgr::OnMenuItemClicked(const CEGUI::EventArgs& e)
{
	HidePopup();
	if (mCurrentCallback.IsSet())
	{
		const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
		CEGUI::Window* clickedItem = args.window;
		mCurrentCallback.Call(clickedItem);
	}
	else
	{
		ocWarning << "Popup menu item clicked, but no callback installed.";
	}
	return true;
}