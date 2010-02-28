#include "Common.h"
#include "EditorGUI.h"
#include "EditorMgr.h"
#include "GUISystem/GUIMgr.h"
#include "EntitySystem/EntityMgr/EntityMgr.h"
#include "CEGUI.h"

#define gCEGUIWindowManager CEGUI::WindowManager::getSingleton()

namespace Editor
{
	const string ENTITY_EDITOR_NAME = "EditorRoot/EntityEditor/Scrollable";
	const int ENTITY_EDITOR_ITEM_HEIGHT = 26;
	const int ENTITY_EDITOR_GROUP_HEIGHT = 30;
}


using namespace Editor;

EditorGUI::EditorGUI()
{
}

EditorGUI::~EditorGUI()
{

}

void EditorGUI::LoadGUI()
{
	gGUIMgr.LoadRootLayout("Editor.layout");
	CEGUI::System::getSingleton().setDefaultTooltip("Editor/Tooltip");

/*
	CEGUI::MultiColumnList* propertyListWidget = static_cast<CEGUI::MultiColumnList*>(
	CEGUI::WindowManager::getSingleton().getWindow("EditorRoot/EntityProperties/PropertyList"));
	propertyListWidget->addColumn("Component", 0, CEGUI::UDim(0.25, 0));
	propertyListWidget->addColumn("Name", 1, CEGUI::UDim(0.25, 0));
	propertyListWidget->addColumn("Value", 2, CEGUI::UDim(0.5, 0));
*/



	/// DOCASNE
	CEGUI::FrameWindow* picker = (CEGUI::FrameWindow*)CEGUI::WindowManager::getSingleton().createWindow("Editor/FrameWindow", "picker");
	picker->setText("EntityPicker");
	picker->setSize(CEGUI::UVector2(CEGUI::UDim(0, 200), CEGUI::UDim(0, 50)));
	picker->setPosition(CEGUI::UVector2(CEGUI::UDim(1, -210), CEGUI::UDim(1, -80)));
	CEGUI::WindowManager::getSingleton().getWindow("EditorRoot")->addChildWindow(picker);

	CEGUI::Editbox* pickerInput = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().createWindow("Editor/Editbox", "picker/edit");
	picker->addChildWindow(pickerInput);
	pickerInput->setArea(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0));

	pickerInput->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&EditorGUI::EntityPickerHandler, this));
}

void EditorGUI::SetCurrentEntity(const EntitySystem::EntityHandle* newActiveEntity)
{
	if (newActiveEntity->IsValid() == false)
	{
		newActiveEntity = 0;
	}

	UpdateEntityEditorWindow(newActiveEntity);
}

bool EditorGUI::EntityPickerHandler (const CEGUI::EventArgs& args)
{
	string entityName = CEGUI::WindowManager::getSingleton().getWindow("picker/edit")->getText().c_str();
	EntitySystem::EntityHandle entity = gEntityMgr.FindFirstEntity(entityName);
	EditorMgr::GetSingleton().SetCurrentEntity(&entity);
	return true;
}

void EditorGUI::SetText(const CEGUI::String& windowName, const CEGUI::String& text)
{
	CEGUI::WindowManager::getSingleton().getWindow(windowName)->setText(text);
}

void EditorGUI::UpdateEntityEditorWindow(const EntitySystem::EntityHandle* newActiveEntity)
{
	CEGUI::ScrollablePane* entityEditorPane = static_cast<CEGUI::ScrollablePane*>( gCEGUIWindowManager.getWindow(ENTITY_EDITOR_NAME));
	OC_ASSERT(entityEditorPane);

	// Clear all the content of Entity Editor
	{
		const CEGUI::Window* entityEditorContentPane = entityEditorPane->getContentPane();
		OC_ASSERT(entityEditorContentPane);

		int childCount = entityEditorContentPane->getChildCount();
		for (int i = (childCount - 1); i >= 0; --i)
		{
			gCEGUIWindowManager.destroyWindow(entityEditorContentPane->getChildAtIdx(i));
		}
	}

	int newGroupY = 22;			// The y coord for new group
	const int groupHeight = 30;	// The height of an empty group. Items must be summed up to get the total height of a group.


	// First "component" is general entity info
	{
		const string namePrefix = ENTITY_EDITOR_NAME + "/ComponentGeneralInfo";
		CEGUI::Window* componentGroup = gCEGUIWindowManager.createWindow("Editor/GroupBox", namePrefix);
		componentGroup->setText("General Info");
		entityEditorPane->addChildWindow(componentGroup);

		int currentItem = 0;
		appendEntityEditorItem(componentGroup, "Entity ID", newActiveEntity ? StringConverter::ToString(newActiveEntity->GetID()) : 0, currentItem++, "Unique ID of the entity. It cannot be modified.", true);
		appendEntityEditorItem(componentGroup, "Entity Name", newActiveEntity ? gEntityMgr.GetEntityName(*newActiveEntity) : "", currentItem++, "Name of the entity.");

		componentGroup->setArea(CEGUI::URect(CEGUI::UDim(0, 4), CEGUI::UDim(0, (float)newGroupY + 1), CEGUI::UDim(1, -4), CEGUI::UDim(0, (float)newGroupY + groupHeight + (currentItem * ENTITY_EDITOR_ITEM_HEIGHT) - 1)));
		newGroupY += groupHeight + (currentItem * ENTITY_EDITOR_ITEM_HEIGHT);
	}

	if (newActiveEntity == 0)
		return;

	int32 componentCount = EntitySystem::EntityMgr::GetSingleton().GetNumberOfEntityComponents(*newActiveEntity);

	if (newActiveEntity)
	{
		for (int componentID = 0; componentID < componentCount; ++componentID)
		{
			const string namePrefix = ENTITY_EDITOR_NAME + "/Component" + StringConverter::ToString(componentID);
			const string componentName = EntitySystem::GetComponentTypeName(gEntityMgr.GetEntityComponentType(*newActiveEntity, componentID));

			CEGUI::Window* componentGroup = gCEGUIWindowManager.createWindow("Editor/GroupBox", namePrefix);
			componentGroup->setText(componentName);
			entityEditorPane->addChildWindow(componentGroup);

			PropertyList propertyList;
			gEntityMgr.GetEntityComponentProperties(*newActiveEntity, componentID, propertyList);
			int currentItem = 0;
			for (PropertyList::iterator it = propertyList.begin(); it != propertyList.end(); ++it)
			{
				appendEntityEditorItem(componentGroup, it->GetName(), it->GetValueString(), currentItem, it->GetComment());

				currentItem++;
			}

			componentGroup->setArea(CEGUI::URect(CEGUI::UDim(0, 4), CEGUI::UDim(0, (float)newGroupY + 1), CEGUI::UDim(1, -4), CEGUI::UDim(0, (float)newGroupY + groupHeight + (currentItem * ENTITY_EDITOR_ITEM_HEIGHT))));
			newGroupY += groupHeight + (currentItem * ENTITY_EDITOR_ITEM_HEIGHT);
		}
	}
}


void EditorGUI::appendEntityEditorItem(CEGUI::Window* componentGroup, const string& itemName, const string& itemValue, int itemPos, const string& itemTooltip, bool readOnly)
{
	const CEGUI::String& prefixName = componentGroup->getName();
	CEGUI::Window* groupItemName = gCEGUIWindowManager.createWindow("Editor/StaticText", prefixName + "/" + itemName + "Label");
	groupItemName->setText(itemName);
	groupItemName->setProperty("FrameEnabled", "False");
	groupItemName->setProperty("BackgroundEnabled", "False");
	groupItemName->setTooltipText(itemTooltip);
	groupItemName->setArea(CEGUI::URect(CEGUI::UDim(0, 2), CEGUI::UDim(0, (float)(itemPos * ENTITY_EDITOR_ITEM_HEIGHT) + 1), CEGUI::UDim(0.5, -1), CEGUI::UDim(0, (float)(itemPos + 1) * ENTITY_EDITOR_ITEM_HEIGHT - 1)));
	componentGroup->addChildWindow(groupItemName);

	CEGUI::Window* groupItemValue = gCEGUIWindowManager.createWindow("Editor/Editbox", prefixName + "/" + itemName + "Value");
	groupItemValue->setText(itemValue);
	groupItemValue->setProperty("ReadOnly", readOnly ? "True" : "False");
	groupItemValue->setArea(CEGUI::URect(CEGUI::UDim(0.5, 1), CEGUI::UDim(0, (float)(itemPos * ENTITY_EDITOR_ITEM_HEIGHT) + 1), CEGUI::UDim(1, -1), CEGUI::UDim(0, (float)(itemPos + 1) * ENTITY_EDITOR_ITEM_HEIGHT - 1)));
	componentGroup->addChildWindow(groupItemValue);
}