#include "Common.h"
#include "EditorGUI.h"
#include "EditorMgr.h"
#include "ValueEditors/PropertyEditorCreator.h"

#include "GUISystem/GUIMgr.h"
#include "EntitySystem/EntityMgr/EntityMgr.h"

#include "CEGUI.h"
#include "GUISystem/VerticalLayout.h"
#include "GUISystem/ViewportWindow.h"

#define gCEGUIWM CEGUI::WindowManager::getSingleton()

namespace Editor
{
	const string ENTITY_EDITOR_NAME = "EditorRoot/EntityEditor/Scrollable";
}


using namespace Editor;

EditorGUI::EditorGUI():
	mPropertyItemHeight(0),
	mComponentGroupHeight(0),
	mPropertyUpdateTimer(0),
	mEntityEditorLayout(0),
	mViewport(0)
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

	mViewport = static_cast<GUISystem::ViewportWindow*>(gCEGUIWM.createWindow("Editor/Viewport", "EditorRoot/Viewport"));
	mViewport->setArea(CEGUI::URect(CEGUI::UDim(0.3f, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(0.5f, 0)));
	gCEGUIWM.getWindow("EditorRoot")->addChildWindow(mViewport);

	mViewport->SetCamera(gEntityMgr.FindFirstEntity("Camera2"));

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

	
	{
		const CEGUI::Window* entityEditorContentPane = entityEditorPane->getContentPane();
		OC_ASSERT(entityEditorContentPane);

		// Set layout on Entity Editor
		if (!mEntityEditorLayout)
			mEntityEditorLayout = new GUISystem::VerticalLayout(entityEditorPane, entityEditorContentPane);
		
		// Clear all the content of Entity Editor.
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

	// First "component" is general entity info
	{
		const string namePrefix = ENTITY_EDITOR_NAME + "/ComponentGeneralInfo";
		CEGUI::GroupBox* componentGroup = static_cast<CEGUI::GroupBox*>(gCEGUIWM.createWindow("Editor/GroupBox", namePrefix));
		componentGroup->setText("General Info");
		mEntityEditorLayout->AddChildWindow(componentGroup);
		GUISystem::VerticalLayout* layout = new GUISystem::VerticalLayout(componentGroup, componentGroup->getContentPane(), true);

		{
			AbstractValueEditor* editor = CreateEntityIDEditor(currentEntity);
			mPropertyEditors.push_back(editor);
			layout->AddChildWindow(editor->CreateWidget(namePrefix + "/EntityIdEditor"));
		}
		{
			AbstractValueEditor* editor = CreateEntityNameEditor(currentEntity);
			mPropertyEditors.push_back(editor);
			layout->AddChildWindow(editor->CreateWidget(namePrefix + "/EntityNameEditor"));
		}

		layout->UpdateLayout();
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

		CEGUI::GroupBox* componentGroup = static_cast<CEGUI::GroupBox*>(gCEGUIWM.createWindow("Editor/GroupBox", namePrefix));
		componentGroup->setText(componentName);
		mEntityEditorLayout->AddChildWindow(componentGroup);
		GUISystem::VerticalLayout* layout = new GUISystem::VerticalLayout(componentGroup, componentGroup->getContentPane(), true);
		
		for (PropertyList::iterator it = propertyList.begin(); it != propertyList.end(); ++it)
		{
			AbstractValueEditor* editor = CreatePropertyEditor(*it);
			mPropertyEditors.push_back(editor);
			layout->AddChildWindow(editor->CreateWidget(namePrefix + "/" + it->GetKey().ToString() + "Editor"));
		}
		layout->UpdateLayout();
	}
	mEntityEditorLayout->UpdateLayout();
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
