#include "Common.h"
#include "PrototypeWindow.h"
#include "EditorMgr.h"
#include "GUISystem/CEGUICommon.h"
#include "GUISystem/PopupMgr.h"
#include "GUISystem/PromptBox.h"

using namespace Editor;

Editor::PrototypeWindow::PrototypeWindow(): mPopupMenu(0), mWindow(0), mList(0)
{
}

Editor::PrototypeWindow::~PrototypeWindow()
{
	if (mWindow || mPopupMenu)
	{
		ocError << "PrototypeWindow was destroyed without deinitialization.";		
	}
}

void Editor::PrototypeWindow::Init()
{
	CEGUI_TRY;
	{
		mWindow = gGUIMgr.GetWindow("Editor/PrototypeWindow");
		OC_ASSERT(mWindow);

		mList = static_cast<CEGUI::ItemListbox*>(gGUIMgr.CreateWindow("Editor/ItemListbox", "Editor/PrototypeWindow/List"));
		mList->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		mList->setUserString("WantsMouseWheel", "True");
		mWindow->addChildWindow(mList);
		mList->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Editor::PrototypeWindow::OnListClicked, this));
	}
	CEGUI_CATCH;

	CreatePopupMenu();
	Update();
}

void PrototypeWindow::Deinit()
{
	gGUIMgr.DestroyWindow(mWindow);
	mWindow = 0;

	DestroyPopupMenu();
	DestroyItemCache();
}

void PrototypeWindow::Update()
{
	EntitySystem::EntityList prototypes;
	gEntityMgr.ProcessDestroyQueue();
	gEntityMgr.GetPrototypes(prototypes);

	while (mList->getItemCount() > prototypes.size())
	{
		StorePrototypeItem(mList->getItemFromIndex(0));
	}

	while (mList->getItemCount() < prototypes.size())
	{
		CEGUI::ItemEntry* newItem = RestorePrototypeItem();
		mList->addItem(newItem);
	}

	OC_DASSERT(mList->getItemCount() == prototypes.size());

	size_t itemCount = mList->getItemCount();
	for (size_t idx = 0; idx < itemCount; ++idx)
	{
		SetupPrototypeItem(mList->getItemFromIndex(idx), prototypes.at(idx));
	}
}

void Editor::PrototypeWindow::SetSelectedEntity( const EntitySystem::EntityHandle entity)
{
	if (!entity.IsValid())
	{
		mList->clearAllSelections();
		return;
	}

	size_t itemCount = mList->getItemCount();
	for (size_t idx = 0; idx < itemCount; ++idx)
	{
		CEGUI::ItemEntry* item = mList->getItemFromIndex(idx);
		if (entity.IsValid() && (int)item->getID() == entity.GetID())
		{
			item->setSelected(true);
		}
		else
		{
			item->setSelected(false);
		}
	}
}

void PrototypeWindow::ShowCreatePrototypePrompt()
{
	GUISystem::PromptBox* prompt = new GUISystem::PromptBox();
	prompt->SetText(TR("prototype_new_prompt"));
	prompt->RegisterCallback(GUISystem::PromptBox::Callback(this, &PrototypeWindow::CreatePrototypePromptCallback));
	prompt->Show();
}

void PrototypeWindow::CreatePrototype(const string& prototypeName)
{
	EntitySystem::EntityDescription desc;
	desc.SetKind(EntitySystem::EntityDescription::EK_PROTOTYPE);
	desc.SetName(prototypeName);
	gEntityMgr.CreateEntity(desc);
	Update();
	gEntityMgr.SavePrototypes();
}

void PrototypeWindow::DeletePrototype(EntitySystem::EntityHandle entity)
{
	gEntityMgr.DestroyEntity(entity);
	gEntityMgr.ProcessDestroyQueue();
	Update();
	gEntityMgr.SavePrototypes();
}

void PrototypeWindow::InstantiatePrototype(EntitySystem::EntityHandle entity)
{
	gEditorMgr.SelectEntity(gEntityMgr.InstantiatePrototype(entity));
}

bool Editor::PrototypeWindow::OnListItemClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	EntitySystem::EntityHandle clickedPrototype = EntitySystem::EntityHandle(dragContainer->getID());
	if (!clickedPrototype.IsValid())
		return true;

	gEditorMgr.SelectEntity(clickedPrototype);

	if (args.button == CEGUI::RightButton)
	{
		mCurrentPrototype = clickedPrototype;
		mPopupMenu->getChildAtIdx(0)->setEnabled(true);
		mPopupMenu->getChildAtIdx(2)->setEnabled(true);
		gPopupMgr->ShowPopup(mPopupMenu, args.position.d_x, args.position.d_y, GUISystem::PopupMgr::Callback(this, &Editor::PrototypeWindow::OnPopupMenuItemClicked));
	}
	return true;
}

bool Editor::PrototypeWindow::OnListClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);

	if (args.button == CEGUI::RightButton)
	{
		mPopupMenu->getChildAtIdx(0)->setEnabled(false);
		mPopupMenu->getChildAtIdx(2)->setEnabled(false);
		gPopupMgr->ShowPopup(mPopupMenu, args.position.d_x, args.position.d_y, GUISystem::PopupMgr::Callback(this, &Editor::PrototypeWindow::OnPopupMenuItemClicked));
	}
	return true;
}

CEGUI::ItemEntry* PrototypeWindow::CreatePrototypeItem()
{
	static uint32 prototypeItemCounter = 0;
	const CEGUI::String& windowName = "Editor/PrototypeWindow/List/PrototypeItem" + StringConverter::ToString(prototypeItemCounter++);
	CEGUI::ItemEntry* newItem = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("Editor/ListboxItem", windowName));

	CEGUI::Window* dragContainer = gGUIMgr.CreateWindow("DragContainer", windowName + "/DC");
	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	newItem->addChildWindow(dragContainer);

	CEGUI::Window* newItemText = gGUIMgr.CreateWindow("Editor/StaticText", windowName + "/DC/Text");
	newItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	newItemText->setProperty("FrameEnabled", "False");
	newItemText->setProperty("BackgroundEnabled", "False");
	newItemText->setMousePassThroughEnabled(true);

	dragContainer->addChildWindow(newItemText);
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Editor::PrototypeWindow::OnListItemClicked, this));
	dragContainer->setUserString("DragDataType", "Prototype");

	return newItem;
}

void PrototypeWindow::SetupPrototypeItem(CEGUI::ItemEntry* prototypeItem, EntitySystem::EntityHandle mPrototype)
{
	prototypeItem->setID(mPrototype.GetID());
	prototypeItem->setText(gEntityMgr.GetEntityName(mPrototype));
	prototypeItem->getChildAtIdx(0)->setID(mPrototype.GetID());
	prototypeItem->getChildAtIdx(0)->getChildAtIdx(0)->setFont("Editor");
}

void PrototypeWindow::StorePrototypeItem(CEGUI::ItemEntry* prototypeItem)
{
	CEGUI::Window* parent = prototypeItem->getParent();
	if (parent)
		parent->removeChildWindow(prototypeItem);

	mPrototypeItemCache.push_back(prototypeItem);
}

CEGUI::ItemEntry* PrototypeWindow::RestorePrototypeItem()
{
	CEGUI::ItemEntry* prototypeItem = 0;
	if (mPrototypeItemCache.empty())
	{
		prototypeItem = CreatePrototypeItem();
	}
	else
	{
		prototypeItem = mPrototypeItemCache.back();
		mPrototypeItemCache.pop_back();
	}
	return prototypeItem;
}

void PrototypeWindow::DestroyItemCache()
{
	for (ItemCache::iterator it = mPrototypeItemCache.begin(); it != mPrototypeItemCache.end(); ++it)
	{
		gGUIMgr.DestroyWindow((*it));
	}
	mPrototypeItemCache.clear();
}

void Editor::PrototypeWindow::CreatePopupMenu()
{
	mPopupMenu = gPopupMgr->CreatePopupMenu("PrototypeWindow/Popup");
	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("PrototypeWindow/Popup/InstantiatePrototype", TR("prototype_instantiate"), TR("prototype_instantiate_hint"), PI_INSTANTIATE_PROTOTYPE));
	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("PrototypeWindow/Popup/AddPrototype", TR("prototype_add"), TR("prototype_add_hint"), PI_ADD_PROTOTYPE));
	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("PrototypeWindow/Popup/DeletePrototype", TR("prototype_delete"), TR("prototype_delete_hint"), PI_DELETE_PROTOTYPE));
}

void Editor::PrototypeWindow::DestroyPopupMenu()
{
	gPopupMgr->DestroyPopupMenu(mPopupMenu);
	mPopupMenu = 0;
}

void Editor::PrototypeWindow::OnPopupMenuItemClicked(CEGUI::Window* menuItem)
{
	switch(static_cast<ePopupItem>(menuItem->getID()))
	{
	case PI_ADD_PROTOTYPE:
		ShowCreatePrototypePrompt();
		break;
	case PI_DELETE_PROTOTYPE:
		DeletePrototype(mCurrentPrototype);
		break;
	case PI_INSTANTIATE_PROTOTYPE:
		InstantiatePrototype(mCurrentPrototype);
		break;
	default:
		OC_NOT_REACHED();
	}
}

void Editor::PrototypeWindow::CreatePrototypePromptCallback(bool clickedOK, const string& text, int32 tag)
{
	OC_UNUSED(tag);
	if (!clickedOK) return;
	CreatePrototype(text);
}