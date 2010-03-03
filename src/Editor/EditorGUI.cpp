#include "Common.h"
#include "EditorGUI.h"
#include "EditorMgr.h"
#include "PropertyEditors/PropertyEditorCreator.h"

#include "GUISystem/GUIMgr.h"
#include "EntitySystem/EntityMgr/EntityMgr.h"

#include "CEGUI.h"



#define gCEGUIWM CEGUI::WindowManager::getSingleton()

namespace Editor
{
	const string ENTITY_EDITOR_NAME = "EditorRoot/EntityEditor/Scrollable";
	const int ENTITY_EDITOR_ITEM_HEIGHT = 26;
	const int ENTITY_EDITOR_GROUP_HEIGHT = 30;
}


using namespace Editor;

EditorGUI::EditorGUI():
	mPropertyItemHeight(0),
	mComponentGroupHeight(0)
{
}

EditorGUI::~EditorGUI()
{

}

void EditorGUI::LoadGUI()
{
	gGUIMgr.LoadRootLayout("Editor.layout");
	CEGUI::System::getSingleton().setDefaultTooltip("Editor/Tooltip");
	mPropertyItemHeight = (int32)gCEGUIWM.getWindow("EditorRoot")->getFont(true)->getLineSpacing(1.1f) + 10;
	mComponentGroupHeight = 28;

	/// TO BE REMOVED
	{
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
}


void EditorGUI::UpdateEntityEditorWindow()
{
	const EntitySystem::EntityHandle* currentEntity = gEditorMgr.GetCurrentEntity();

	if (currentEntity && !currentEntity->IsValid())
		currentEntity = 0;

	CEGUI::ScrollablePane* entityEditorPane = static_cast<CEGUI::ScrollablePane*>(gCEGUIWM.getWindow(ENTITY_EDITOR_NAME));
	OC_ASSERT(entityEditorPane);

	// Clear all the content of Entity Editor
	{
		const CEGUI::Window* entityEditorContentPane = entityEditorPane->getContentPane();
		OC_ASSERT(entityEditorContentPane);

		int childCount = entityEditorContentPane->getChildCount();
		for (int i = (childCount - 1); i >= 0; --i)
		{
			gCEGUIWM.destroyWindow(entityEditorContentPane->getChildAtIdx(i));
		}
	}

	int newGroupY = 0; // The y coord for new group

	// First "component" is general entity info
	{
		const string namePrefix = ENTITY_EDITOR_NAME + "/ComponentGeneralInfo";
		CEGUI::Window* componentGroup = gCEGUIWM.createWindow("Editor/GroupBox", namePrefix);
		componentGroup->setText("General Info");
		entityEditorPane->addChildWindow(componentGroup);

		int currentItem = 0;
		AppendPropertyItem(componentGroup, 0, "Entity ID", currentEntity ? StringConverter::ToString(currentEntity->GetID()) : 0, "entityId", currentItem++, "Unique ID of the entity. It cannot be modified.", true);
		AppendPropertyItem(componentGroup, 0, "Entity Name", currentEntity ? gEntityMgr.GetEntityName(*currentEntity) : "", "entityName", currentItem++, "Name of the entity.");

		componentGroup->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, (float)newGroupY + 1), CEGUI::UDim(1, 0), CEGUI::UDim(0, (float)newGroupY + mComponentGroupHeight + (currentItem * ENTITY_EDITOR_ITEM_HEIGHT) - 1)));
		newGroupY += mComponentGroupHeight + (currentItem * mPropertyItemHeight);
	}

	if (currentEntity == 0)
		return;

	int32 componentCount = EntitySystem::EntityMgr::GetSingleton().GetNumberOfEntityComponents(*currentEntity);

	for (int componentID = 0; componentID < componentCount; ++componentID)
	{
		const string namePrefix = ENTITY_EDITOR_NAME + "/Component" + StringConverter::ToString(componentID);
		const string componentName = EntitySystem::GetComponentTypeName(gEntityMgr.GetEntityComponentType(*currentEntity, componentID));

		CEGUI::Window* componentGroup = gCEGUIWM.createWindow("Editor/GroupBox", namePrefix);
		componentGroup->setText(componentName);
		entityEditorPane->addChildWindow(componentGroup);

		for (PropertyEditors::const_iterator it = mPropertyEditors.begin(); it != mPropertyEditors.end(); ++it)
		{
			(*it)->ResetUpdateFlag();
		}

		PropertyList propertyList;
		gEntityMgr.GetEntityComponentProperties(*currentEntity, componentID, propertyList);
		int currentY = 0;
		for (PropertyList::iterator it = propertyList.begin(); it != propertyList.end(); ++it)
		{
			// Check whether this property is suposed to be shown in editor.
			if (!(it->GetAccessFlags() & Reflection::PA_EDIT_READ))
				continue;

			AbstractPropertyEditor* editor = 0;
			for (PropertyEditors::iterator eit = mPropertyEditors.begin(); eit != mPropertyEditors.end(); ++eit)
			{
				if ((*eit)->GetProperty() == *it)
				{
					editor = *eit;
					break;
				}
			}

            CEGUI::Window* labelWidget = 0;
            CEGUI::Window* editorWidget = 0;

            if (editor == 0)
			{
				// Create an editor for this property
				editor = CreatePropertyEditor(*it);
			    labelWidget = editor->CreateLabelWidget(componentGroup);
				editorWidget = editor->CreateEditorWidget(componentGroup);
			
			}
			else
			{
				// Update the editor for this property
				editor->UpdateEditorWidget();
			}
            labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 2), CEGUI::UDim(0, (float)(currentY + 1)), CEGUI::UDim(0.5, -1), CEGUI::UDim(0, (float32)(currentY + editor->GetEditorHeight()))));
            editorWidget->setArea(CEGUI::URect(CEGUI::UDim(0.5, 1), CEGUI::UDim(0, (float)(currentY + 1)), CEGUI::UDim(1, -1), CEGUI::UDim(0, (float32)(currentY + editor->GetEditorHeight()))));
			currentY += editor->GetEditorHeight() + 2;
		}

		componentGroup->setArea(CEGUI::URect(CEGUI::UDim(0, 4), CEGUI::UDim(0, (float)newGroupY + 1), CEGUI::UDim(1, -4), CEGUI::UDim(0, (float)newGroupY + currentY)));
		newGroupY += currentY;
	}
}


/// TO BE REMOVED
bool EditorGUI::EntityPickerHandler (const CEGUI::EventArgs& args)
{
	string entityName = gCEGUIWM.getWindow("picker/edit")->getText().c_str();
	EntitySystem::EntityHandle entity = gEntityMgr.FindFirstEntity(entityName);
	EditorMgr::GetSingleton().SetCurrentEntity(&entity);
	return true;
}


void EditorGUI::AppendPropertyItem(CEGUI::Window* componentGroup, ComponentID componentID, const string& propertyName, const string& propertyValue, const string& propertyKey, int propertyPos, const string& propertyTooltip, bool readOnly)
{
	const CEGUI::String& prefixName = componentGroup->getName();

	CEGUI::Window* propertyNameLabel = gCEGUIWM.createWindow("Editor/StaticText", prefixName + "/" + propertyKey + "Label");
	propertyNameLabel->setText(propertyName);
	propertyNameLabel->setProperty("FrameEnabled", "False");
	propertyNameLabel->setProperty("BackgroundEnabled", "False");
	propertyNameLabel->setTooltipText(propertyTooltip);
	propertyNameLabel->setArea(CEGUI::URect(CEGUI::UDim(0, 2), CEGUI::UDim(0, (float)(propertyPos * mPropertyItemHeight) + 1), CEGUI::UDim(0.5, -1), CEGUI::UDim(0, (float)(propertyPos + 1) * mPropertyItemHeight - 1)));
	componentGroup->addChildWindow(propertyNameLabel);

	CEGUI::Window* propertyValueEditbox = gCEGUIWM.createWindow("Editor/Editbox", prefixName + "/" + propertyKey + "Value");
	propertyValueEditbox->setText(propertyValue);
	propertyValueEditbox->setProperty("ReadOnly", readOnly ? "True" : "False");
	propertyValueEditbox->setArea(CEGUI::URect(CEGUI::UDim(0.5, 1), CEGUI::UDim(0, (float)(propertyPos * mPropertyItemHeight) + 1), CEGUI::UDim(1, -1), CEGUI::UDim(0, (float)(propertyPos + 1) * mPropertyItemHeight - 1)));
	propertyValueEditbox->setUserString("PropertyKey", propertyKey);
	propertyValueEditbox->setUserString("ComponentID", Utils::StringConverter::ToString(componentID));
	componentGroup->addChildWindow(propertyValueEditbox);

	if (!readOnly)
	{
		propertyValueEditbox->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&Editor::EditorGUI::OnEntityEditorItemChanged, this));
	}

}

bool EditorGUI::OnEntityEditorItemChanged(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);

	const string newValue = args.window->getText().c_str();
	const string propertyKey = args.window->getUserString("PropertyKey").c_str();
	EntitySystem::ComponentID componentID = Utils::StringConverter::FromString<int32>(args.window->getUserString("ComponentID").c_str());


	if (componentID != 0)
	{
		/// The _genuine_ property was modified
		EditorMgr::GetSingleton().UpdateCurrentEntityProperty(componentID, propertyKey, newValue);
	}
	else
	{
		/// One of our pseudo property from General Info "component" was modified
		if (propertyKey == "entityName") {
			EditorMgr::GetSingleton().UpdateCurrentEntityName(newValue);
		}

	}
	return true;
}
