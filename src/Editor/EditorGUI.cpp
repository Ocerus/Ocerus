#include "Common.h"
#include "EditorGUI.h"
#include "GUISystem/GUIMgr.h"
#include "EntitySystem/EntityMgr/EntityMgr.h"
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
	UpdateComponentsWindow(newActiveEntity);
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

void EditorGUI::UpdateComponentsWindow(const EntitySystem::EntityHandle* newActiveEntity)
{
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

	// Get components window
	CEGUI::Window* componentsWindow = winMgr.getWindow("EditorRoot/ComponentsWindow/Scrollable");
	OC_ASSERT(componentsWindow);

	// Clear components window
	for (CEGUIWindows::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
	{
		componentsWindow->removeChildWindow(*it);
		winMgr.destroyWindow(*it);
	}

	if (newActiveEntity)
	{

		// Set components and properties
		int yPos = 2;
		const int componentCount = gEntityMgr.GetNumberOfEntityComponents(*newActiveEntity);
		for (int componentID = 0; componentID < componentCount; ++componentID)
		{

			string namePrefix = "EditorRoot/ComponentsWindow/Component" + StringConverter::ToString(componentID) + "/";



			// create component name label
			CEGUI::Window* componentNameWidget = winMgr.createWindow("Editor/StaticText", namePrefix + "ComponentName");
			componentsWindow->addChildWindow(componentNameWidget);
			componentNameWidget->setArea(CEGUI::URect(
					CEGUI::UDim(0, 2),
					CEGUI::UDim(0, yPos + 2),
					CEGUI::UDim(1, -14),
					CEGUI::UDim(0, yPos + 33)));
			componentNameWidget->setText(EntitySystem::GetComponentTypeName(
					gEntityMgr.GetEntityComponentType(*newActiveEntity, componentID)));

			PropertyList propertyList;
			EntitySystem::EntityMgr::GetSingleton().GetEntityComponentProperties(*newActiveEntity, componentID, propertyList);
			int propertyCount = propertyList.size();
			ocWarning << gEntityMgr.GetEntityComponentType(*newActiveEntity, componentID) << ":" << propertyCount;

			if (propertyCount != 0)
			{
				// create property list
				CEGUI::MultiColumnList* propertyListWidget = static_cast<CEGUI::MultiColumnList*>
						(winMgr.createWindow("Editor/MultiColumnList", namePrefix + "PropertyList"));
				componentsWindow->addChildWindow(propertyListWidget);
				propertyListWidget->setArea(CEGUI::URect(
						CEGUI::UDim(0, 2),
						CEGUI::UDim(0, yPos + 35),
						CEGUI::UDim(1, -14),
						CEGUI::UDim(0, yPos + 35 + 35 + (35 * propertyCount))));

				propertyListWidget->addColumn("Name", 0, CEGUI::UDim(0.4, 0));
				propertyListWidget->addColumn("Value", 1, CEGUI::UDim(0.5, 0));

				for (PropertyList::iterator it = propertyList.begin(); it != propertyList.end(); ++it)
				{
					CEGUI::ListboxTextItem* propertyName = new CEGUI::ListboxTextItem(it->GetName());
					CEGUI::ListboxTextItem* propertyValue = new CEGUI::ListboxTextItem(it->GetValueString());

					int newRow = propertyListWidget->addRow(propertyName, 0);
					propertyListWidget->setItem(propertyValue, 1, newRow);
				}
				yPos += 35 + (35 * propertyCount);
			}

			yPos += 35 + 5;
		}
	}

}

