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
	mComponentGroupHeight(0),
	mPropertyUpdateTimer(0)
{
}

EditorGUI::~EditorGUI()
{
}

void EditorGUI::LoadGUI()
{
	/// @todo This code is quite ugly and will be changed
	gGUIMgr.LoadRootLayout("Editor.layout");
	CEGUI::System::getSingleton().setDefaultTooltip("Editor/Tooltip");
	mPropertyItemHeight = (int32)gCEGUIWM.getWindow("EditorRoot")->getFont(true)->getLineSpacing(1.1f) + 10;
	mComponentGroupHeight = 28;
	gCEGUIWM.getWindow("EditorRoot")->setMousePassThroughEnabled(true);
	


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

void EditorGUI::Update(const float32 delta)
{
	mPropertyUpdateTimer += delta;
	if (mPropertyUpdateTimer > 0.3)
	{
		mPropertyUpdateTimer = 0;
		for (PropertyEditors::const_iterator it = mPropertyEditors.begin(); it != mPropertyEditors.end(); ++it)
		{
			(*it)->Update();
		}
	}
}

void EditorGUI::UpdateEntityEditorWindow()
{
	EntitySystem::EntityHandle currentEntity = gEditorMgr.GetCurrentEntity();

	CEGUI::ScrollablePane* entityEditorPane = static_cast<CEGUI::ScrollablePane*>(gCEGUIWM.getWindow(ENTITY_EDITOR_NAME));
	OC_ASSERT(entityEditorPane);

	// Clear all the content of Entity Editor.
	{
		const CEGUI::Window* entityEditorContentPane = entityEditorPane->getContentPane();
		OC_ASSERT(entityEditorContentPane);

		int childCount = entityEditorContentPane->getChildCount();
		for (int i = (childCount - 1); i >= 0; --i)
		{
			gCEGUIWM.destroyWindow(entityEditorContentPane->getChildAtIdx(i));
		}
	}

	// Clear all property editors.
	for (PropertyEditors::const_iterator it = mPropertyEditors.begin(); it != mPropertyEditors.end(); ++it)
	{
		delete (*it);
	}
	mPropertyEditors.clear();

	// There is no entity to be selected.
	if (!currentEntity.IsValid()) return;

	int newGroupY = 0; // The y coord for new group

	// First "component" is general entity info
	{
		const string namePrefix = ENTITY_EDITOR_NAME + "/ComponentGeneralInfo";
		CEGUI::Window* componentGroup = gCEGUIWM.createWindow("Editor/GroupBox", namePrefix);
		componentGroup->setText("General Info");
		entityEditorPane->addChildWindow(componentGroup);

		int32 currentY = 0;
		CreateValueEditorWidgets(CreateEntityIDEditor(currentEntity), componentGroup, currentY);
		CreateValueEditorWidgets(CreateEntityNameEditor(currentEntity), componentGroup, currentY);
		currentY += ENTITY_EDITOR_GROUP_HEIGHT;
		componentGroup->setArea(CEGUI::URect(CEGUI::UDim(0, 4), CEGUI::UDim(0, (float)newGroupY + 1), CEGUI::UDim(1, -4), CEGUI::UDim(0, (float)newGroupY + currentY)));
		newGroupY += currentY;
	}

	int32 componentCount = EntitySystem::EntityMgr::GetSingleton().GetNumberOfEntityComponents(currentEntity);

	for (int componentID = 0; componentID < componentCount; ++componentID)
	{
		const string namePrefix = ENTITY_EDITOR_NAME + "/Component" + StringConverter::ToString(componentID);
		const string componentName = EntitySystem::GetComponentTypeName(gEntityMgr.GetEntityComponentType(currentEntity, componentID));

		PropertyList propertyList;
		gEntityMgr.GetEntityComponentProperties(currentEntity, componentID, propertyList, Reflection::PA_EDIT_READ);

		// Components with no readable properties are not shown.
		if (propertyList.size() == 0)
			continue;

		CEGUI::Window* componentGroup = gCEGUIWM.createWindow("Editor/GroupBox", namePrefix);
		componentGroup->setText(componentName);
		entityEditorPane->addChildWindow(componentGroup);

		int currentY = 0;

		for (PropertyList::iterator it = propertyList.begin(); it != propertyList.end(); ++it)
		{
			CreateValueEditorWidgets(CreatePropertyEditor(*it), componentGroup, currentY);
		}
		currentY += ENTITY_EDITOR_GROUP_HEIGHT;

		componentGroup->setArea(CEGUI::URect(CEGUI::UDim(0, 4), CEGUI::UDim(0, (float)newGroupY + 1), CEGUI::UDim(1, -4), CEGUI::UDim(0, (float)newGroupY + currentY)));
		newGroupY += currentY;
	}
}


/// TO BE REMOVED
bool EditorGUI::EntityPickerHandler (const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	string entityName = gCEGUIWM.getWindow("picker/edit")->getText().c_str();
	EntitySystem::EntityHandle entity = gEntityMgr.FindFirstEntity(entityName);
	EditorMgr::GetSingleton().SetCurrentEntity(entity);
	return true;
}


void EditorGUI::CreateValueEditorWidgets(IValueEditor* editor, CEGUI::Window* componentGroup, int32& currentY)
{
	mPropertyEditors.push_back(editor);
	CEGUI::Window* labelWidget = editor->CreateLabelWidget(componentGroup);
	CEGUI::Window* editorWidget = editor->CreateEditorWidget(componentGroup);
	labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 2), CEGUI::UDim(0, (float)(currentY + 1)), CEGUI::UDim(0.5, -1), CEGUI::UDim(0, (float32)(currentY + editor->GetEditorHeightHint()))));
	editorWidget->setArea(CEGUI::URect(CEGUI::UDim(0.5, 1), CEGUI::UDim(0, (float)(currentY + 1)), CEGUI::UDim(1, -1), CEGUI::UDim(0, (float32)(currentY + editor->GetEditorHeightHint()))));
	currentY += editor->GetEditorHeightHint() + 2;
}
