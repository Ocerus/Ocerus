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
}

void PopupMgr::HidePopup()
{
	if (mCurrentPopupWindow)
	{
		mCurrentPopupWindow->hide();
		CloseSubmenus(mCurrentPopupWindow);
		mCurrentPopupWindow = 0;
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
	popupMenu->subscribeEvent(CEGUI::MenuBase::EventPopupOpened, CEGUI::Event::Subscriber(&GUISystem::PopupMgr::OnPopupMenuOpened, this));
	return popupMenu;
}

void PopupMgr::DestroyPopupMenu(CEGUI::Window* popupMenu)
{
	gGUIMgr.DestroyWindowDirectly(popupMenu);
}

CEGUI::Window* PopupMgr::CreateMenuItem(const CEGUI::String& windowName, const CEGUI::String& text, const CEGUI::String& tooltip, CEGUI::uint id)
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

void PopupMgr::DoAutoHide(float posX, float posY)
{
	if (mCurrentPopupWindow == 0)
		return;

	if (!mCurrentPopupWindow->isHit(CEGUI::Vector2(posX, posY)))
		HidePopup();
}

bool PopupMgr::OnMenuItemClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	CEGUI::Window* clickedItem = args.window;

	// Don't call callback if item has submenu, instead open the submenu (this is done implicitly by CEGUI)
	if (clickedItem->getChildCount() > 0)
		return true;

	HidePopup();
	if (mCurrentCallback.IsSet())
	{
		mCurrentCallback.Call(clickedItem);
	}
	else
	{
		ocWarning << "Popup menu item clicked, but no callback installed.";
	}
	return true;
}

bool PopupMgr::OnPopupMenuOpened(const CEGUI::EventArgs& e)
{
	// Ensure that popup menu does not go over the screen edge
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	CEGUI::Window* popupWindow = args.window;
	const CEGUI::Size& displaySize = CEGUI::System::getSingleton().getRenderer()->getDisplaySize();	
	
	float popupWindowPosX = popupWindow->getPosition().d_x.d_offset;
	float popupWindowPosY = popupWindow->getPosition().d_y.d_offset;
	CEGUI::Vector2 popupWindowScreenPos = CEGUI::CoordConverter::windowToScreen(*popupWindow, CEGUI::Vector2(0, 0));

	float horizontalOverlap = popupWindowScreenPos.d_x + popupWindow->getWidth().d_offset - displaySize.d_width;
	float verticalOverlap = popupWindowScreenPos.d_y + popupWindow->getHeight().d_offset - displaySize.d_height;

	if (horizontalOverlap > 0)
		popupWindowPosX -= horizontalOverlap;

	if (verticalOverlap > 0)
		popupWindowPosY -= verticalOverlap;
	
	if (horizontalOverlap > 0 || verticalOverlap > 0)
	{
		popupWindow->setPosition(CEGUI::UVector2(cegui_absdim(popupWindowPosX), cegui_absdim(popupWindowPosY)));
	}
	return true;
}

void GUISystem::PopupMgr::CloseSubmenus(CEGUI::Window* window)
{
	for (size_t idx = 0; idx < window->getChildCount(); ++idx)
	{
		CEGUI::Window* child = window->getChildAtIdx(idx);
		if (child->testClassName("PopupMenu"))
		{
			CEGUI::PopupMenu* popupMenu = static_cast<CEGUI::PopupMenu*>(child);
			popupMenu->closePopupMenu();
		}
		CloseSubmenus(child);
	}
}
