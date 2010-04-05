#include "Common.h"
#include "EditorGUI.h"
#include "EditorMgr.h"
#include "ValueEditors/PropertyEditorCreator.h"

#include "GUISystem/GUIMgr.h"
#include "EntitySystem/EntityMgr/EntityMgr.h"
#include "Core/Game.h"

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
	mTopViewport(0),
	mBottomViewport(0)
{
}

EditorGUI::~EditorGUI()
{
}

void EditorGUI::LoadGUI()
{
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
	
	/// Initialize top viewport
	{
		/// Create game camera.
		EntitySystem::EntityDescription desc;
		desc.SetName("GameCamera1");
		desc.AddComponent(EntitySystem::CT_Camera);
		EntitySystem::EntityHandle camera = gEntityMgr.CreateEntity(desc);
		camera.FinishInit();

		/// Assign game camera to top viewport.
		mTopViewport = static_cast<GUISystem::ViewportWindow*>(gCEGUIWM.getWindow("EditorRoot/TopViewport"));
		mTopViewport->SetCamera(camera);
		mTopViewport->SetMovableContent(true);

		/// Pass render target from viewport to Game instance.
		GlobalProperties::Get<Core::Game>("Game").SetRenderTarget(mTopViewport->GetRenderTarget());
	}

	/// Initialize bottom viewport
	{
		/// Create editor camera.
		EntitySystem::EntityDescription desc;
		desc.SetName("EditorCamera1");
		desc.AddComponent(EntitySystem::CT_Camera);
		EntitySystem::EntityHandle camera = gEntityMgr.CreateEntity(desc);
		camera.FinishInit();

		/// Assign game camera to bottom viewport.
		mBottomViewport = static_cast<GUISystem::ViewportWindow*>(gCEGUIWM.getWindow("EditorRoot/BottomViewport"));
		mBottomViewport->SetCamera(camera);
		mBottomViewport->SetMovableContent(true);
	}
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
}

void EditorGUI::Draw(float32 delta)
{
	OC_UNUSED(delta);	// It may be handy one day
	
	/// Render bottom viewport (top viewport is rendered by Game object)
	gGfxRenderer.SetCurrentRenderTarget(mBottomViewport->GetRenderTarget());
	gGfxRenderer.ClearCurrentRenderTarget(GfxSystem::Color(0, 0, 0));
	gGfxRenderer.DrawEntities();
	
/*
	// draw the multi-selection stuff
	if (mSelectionStarted)
	{
		MouseState& mouse = gInputMgr.GetMouseState();
		Vector2 worldCursorPos;
		if (gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mouse.x, mouse.y), worldCursorPos, mRenderTarget))
		{
			float32 minDistance = SELECTION_MIN_DISTANCE / gGfxRenderer.GetRenderTargetCameraZoom(mRenderTarget);
			if ((worldCursorPos-mSelectionCursorPosition).LengthSquared() >= MathUtils::Sqr(minDistance))
			{
				float32 rotation = gGfxRenderer.GetRenderTargetCameraRotation(mRenderTarget);
				gGfxRenderer.DrawRect(mSelectionCursorPosition, worldCursorPos, rotation, GfxSystem::Color(255,255,0,150), false);
			}
		}
	}
*/
	gGfxRenderer.FinalizeRenderTarget();
}


void EditorGUI::UpdateEntityEditorWindow()
{
	PROFILE_FNC();

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