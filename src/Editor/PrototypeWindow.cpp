#include "Common.h"
#include "PrototypeWindow.h"
#include "PopupMenu.h"
#include "EditorMgr.h"
#include "GUISystem/CEGUICommon.h"
#include "GUISystem/PopupMgr.h"
#include "GUISystem/PromptBox.h"

using namespace Editor;

Editor::PrototypeWindow::PrototypeWindow()
{
}

Editor::PrototypeWindow::~PrototypeWindow()
{
}

void Editor::PrototypeWindow::Init()
{
	mSelectedIndex = -1;

	CEGUI_TRY;
	{
		mWindow = gGUIMgr.LoadSystemLayout("PrototypeWindow.layout", "EditorRoot/PrototypeWindow");
		OC_ASSERT(mWindow != 0);
		gGUIMgr.GetGUISheet()->addChildWindow(mWindow);
		mTree = static_cast<CEGUI::ItemListbox*>(mWindow->getChild(mWindow->getName() + "/List"));
		mTree->setUserString("WantsMouseWheel", "True");
		OC_ASSERT(mTree != 0);
		
		mTree->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::PrototypeWindow::OnWindowMouseButtonUp, this));
	}
	CEGUI_CATCH;

	CreatePopupMenu();
	RebuildTree();
}

void PrototypeWindow::NewPrototype()
{
	GUISystem::PromptBox* prompt = new GUISystem::PromptBox();
	prompt->SetText(TR("layer_new_prompt"));
	prompt->RegisterCallback(GUISystem::PromptBox::Callback(this, &PrototypeWindow::NewPrototypePromptCallback));
	prompt->Show();
}

void PrototypeWindow::DeletePrototype(EntitySystem::EntityHandle entity)
{
	gEntityMgr.DestroyEntity(entity);
	gEntityMgr.ProcessDestroyQueue();
	Refresh();
	gEntityMgr.SavePrototypes();
}

void PrototypeWindow::InstantiatePrototype(EntitySystem::EntityHandle entity)
{
	gEditorMgr.SetCurrentEntity(gEntityMgr.InstantiatePrototype(entity));
}

void Editor::PrototypeWindow::RebuildTree()
{
	for (int32 i=mTree->getItemCount()-1; i>=0; --i)
	{
		gGUIMgr.DestroyWindow(mTree->getItemFromIndex(i));
	}
	OC_ASSERT(mTree->getItemCount() == 0);
	mItems.clear();

	gEntityMgr.ProcessDestroyQueue();
	gEntityMgr.GetPrototypes(mItems);

	for (EntitySystem::EntityList::iterator it=mItems.begin(); it!=mItems.end(); ++it)
	{
		size_t itemIndex = (size_t)(it - mItems.begin());

		CEGUI::ItemEntry* newItem = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("Editor/ListboxItem"));
		newItem->setID(itemIndex);
		
		CEGUI::Window* dragContainer = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("DragContainer"));
		dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		newItem->addChildWindow(dragContainer);

		CEGUI::Window* newItemText = gGUIMgr.CreateWindow("Editor/StaticText");
		newItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		newItemText->setText(gEntityMgr.GetEntityName(*it));
		newItemText->setProperty("FrameEnabled", "False");
		newItemText->setProperty("BackgroundEnabled", "False");
		newItemText->setMousePassThroughEnabled(true);

		dragContainer->addChildWindow(newItemText);
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::PrototypeWindow::OnDragContainerMouseButtonUp, this));

		dragContainer->setID(itemIndex);
		dragContainer->setUserString("DragDataType", "Prototype");
		dragContainer->setUserData(this);

		mTree->addChildWindow(newItem);

		if (*it == gEditorMgr.GetCurrentEntity()) newItem->select();
	}
}

bool Editor::PrototypeWindow::OnDragContainerMouseButtonUp(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	CEGUI::ItemEntry* itemEntry = static_cast<CEGUI::ItemEntry*>(args.window->getParent());
	itemEntry->setSelected(true);
	mSelectedIndex = dragContainer->getID();
	
	mCurrentPopupPrototype = GetItemAtIndex(mSelectedIndex);
	if (!mCurrentPopupPrototype.IsValid())
		return true;

	if (args.button == CEGUI::LeftButton)
	{
		gEditorMgr.SetCurrentEntity(mCurrentPopupPrototype);
		gEditorMgr.ClearSelection();
	}
	else if (args.button == CEGUI::RightButton)
	{
		mPopupMenu->getChildAtIdx(0)->setEnabled(true);
		mPopupMenu->getChildAtIdx(2)->setEnabled(true);
		gPopupMgr->ShowPopup(mPopupMenu, args.position.d_x, args.position.d_y, GUISystem::PopupMgr::Callback(this, &Editor::PrototypeWindow::OnPopupMenuItemClicked));
	}
	return true;
}

bool Editor::PrototypeWindow::OnWindowMouseButtonUp(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);

	if (args.button == CEGUI::RightButton)
	{
		mPopupMenu->getChildAtIdx(0)->setEnabled(false);
		mPopupMenu->getChildAtIdx(2)->setEnabled(false);
		gPopupMgr->ShowPopup(mPopupMenu, args.position.d_x, args.position.d_y, GUISystem::PopupMgr::Callback(this, &Editor::PrototypeWindow::OnPopupMenuItemClicked));
		return true;
	}
	else if (args.button == CEGUI::LeftButton)
	{
		if (mSelectedIndex != -1)
		{
			gEditorMgr.SetCurrentEntity(EntitySystem::EntityHandle::Null);
		}
	}

	return true;
}

EntitySystem::EntityHandle Editor::PrototypeWindow::GetItemAtIndex( size_t index )
{
	if (index >= mItems.size()) return EntitySystem::EntityHandle::Null;
	return mItems.at(index);
}

EntitySystem::EntityHandle Editor::PrototypeWindow::GetSelectedItem()
{
	if (mTree->getSelectedCount() == 0) return EntitySystem::EntityHandle::Null;
	else return mItems.at(mSelectedIndex);
}

void Editor::PrototypeWindow::SetSelectedEntity( const EntitySystem::EntityHandle entity )
{
	int32 index = 0;
	for (EntitySystem::EntityList::iterator it=mItems.begin(); it!=mItems.end(); ++it, ++index)
	{
		if (*it == entity)
		{
			mSelectedIndex = index;
			mTree->selectRange(mSelectedIndex, mSelectedIndex);
			return;
		}
	}

	mSelectedIndex = -1;
	mTree->clearAllSelections();
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
}

void Editor::PrototypeWindow::OnPopupMenuItemClicked(CEGUI::Window* menuItem)
{
	switch(static_cast<ePopupItem>(menuItem->getID()))
	{
	case PI_ADD_PROTOTYPE:
		NewPrototype();
		break;
	case PI_DELETE_PROTOTYPE:
		DeletePrototype(mCurrentPopupPrototype);
		break;
	case PI_INSTANTIATE_PROTOTYPE:
		InstantiatePrototype(mCurrentPopupPrototype);
		break;
	default:
		OC_NOT_REACHED();
	}

}

void Editor::PrototypeWindow::NewPrototypePromptCallback(bool clickedOK, const string& text, int32 tag)
{
	OC_UNUSED(tag);
	if (!clickedOK) return;
	EntitySystem::EntityDescription desc;
	desc.SetKind(EntitySystem::EntityDescription::EK_PROTOTYPE);
	desc.SetName(text);
	gEntityMgr.CreateEntity(desc);
	Refresh();
	gEntityMgr.SavePrototypes();
}
