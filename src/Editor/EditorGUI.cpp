#include "Common.h"
#include "EditorGUI.h"
#include "EditorMgr.h"
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

void EditorGUI::LoadGUI()
{
	gGUIMgr.LoadRootLayout("Editor.layout");
	CEGUI::MultiColumnList* propertyListWidget = static_cast<CEGUI::MultiColumnList*>(
	CEGUI::WindowManager::getSingleton().getWindow("EditorRoot/EntityProperties/PropertyList"));
	propertyListWidget->addColumn("Component", 0, CEGUI::UDim(0.25, 0));
	propertyListWidget->addColumn("Name", 1, CEGUI::UDim(0.25, 0));
	propertyListWidget->addColumn("Value", 2, CEGUI::UDim(0.5, 0));




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
	SetEntityID(newActiveEntity ? StringConverter::ToString(newActiveEntity->GetID()) : "");
	SetEntityName(newActiveEntity ? gEntityMgr.GetEntityName(*newActiveEntity) : "");

	//// Update component window
	UpdatePropertiesWindow(newActiveEntity);
}

bool EditorGUI::EntityPickerHandler (const CEGUI::EventArgs& args)
{
	string entityName = CEGUI::WindowManager::getSingleton().getWindow("picker/edit")->getText().c_str();
	EntitySystem::EntityHandle entity = gEntityMgr.FindFirstEntity(entityName);
	EditorMgr::GetSingleton().SetCurrentEntity(&entity);
	return true;
}

void EditorGUI::SetEntityID(const string& entityID)
{
	SetText("EditorRoot/EntityInfo/EntityIDValue", entityID);
}

void EditorGUI::SetEntityName(const string& entityName)
{
	SetText("EditorRoot/EntityInfo/EntityNameValue", entityName);
}

void EditorGUI::SetText(const CEGUI::String& windowName, const CEGUI::String& text)
{
	CEGUI::WindowManager::getSingleton().getWindow(windowName)->setText(text);
}

void EditorGUI::UpdatePropertiesWindow(const EntitySystem::EntityHandle* newActiveEntity)
{
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

	// Get components window
	CEGUI::MultiColumnList* propertyListWidget = static_cast<CEGUI::MultiColumnList*>( winMgr.getWindow("EditorRoot/EntityProperties/PropertyList"));
	OC_ASSERT(propertyListWidget);

	// Clear components window
	propertyListWidget->resetList();

	int32 componentCount = EntitySystem::EntityMgr::GetSingleton().GetNumberOfEntityComponents(*newActiveEntity);

	if (newActiveEntity)
	{
		for (int componentID = 0; componentID < componentCount; ++componentID)
		{
			const string componentName = EntitySystem::GetComponentTypeName(gEntityMgr.GetEntityComponentType(*newActiveEntity, componentID));


			//string namePrefix = "EditorRoot/ComponentsWindow/Component" + StringConverter::ToString(componentID) + "/";

			/*


			// create component name label
			CEGUI::Window* componentNameWidget = winMgr.createWindow("Editor/StaticText", namePrefix + "ComponentName");
			propertyListWidget->addChildWindow(componentNameWidget);
			componentNameWidget->setArea(CEGUI::URect(
					CEGUI::UDim(0, 2),
					CEGUI::UDim(0, (float)yPos + 2),
					CEGUI::UDim(1, -14),
					CEGUI::UDim(0, (float)yPos + 33)));
			componentNameWidget->setText(EntitySystem::GetComponentTypeName(
					gEntityMgr.GetEntityComponentType(*newActiveEntity, componentID)));

			*/

			PropertyList propertyList;
			gEntityMgr.GetEntityComponentProperties(*newActiveEntity, componentID, propertyList);
			for (PropertyList::iterator it = propertyList.begin(); it != propertyList.end(); ++it)
			{
				CEGUI::ListboxTextItem* propertyNameItem = new CEGUI::ListboxTextItem(it->GetName());
				CEGUI::ListboxTextItem* propertyValueItem = new CEGUI::ListboxTextItem(it->GetValueString());
				CEGUI::ListboxTextItem* componentItem = new CEGUI::ListboxTextItem(componentName);

				CEGUI::uint row = propertyListWidget->addRow(componentItem, 0);
				propertyListWidget->setItem(propertyNameItem, 1, row);
				propertyListWidget->setItem(propertyValueItem, 2, row);
			}
/*

			if (propertyCount != 0)
			{
				// create property list
				CEGUI::MultiColumnList* propertyListWidget = static_cast<CEGUI::MultiColumnList*>
						(winMgr.createWindow("Editor/MultiColumnList", namePrefix + "PropertyList"));
				propertyListWidget->addChildWindow(propertyListWidget);
				propertyListWidget->setArea(CEGUI::URect(
						CEGUI::UDim(0, 2),
						CEGUI::UDim(0, (float)yPos + 35),
						CEGUI::UDim(1, -14),
						CEGUI::UDim(0, (float)yPos + 35 + 35 + (35 * propertyCount))));

				propertyListWidget->addColumn("Name", 0, CEGUI::UDim((float)0.4, 0));
				propertyListWidget->addColumn("Value", 1, CEGUI::UDim((float)0.5, 0));

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
*/
		}
	}

}

