#include "Common.h"

#include "Editor/EditorGUI.h"
#include "Editor/EditorMgr.h"
#include "Editor/ValueEditors/PropertyEditorCreator.h"

#include "Core/Application.h"
#include "Core/Game.h"

#include "EntitySystem/EntityMgr/EntityMgr.h"

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

	/// Initialize menu
	{
		CEGUI::Window* menubar = gCEGUIWM.getWindow("EditorRoot/Menubar");

		/// Initialize component types in Add Component menu
		CEGUI::Window* addComponentMenu = gCEGUIWM.getWindow("EditorRoot/Menubar/Edit/NewComponent/AutoPopup");
		for (int i = 0; i < EntitySystem::NUM_COMPONENT_TYPES; ++i)
		{
			const string& componentName = EntitySystem::GetComponentTypeName((EntitySystem::eComponentType)i);
			CEGUI::Window* componentMenuItem = gCEGUIWM.createWindow("Editor/MenuItem", "EditorRoot/Menubar/Edit/NewComponent/Component" + StringConverter::ToString(i));
			componentMenuItem->setText(componentName);
			addComponentMenu->addChildWindow(componentMenuItem);
		}

		ConfigureMenu(menubar, true);
	}

	/// Initialize popup pseudomenu
	{
		CEGUI::Window* popupMenubar = gCEGUIWM.getWindow("EditorRoot/Popup");
		popupMenubar->hide();
	}

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

	/// Nasty hack to solve scrollbar issue
	{
		const CEGUI::UDim height = entityEditorPane->getHeight();
		entityEditorPane->setHeight(height + CEGUI::UDim(0, 1));
		entityEditorPane->setHeight(height);
	}
}

bool Editor::EditorGUI::OnComponentAddClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	CEGUI::PopupMenu* componentTypeMenu = static_cast<CEGUI::PopupMenu*>(gCEGUIWM.getWindow("EditorRoot/Popup/ComponentPopup/AutoPopup"));

	/// Fake MouseEvent to open popup menu
	CEGUI::MouseEventArgs me(args.window);

	me.position.d_x = CEGUI::CoordConverter::windowToScreenX(*me.window, 0);
	me.position.d_y = CEGUI::CoordConverter::windowToScreenY(*me.window, me.window->getHeight());

	GUISystem::ShowPopup(componentTypeMenu, me);
	return true;
}

bool Editor::EditorGUI::OnComponentRemoveClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	const CEGUI::String& componentIDString = args.window->getUserString("ComponentID");
	EntitySystem::ComponentID componentID = StringConverter::FromString<EntitySystem::ComponentID>(componentIDString.c_str());
	gEditorMgr.RemoveComponent(componentID);
	UpdateEntityEditorWindow();
	return true;
}

bool Editor::EditorGUI::OnComponentMenuOpened(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	ocWarning << "OnComponentMenuOpened: " << args.window->getName();
	return true;
}

bool Editor::EditorGUI::OnMouseEntersMenuItem(const CEGUI::EventArgs& )
{
	return true;
}

bool Editor::EditorGUI::OnMouseLeavesMenuItem(const CEGUI::EventArgs& )
{
	return true;
}

bool Editor::EditorGUI::OnMouseLeavesPopupMenuItem(const CEGUI::EventArgs& )
{
	return true;
}


/// Actions for all menu items are handled here.
bool Editor::EditorGUI::OnMenuItemClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	const CEGUI::String& itemName = args.window->getName();
	const string itemNameStr = itemName.c_str();

	/// ---- File menu ----
	if (itemName == "EditorRoot/Menubar/File")
	{
		return true;
	}
	
	if (itemName == "EditorRoot/Menubar/File/CreateProject")
	{
		return true;
	}

	if (itemName == "EditorRoot/Menubar/File/OpenProject")
	{
		return true;
	}

	if (itemName == "EditorRoot/Menubar/File/ExportGame")
	{
		return true;
	}

	if (itemName == "EditorRoot/Menubar/File/Quit")
	{
		gApp.Shutdown();
		return true;
	}

	/// ---- Scene menu ----
	if (itemName == "EditorRoot/Menubar/Scene")
	{
		return true;
	}

	if (itemName == "EditorRoot/Menubar/Scene/NewScene")
	{
		return true;
	}

	if (itemName == "EditorRoot/Menubar/Scene/OpenScene")
	{
		return true;
	}

	if (itemName == "EditorRoot/Menubar/Scene/SaveScene")
	{
		return true;
	}

	/// ---- Edit menu ----
	if (itemName == "EditorRoot/Menubar/Edit")
	{
		return true;
	}

	if (itemName == "EditorRoot/Menubar/Edit/NewEntity")
	{
		return true;
	}

	if (itemName == "EditorRoot/Menubar/Edit/NewComponent")
	{
		return true;
	}

	/// New component
	{
		string pattern = "EditorRoot/Menubar/Edit/NewComponent/Component";
		if (itemNameStr.substr(0, pattern.size()) == pattern)
		{
			OC_DASSERT(itemNameStr.size() > pattern.size());
			int componentType = StringConverter::FromString<int>(itemNameStr.substr(pattern.size()));
			gEditorMgr.AddComponent((EntitySystem::eComponentType)componentType);
			UpdateEntityEditorWindow();
			return true;
		}
	}

	if (itemName == "EditorRoot/Menubar/Edit/DuplicateEntity")
	{
		return true;
	}

	if (itemName == "EditorRoot/Menubar/Edit/DeleteEntity")
	{
		return true;
	}

	if (itemName == "EditorRoot/Menubar/Edit/CreatePrototype")
	{
		return true;
	}

	ocWarning << "MenuItem " << itemName << " clicked, but no action defined.";
	return true;
}

void Editor::EditorGUI::ConfigureMenu(CEGUI::Window* parent, bool isMenubar)
{
	size_t childCount = parent->getChildCount();
	for(size_t childIdx = 0; childIdx < childCount; childIdx++)
	{
		if (parent->getChildAtIdx(childIdx)->testClassName("MenuItem")
				|| parent->getChildAtIdx(childIdx)->testClassName("PopupMenu"))
		{
			parent->getChildAtIdx(childIdx)->subscribeEvent(CEGUI::MenuItem::EventMouseEnters,
					CEGUI::Event::Subscriber(&Editor::EditorGUI::OnMouseEntersMenuItem, this));

			parent->getChildAtIdx(childIdx)->subscribeEvent(CEGUI::MenuItem::EventMouseLeaves, isMenubar ?
					CEGUI::Event::Subscriber(&Editor::EditorGUI::OnMouseLeavesMenuItem, this) :
					CEGUI::Event::Subscriber(&Editor::EditorGUI::OnMouseLeavesPopupMenuItem, this));

			if(parent->getChildAtIdx(childIdx)->testClassName("MenuItem"))
			{
				parent->getChildAtIdx(childIdx)->subscribeEvent(CEGUI::MenuItem::EventClicked,
						CEGUI::Event::Subscriber(&Editor::EditorGUI::OnMenuItemClicked, this));
			}
		}
		ConfigureMenu(parent->getChildAtIdx(childIdx), isMenubar);
	}
}


