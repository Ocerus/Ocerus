#include "Common.h"
#include "PopupMenu.h"
#include "EditorMgr.h"
#include "EditorGUI.h"
#include "ResourceWindow.h"
#include "PrototypeWindow.h"
#include "GUISystem/CEGUITools.h"


bool Editor::PopupMenu::OnMenuItemMouseUp( const CEGUI::EventArgs& e )
{
	gEditorMgr.DisablePopupClosing();

	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	const CEGUI::String& itemCeguiName = args.window->getName();
	CEGUI::Window* menu = gCEGUIWM.getWindow(mName);
	OC_ASSERT(menu);

	bool handled = false;

	if (itemCeguiName == "EditorRoot/Popup/Resource/ChangeType")
	{
		PopupMenu* resTypes = new PopupMenu(mName + "/Types", false);
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
	else if (itemCeguiName == "EditorRoot/Popup/Prototype/AddPrototype")
	{
		EntitySystem::EntityDescription desc;
		desc.SetKind(EntitySystem::EntityDescription::EK_PROTOTYPE);
		desc.SetName("New Prototype");
		gEntityMgr.CreateEntity(desc);
		gEditorMgr.GetEditorGui()->GetPrototypeWindow()->Refresh();
		gEntityMgr.SavePrototypes();
		handled = true;
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
	}

	size_t childCount = menu->getChildCount();
	for(size_t childIdx = 0; childIdx < childCount; childIdx++)
	{
		if (menu->getChildAtIdx(childIdx)->testClassName("MenuItem"))
		{
			mEventConnections.push_back(menu->getChildAtIdx(childIdx)->subscribeEvent(CEGUI::MenuItem::EventMouseButtonUp, CEGUI::Event::Subscriber(&PopupMenu::OnMenuItemMouseUp, this)));
		}
	}

	mInited = true;
}

Editor::PopupMenu::~PopupMenu()
{
	for (list<CEGUI::Event::Connection>::iterator it=mEventConnections.begin(); it!=mEventConnections.end(); ++it)
	{
		gGUIMgr.DisconnectEvent(*it);
	}
	
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
	x -= 0.5f * menu->getWidth().d_offset;
	y -= 0.5f * menu->getHeight().d_offset;
	menu->setPosition(CEGUI::UVector2(CEGUI::UDim(0, x), CEGUI::UDim(0, y)));
	menu->show();
	menu->activate();
}

void Editor::PopupMenu::Close()
{
	ocInfo << "Popup " << mName << " closing";
	CEGUI::Window* menu = gCEGUIWM.getWindow(mName);
	OC_ASSERT(menu);
	menu->hide();

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