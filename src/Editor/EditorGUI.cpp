#include "Common.h"
#include "EditorGUI.h"
#include "GUISystem/GUIMgr.h"

#include "CEGUI.h"

using namespace Editor;

EditorGUI::EditorGUI()
{

}

EditorGUI::~EditorGUI()
{

}

void EditorGUI::SetCurrentEntity(const EntitySystem::EntityHandle* newActiveEntity)
{
	if (newActiveEntity->IsValid() == false)
	{
		newActiveEntity = 0;
	}
	SetEntityID(newActiveEntity ? StringConverter::ToString(newActiveEntity->GetID()) : "");
	SetEntityName(newActiveEntity ? gEntityMgr.GetEntityName(*newActiveEntity) : "");

	//// Update component window
	CEGUI::Window* componentsWindow = CEGUI::WindowManager::getSingleton().getWindow("EditorRoot/ComponentsWindow/Scrollable");
	OC_ASSERT(componentsWindow);
	for (CEGUIWindows::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
	{
		componentsWindow->removeChildWindow(*it);
		CEGUI::WindowManager::getSingleton().destroyWindow(*it);
	}

	if (newActiveEntity)
	{
		EntitySystem::ComponentTypeList componentTypes;
		EntitySystem::EntityMgr::GetSingleton().GetEntityComponentTypes(*newActiveEntity, componentTypes);

		int i = 0;
		int yPos = 0;
		for (EntitySystem::ComponentTypeList::iterator it = componentTypes.begin();
			it != componentTypes.end(); ++it)
		{
			++i;
			string namePrefix = "EditorRoot/ComponentsWindow/Component" + StringConverter::ToString(i) + "/";
			CEGUI::Window* componentWindow = gGUIMgr.LoadWindowLayout("EditorComponent.layout", namePrefix);
			if (componentWindow == 0)
				continue;
			mComponents.push_back(componentWindow);
			componentsWindow->addChildWindow(componentWindow);
			componentWindow->setArea(CEGUI::UDim(0, 0), CEGUI::UDim(0, (float32)yPos), CEGUI::UDim(1, 0), CEGUI::UDim(0, (float32)yPos + 30));
			componentWindow->getChildRecursive(namePrefix + "Root/ComponentLabel")->setText(EntitySystem::GetComponentTypeName(*it));
			yPos += 35;
		}
	}
}

void EditorGUI::SetEntityID(const string& entityID)
{
	SetText("EditorRoot/EntityInfoWindow/IDValue", entityID);
}

void EditorGUI::SetEntityName(const string& entityName)
{
	SetText("EditorRoot/EntityInfoWindow/NameValue", entityName);
}

void EditorGUI::SetText(const CEGUI::String& windowName, const CEGUI::String& text)
{
	CEGUI::WindowManager::getSingleton().getWindow(windowName)->setText(text);
}

