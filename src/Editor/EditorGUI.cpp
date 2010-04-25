#include "Common.h"

#include "Editor/EditorGUI.h"
#include "Editor/EditorMgr.h"
#include "Editor/EditorMenu.h"
#include "Editor/LayerMgrWidget.h"
#include "Editor/ResourceWindow.h"
#include "Editor/ValueEditors/PropertyEditorCreator.h"

#include "Core/Game.h"

#include "EntitySystem/EntityMgr/EntityMgr.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"

#include "GUISystem/GUIMgr.h"
#include "GUISystem/CEGUITools.h"
#include "GUISystem/VerticalLayout.h"
#include "GUISystem/ViewportWindow.h"


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
	mGameViewport(0),
	mEditorViewport(0),
	mLayerMgrWidget(0),
	mResourceWindow(0)
{
}

EditorGUI::~EditorGUI()
{
	delete mLayerMgrWidget;
	delete mEditorMenu;
	delete mResourceWindow;
}

void EditorGUI::LoadGUI()
{
	CEGUI_EXCEPTION_BEGIN

	// Load Imagesets for Editor.
	CEGUI::ImagesetManager::getSingleton().create("EditorToolbar.imageset");

	CEGUI_EXCEPTION_END
	
	/// @todo This code is quite ugly and will be changed
	if (!gGUIMgr.LoadRootLayout("Editor.layout"))
	{
		ocError << "Can't load editor";
		return;
	}
	CEGUI::System::getSingleton().setDefaultTooltip("Editor/Tooltip");
	mPropertyItemHeight = (int32)gCEGUIWM.getWindow("EditorRoot")->getFont(true)->getLineSpacing(1.1f) + 10;
	mComponentGroupHeight = 28;
	gCEGUIWM.getWindow("EditorRoot")->setMousePassThroughEnabled(true);

	/// Initialize EditorMenu
	mEditorMenu = new EditorMenu();
	mEditorMenu->Init();

	/// Initialize LayerMgrWidget
	mLayerMgrWidget = new LayerMgrWidget(gCEGUIWM.getWindow("EditorRoot/LayerMgr"));
	mLayerMgrWidget->Init();

	/// Initialize top viewport
	{
		/// Create game camera.
		EntitySystem::EntityDescription desc;
		desc.SetName("GameCamera1");
		desc.AddComponent(EntitySystem::CT_Camera);
		desc.SetTransient(true);
		EntitySystem::EntityHandle camera = gEntityMgr.CreateEntity(desc);
		camera.FinishInit();

		/// Assign game camera to top viewport.
		mGameViewport = static_cast<GUISystem::ViewportWindow*>(gCEGUIWM.getWindow("EditorRoot/TopViewport"));
		mGameViewport->SetCamera(camera);
		mGameViewport->SetMovableContent(false);

		/// Pass render target from viewport to Game instance.
		GlobalProperties::Get<Core::Game>("Game").SetRenderTarget(mGameViewport->GetRenderTarget());
	}

	/// Initialize bottom viewport
	{
		/// Create editor camera.
		EntitySystem::EntityDescription desc;
		desc.SetName("EditorCamera1");
		desc.AddComponent(EntitySystem::CT_Camera);
		desc.SetTransient(true);
		EntitySystem::EntityHandle camera = gEntityMgr.CreateEntity(desc);
		camera.FinishInit();

		/// Assign game camera to bottom viewport.
		mEditorViewport = static_cast<GUISystem::ViewportWindow*>(gCEGUIWM.getWindow("EditorRoot/BottomViewport"));
		mEditorViewport->SetCamera(camera);
		mEditorViewport->SetMovableContent(true);
	}

	/// Initialize resource window
	mResourceWindow = new ResourceWindow();
	mResourceWindow->Init();
}

void EditorGUI::Update(float32 delta)
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
	mLayerMgrWidget->Update(delta);
}

void EditorGUI::Draw(float32 delta)
{
	OC_UNUSED(delta);	// It may be handy one day
}


void EditorGUI::UpdateEntityEditorWindow()
{
	PROFILE_FNC();

	EntitySystem::EntityHandle currentEntity = gEditorMgr.GetCurrentEntity();

	CEGUI::ScrollablePane* entityEditorPane = static_cast<CEGUI::ScrollablePane*>(gCEGUIWM.getWindow(ENTITY_EDITOR_NAME));
	float savedScrollPosition = entityEditorPane->getVerticalScrollPosition() * entityEditorPane->getContentPaneArea().getHeight();
	OC_ASSERT(entityEditorPane);

	{
		const CEGUI::Window* entityEditorContentPane = entityEditorPane->getContentPane();
		OC_ASSERT(entityEditorContentPane);

		// Set layout on Entity Editor
		if (!mEntityEditorLayout)
			mEntityEditorLayout = new GUISystem::VerticalLayout(entityEditorPane, entityEditorContentPane);

		// Clear all the content of Entity Editor.
		size_t childCount = entityEditorContentPane->getChildCount();
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

	mEntityEditorLayout->LockUpdates();

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

		CEGUI::GroupBox* componentGroup = static_cast<CEGUI::GroupBox*>(gCEGUIWM.createWindow("Editor/GroupBox", namePrefix));
		componentGroup->setText(componentName);
		mEntityEditorLayout->AddChildWindow(componentGroup);


		CEGUI::PushButton* removeComponentButton = static_cast<CEGUI::PushButton*>(gCEGUIWM.createWindow("Editor/Button", namePrefix + "/RemoveButton"));
		removeComponentButton->setArea(CEGUI::URect(CEGUI::UDim(1, -8), CEGUI::UDim(0, -24), CEGUI::UDim(1, 12), CEGUI::UDim(0, -4)));
		removeComponentButton->setText("x");
		removeComponentButton->setClippedByParent(false);

		if (gEntityMgr.CanDestroyEntityComponent(currentEntity, componentID))
		{
			removeComponentButton->setTooltipText("Remove the component.");
			removeComponentButton->setUserString("ComponentID", StringConverter::ToString(componentID));
			removeComponentButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::EditorGUI::OnComponentRemoveClicked, this));
		}
		else
		{
			removeComponentButton->setTooltipText("This component cannot be removed.");
			removeComponentButton->setEnabled(false);
		}

		componentGroup->addChildWindow(removeComponentButton);

		GUISystem::VerticalLayout* layout = new GUISystem::VerticalLayout(componentGroup, componentGroup->getContentPane(), true);
		
		for (PropertyList::iterator it = propertyList.begin(); it != propertyList.end(); ++it)
		{
			AbstractValueEditor* editor = CreatePropertyEditor(*it);
			mPropertyEditors.push_back(editor);
			layout->AddChildWindow(editor->CreateWidget(namePrefix + "/" + it->GetKey().ToString() + "Editor"));
		}
		layout->UpdateLayout();
	}

	mEntityEditorLayout->UnlockUpdates();
	mEntityEditorLayout->UpdateLayout();

	entityEditorPane->setVerticalScrollPosition(savedScrollPosition / entityEditorPane->getContentPaneArea().getHeight());
	
	/// Nasty hack to solve scrollbar issue
	{
		const CEGUI::UDim height = entityEditorPane->getHeight();
		entityEditorPane->setHeight(height + CEGUI::UDim(0, 1));
		entityEditorPane->setHeight(height);
	}
}

bool Editor::EditorGUI::OnComponentRemoveClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	const CEGUI::String& componentIDString = args.window->getUserString("ComponentID");
	EntitySystem::ComponentID componentID = StringConverter::FromString<EntitySystem::ComponentID>(componentIDString.c_str());
	gEditorMgr.RemoveComponent(componentID);
	return true;
}
