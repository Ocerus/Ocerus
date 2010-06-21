#include "Common.h"
#include "PopupMenu.h"
#include "EditorMgr.h"
#include "EditorGUI.h"
#include "ResourceWindow.h"
#include "PrototypeWindow.h"
#include "HierarchyWindow.h"
#include "GUISystem/CEGUITools.h"


bool Editor::PopupMenu::OnMenuItemMouseUp( const CEGUI::EventArgs& e )
{
	CEGUI::Window* menu = gCEGUIWM.getWindow(mName);
	OC_ASSERT(menu);

	if (!menu->isVisible()) return false;

	gEditorMgr.DisablePopupClosing();

	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	const CEGUI::String& itemCeguiName = args.window->getName();

	bool handled = false;

	if (itemCeguiName == "EditorRoot/Popup/Resource/ChangeType")
	{
		PopupMenu* resTypes = new PopupMenu(mName + "/Types");
		CEGUI::UVector2 newPos = menu->getPosition() + CEGUI::UVector2(menu->getWidth(), menu->getHeight()) * CEGUI::UDim(0.0f, 0.5f);
		resTypes->Init<ResourceSystem::ResourcePtr>(GetData<ResourceSystem::ResourcePtr>());
		resTypes->Open(newPos.d_x.d_offset, newPos.d_y.d_offset);
		gEditorMgr.RegisterPopupMenu(resTypes);
		handled = true;
	}
	else if (itemCeguiName.substr(0, itemCeguiName.size()-1) == "EditorRoot/Popup/Resource/Types/Type")
	{
		ResourceSystem::eResourceType newType = (ResourceSystem::eResourceType)args.window->getID();
		gResourceMgr.ChangeResourceType(GetData<ResourceSystem::ResourcePtr>(), newType);
		gEditorMgr.GetEditorGui()->GetResourceWindow()->Refresh();
		handled = true;
	}
	else if (mName.find("EditorRoot/Popup/Prototype") == 0)
	{
		if (itemCeguiName == mName + "/AddPrototype")
		{
			EntitySystem::EntityDescription desc;
			desc.SetKind(EntitySystem::EntityDescription::EK_PROTOTYPE);
			desc.SetName("New Prototype");
			gEntityMgr.CreateEntity(desc);
			gEditorMgr.GetEditorGui()->GetPrototypeWindow()->Refresh();
			gEntityMgr.SavePrototypes();
			handled = true;
		}
		else if (itemCeguiName == mName + "/DeletePrototype")
		{
			gEntityMgr.DestroyEntity(GetData<EntitySystem::EntityHandle>());
			gEntityMgr.ProcessDestroyQueue();
			gEditorMgr.GetEditorGui()->GetPrototypeWindow()->Refresh();
			gEntityMgr.SavePrototypes();
			handled = true;
		}
		else if (itemCeguiName == mName + "/InstantiatePrototype")
		{
			gEditorMgr.SetCurrentEntity(gEntityMgr.InstantiatePrototype(GetData<EntitySystem::EntityHandle>()));
			handled = true;
		}
	}
	else if (mName.find("EditorRoot/Popup/Entity") == 0)
	{
		if (itemCeguiName == mName + "/AddEntity")
		{
			gEditorMgr.GetEditorGui()->GetHierarchyWindow()->SetCurrentParent(GetData<EntitySystem::EntityHandle>());
			gEditorMgr.CreateEntity();
			gEditorMgr.GetEditorGui()->GetHierarchyWindow()->SetCurrentParent(EntitySystem::EntityHandle::Null);
			handled = true;
		}
		else if (itemCeguiName == mName + "/NewComponent")
		{
		  return true;
		}
		else if (itemCeguiName == mName + "/DuplicateEntity")
		{
			EntitySystem::EntityHandle parent = gEditorMgr.GetEditorGui()->GetHierarchyWindow()->GetParent(GetData<EntitySystem::EntityHandle>());
			gEditorMgr.GetEditorGui()->GetHierarchyWindow()->SetCurrentParent(parent);
			gEditorMgr.DuplicateCurrentEntity();
			gEditorMgr.GetEditorGui()->GetHierarchyWindow()->SetCurrentParent(EntitySystem::EntityHandle::Null);
			handled = true;
		}
		else if (itemCeguiName == mName + "/DeleteEntity")
		{
			gEditorMgr.DeleteCurrentEntity();
			handled = true;
		}
		else if (itemCeguiName == mName + "/CreatePrototype")
		{
			gEditorMgr.CreatePrototypeFromCurrentEntity();
			handled = true;
		}
		
		/// New component
	  {
		  string pattern = mName + "/NewComponent/Component";
		  if (itemCeguiName.substr(0, pattern.size()) == pattern)
		  {
			  OC_DASSERT(itemCeguiName.size() > pattern.size());
			  int componentType = StringConverter::FromString<int>(string(itemCeguiName.substr(pattern.size()).c_str()));
			  gEditorMgr.AddComponent((EntitySystem::eComponentType)componentType);
			  handled = true;
		  }
	  }
	}

	Close();
	return handled;
}

Editor::PopupMenu::PopupMenu( const string& menuName, const bool selfDestruct ):
	mName(menuName), mSelfDestruct(selfDestruct), mInited(false), mData(0)
{

}

void Editor::PopupMenu::Init()
{
	CEGUI::Window* menu = gCEGUIWM.getWindow(mName);
	OC_ASSERT(menu);
	mEventConnections.push_back(menu->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&PopupMenu::OnMenuMouseUp, this)));

	if (mName == "EditorRoot/Popup/Resource/Types")
	{
		InitResourceTypes();
	} else if (mName == "EditorRoot/Popup/EntityAboveItem") {
	  InitComponentTypes();
	}

	mInited = true;
}

Editor::PopupMenu::~PopupMenu()
{
	gEditorMgr.UnregisterPopupMenu(this);

	if (mData) delete mData;
}

void Editor::PopupMenu::Open( float32 x, float32 y )
{
	OC_ASSERT(mInited);

	gEditorMgr.DisablePopupClosing();

	ocInfo << "Popup " << mName << " opening";
	CEGUI::Window* menu = gCEGUIWM.getWindow(mName);
	OC_ASSERT(menu);

	ConfigureMenu(menu);

	x -= 0.5f * menu->getWidth().d_offset;
	y -= 0.5f * menu->getHeight().d_offset;
	menu->setPosition(CEGUI::UVector2(CEGUI::UDim(0, x), CEGUI::UDim(0, y)));
	menu->show();
	menu->activate();
}

void Editor::PopupMenu::ConfigureMenu(CEGUI::Window* parent)
{
  size_t childCount = parent->getChildCount();
	for(size_t childIdx = 0; childIdx < childCount; childIdx++)
	{
		CEGUI::Window* child = parent->getChildAtIdx(childIdx);
		if (child->testClassName("MenuItem"))
		{
			mEventConnections.push_back(child->subscribeEvent(CEGUI::MenuItem::EventMouseButtonUp, 
			  CEGUI::Event::Subscriber(&PopupMenu::OnMenuItemMouseUp, this)));
		}
		ConfigureMenu(child);
	}
}

void Editor::PopupMenu::Close()
{
	CEGUI::Window* menu = gCEGUIWM.getWindow(mName);
	OC_ASSERT(menu);
	if (!menu->isVisible()) return;
	ocInfo << "Popup " << mName << " closing";
	
	menu->hide();

	for (list<CEGUI::Event::Connection>::iterator it=mEventConnections.begin(); it!=mEventConnections.end(); ++it)
	{
		gGUIMgr.DisconnectEvent(*it);
	}

	if (mSelfDestruct) delete this;
}

bool Editor::PopupMenu::OnMenuMouseUp( const CEGUI::EventArgs& )
{
	gEditorMgr.DisablePopupClosing();
	return true;
}

void Editor::PopupMenu::InitResourceTypes()
{
	CEGUI::Window* menu = gCEGUIWM.getWindow(mName);
	OC_ASSERT(menu);

	size_t childCount = menu->getChildCount();
	for (int i = (childCount - 1); i >= 0; --i)
	{
		gCEGUIWM.destroyWindow(menu->getChildAtIdx(i));
	}

	for (int resType=0; resType<ResourceSystem::NUM_RESTYPES; ++resType)
	{
		string resName = ResourceSystem::GetResourceTypeName((ResourceSystem::eResourceType)resType);
		CEGUI::Window* menuItem = gCEGUIWM.createWindow("Editor/MenuItem", mName + "/Type" + StringConverter::ToString(resType));
		menuItem->setText(resName);
		menuItem->setID(resType);
		menu->addChildWindow(menuItem);
	}

	ResourceSystem::eResourceType myType = GetData<ResourceSystem::ResourcePtr>()->GetType();
	menu->getChildAtIdx(myType)->appendText(" *");
}

void Editor::PopupMenu::InitComponentTypes()
{
  CEGUI_EXCEPTION_BEGIN
	CEGUI::Window* addComponentMenu = gCEGUIWM.getWindow("EditorRoot/Popup/EntityAboveItem/NewComponent/AutoPopup");
	OC_ASSERT(addComponentMenu);
	
	size_t childCount = addComponentMenu->getChildCount();
	for (int i = (childCount - 1); i >= 0; --i)
	{
		gCEGUIWM.destroyWindow(addComponentMenu->getChildAtIdx(i));
	}
	
	for (int32 i = 0; i < EntitySystem::NUM_COMPONENT_TYPES; ++i)
	{
		const string& componentName = EntitySystem::GetComponentTypeName((EntitySystem::eComponentType)i);
		CEGUI::Window* componentMenuItem = gCEGUIWM.createWindow("Editor/MenuItem",
				"EditorRoot/Popup/EntityAboveItem/NewComponent/Component" + StringConverter::ToString(i));
		componentMenuItem->setText(componentName);
		addComponentMenu->addChildWindow(componentMenuItem);
	}
	CEGUI_EXCEPTION_END
}