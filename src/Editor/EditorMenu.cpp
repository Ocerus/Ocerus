#include "Common.h"

#include "Editor/EditorMenu.h"
#include "Editor/EditorMgr.h"
#include "Core/Application.h"
#include "Core/Project.h"
#include "GUISystem/CEGUICommon.h"

using namespace Editor;

Editor::EditorMenu::EditorMenu()
{
}

EditorMenu::~EditorMenu()
{
}

void Editor::EditorMenu::Init()
{
	OC_CEGUI_TRY;
	{
		mTopMenu = static_cast<CEGUI::PopupMenu*>(gGUIMgr.GetWindow("Editor/TopMenu"));
		InitMenu();
		InitToolbar();
		mTopMenu->subscribeEvent(CEGUI::Window::EventDeactivated, CEGUI::Event::Subscriber(&Editor::EditorMenu::OnTopMenuDeactivated, this));
	}
	OC_CEGUI_CATCH_CRITICAL;
}

bool Editor::EditorMenu::OnTopMenuDeactivated(const CEGUI::EventArgs& e)
{
	OC_UNUSED(e);
	CEGUI::MenuItem* popupMenu = mTopMenu->getPopupMenuItem();
	if(popupMenu)
	{
		// This does not close sub-popup menus, only the current one
		popupMenu->closePopupMenu();
	}
	return true;
}

/// Actions for all menu items are handled here.
bool Editor::EditorMenu::OnMenuItemClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	CEGUI::Window* menuItem = args.window;
	CEGUI::Window* submenu = menuItem->getParent();

	if (submenu == mComponentsSubmenu)
	{
		// New component
		EntitySystem::eComponentType componentType = (EntitySystem::eComponentType)args.window->getID();
		gEditorMgr.AddComponentToSelectedEntity(componentType);
	}
	else if (submenu == mOpenSceneSubmenu)
	{
		// Open scene
		uint32 sceneIndex = args.window->getID();
		gEditorMgr.OpenSceneAtIndex(sceneIndex);
	}
	else if (submenu == mDeploySubmenu)
	{
		// Deploy project
		string platform = args.window->getText().c_str();
		gEditorMgr.ShowDeployProjectDialog(platform);
	}
	else
	{
		switch (menuItem->getID())
		{
		case MI_FILE_CREATEPROJECT:
			gEditorMgr.ShowCreateProjectDialog();
			break;
		case MI_FILE_OPENPROJECT:
			gEditorMgr.ShowOpenProjectDialog();
			break;
		case MI_FILE_CLOSEPROJECT:
			gEditorMgr.CloseProject();
			break;
		case MI_FILE_QUIT:
			gEditorMgr.ShowQuitDialog();
			break;
		case MI_SCENE_NEW:
			gEditorMgr.ShowNewSceneDialog();
			break;
		case MI_SCENE_SAVE:
			gEditorMgr.SaveOpenedScene();
			break;
		case MI_SCENE_CLOSE:
			gEditorMgr.CloseScene();
			break;
		case MI_EDIT_NEWENTITY:
			gEditorMgr.ShowCreateEntityPrompt();
			break;
		case MI_EDIT_DUPLICATEENTITY:
			gEditorMgr.DuplicateSelectedEntity();
			break;
		case MI_EDIT_DUPLICATESELECTEDENTITIES:
			gEditorMgr.DuplicateSelectedEntities();
			break;
		case MI_EDIT_DELETEENTITY:
			gEditorMgr.DeleteSelectedEntity();
			break;
		case MI_EDIT_DELETESELECTEDENTITIES:
			gEditorMgr.DeleteSelectedEntities();
			break;
		case MI_EDIT_CREATEPROTOTYPE:
			gEditorMgr.CreatePrototypeFromSelectedEntity();
			break;
		case MI_HELP_DESIGNDOCUMENTATION:
			gApp.OpenFileInExternalApp("docs/DesignDocumentation.pdf");
			break;
		case MI_HELP_EXTENDINGOCERUS:
			gApp.OpenFileInExternalApp("docs/ExtendingOcerus.pdf");
			break;
		case MI_HELP_SCRIPTREFERENCE:
			gApp.OpenFileInExternalApp("docs/ScriptReference.html");
			break;
		case MI_HELP_DOXYGEN:
			gApp.OpenFileInExternalApp("docs/Doxygen.html");
			break;
		case MI_HELP_SHORTCUTS:
			gApp.OpenFileInExternalApp("docs/Shortcuts.pdf");
			break;
		case MI_HELP_USERGUIDE:
			gApp.OpenFileInExternalApp("docs/UserGuide.pdf");
			break;
		case MI_HELP_ABOUT:
			GUISystem::ShowMessageBox(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup,
																   "about_message_text"));
			break;
		default:
			break;
		}
		
	}
	return true;
}

/// Actions for just-in-time generating submenu items are implemented here
bool EditorMenu::OnMenuItemHovered(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	CEGUI::MenuItem* menuItem = static_cast<CEGUI::MenuItem*>(args.window);

	if (!menuItem->getPopupMenu())
		return false;
	
	switch (menuItem->getID())
	{
		case MI_SCENE_OPEN:
			UpdateSceneMenu();
			break;
		default:
			break;
	}

	if (!menuItem->isOpened())
	{
		menuItem->openPopupMenu();
	}
	return true;
}

bool Editor::EditorMenu::OnPopupMenuClosed(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	CEGUI::PopupMenu* popupMenu = static_cast<CEGUI::PopupMenu*>(args.window);
	for (uint32 i = 0; i < popupMenu->getChildCount(); ++i)
	{
		CEGUI::MenuItem* menuItem = static_cast<CEGUI::MenuItem*>(popupMenu->getChildAtIdx(i));
		if (menuItem->getPopupMenu())
		{
			menuItem->closePopupMenu();
		}
	}
	return true;
}

bool Editor::EditorMenu::OnToolbarButtonStateChanged(const CEGUI::EventArgs& e)
{
	if (!gEditorMgr.IsProjectOpened()) return false;

	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	CEGUI::RadioButton* button = static_cast<CEGUI::RadioButton*>(args.window);
	if (!button->isSelected()) return false;
	CEGUI::RadioButton* radioButton = static_cast<CEGUI::RadioButton*>(args.window);
	
	if (radioButton->getGroupID() == TG_ACTION_TOOLS)
	{
		switch ((EditorMgr::eActionTool)radioButton->getID())
		{
		case EditorMgr::AT_RUN:
			gEditorMgr.ResumeAction();
			return true;
		case EditorMgr::AT_PAUSE:
			if (!gEditorMgr.IsActionRunning())
			{
				gEditorMgr.SwitchActionTool(Editor::EditorMgr::AT_RESTART);
				return true;
			}
			gEditorMgr.PauseAction();
			return true;
		case EditorMgr::AT_RESTART:
			gEditorMgr.RestartAction();
			return true;
		}
	}
	else if (radioButton->getGroupID() == TG_EDIT_TOOLS)
	{
		gEditorMgr.SetCurrentEditTool((EditorMgr::eEditTool)radioButton->getID());
		return true;
	}
	return true;
}

void EditorMenu::UpdateSceneMenu()
{
	OC_CEGUI_TRY;
	{
		Core::SceneInfoList scenes;
		gEditorMgr.GetCurrentProject()->GetSceneList(scenes);

		for (int32 i = mOpenSceneSubmenu->getChildCount() - 1; i >= 0; --i)
		{
			gGUIMgr.DestroyWindow(mOpenSceneSubmenu->getChildAtIdx(i));
		}

		for (Core::SceneInfoList::const_iterator it = scenes.begin(); it != scenes.end(); ++it)
		{
			int sceneID = it - scenes.begin();
			mOpenSceneSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Scene/OpenScene/" + StringConverter::ToString(sceneID), it->name + " (" + it->filename + ")", "", sceneID));
		}
	}
	OC_CEGUI_CATCH;
}

void EditorMenu::UpdateItemsEnabled()
{
	OC_CEGUI_TRY;
	{
		bool editMenuEnabled = gEditorMgr.GetCurrentProject()->IsSceneOpened();
		bool sceneMenuEnabled = gEditorMgr.GetCurrentProject()->IsProjectOpened();
		bool toolbarEnabled = editMenuEnabled;

		// Set enabled/disabled state to items in Edit menu
		CEGUI::Window* editMenu = mTopMenu->getItemFromIndex(2)->getChildAtIdx(0);
		for (size_t childIdx = 0, childCount = editMenu->getChildCount(); childIdx < childCount; childIdx++)
		{
			editMenu->getChildAtIdx(childIdx)->setEnabled(editMenuEnabled);
		}

		// Set enabled/disabled state to the components in the AddComponent submenu
		for (size_t childIdx = 0, childCount = mComponentsSubmenu->getChildCount(); childIdx < childCount; childIdx++)
		{
			mComponentsSubmenu->getChildAtIdx(childIdx)->setEnabled(editMenuEnabled);
		}

		// Set enabled/disabled state to the items in the Scene menu
		CEGUI::Window* sceneMenu = mTopMenu->getItemFromIndex(1)->getChildAtIdx(0);
		for (size_t childIdx = 0, childCount = sceneMenu->getChildCount(); childIdx < childCount; childIdx++)
		{
			sceneMenu->getChildAtIdx(childIdx)->setEnabled(sceneMenuEnabled);
		}

		// Set enabled/disabled state to the items in the toolbar
		for (RadioButtonList::iterator it=mActionButtons.begin(); it!=mActionButtons.end(); ++it)
		{
			(*it)->setEnabled(toolbarEnabled);
		}
		for (RadioButtonList::iterator it=mToolButtons.begin(); it!=mToolButtons.end(); ++it)
		{
			(*it)->setEnabled(toolbarEnabled);
		}
	}
	OC_CEGUI_CATCH;
}

void Editor::EditorMenu::SwitchToolButton( uint32 selectedButtonIndex )
{
	OC_ASSERT_MSG(selectedButtonIndex < mToolButtons.size(), "Invalid tool button index");
	if (!mToolButtons[selectedButtonIndex]->isDisabled())
	{
		mToolButtons[selectedButtonIndex]->setSelected(true);
	}
}

void Editor::EditorMenu::SwitchActionButton( uint32 selectedButtonIndex )
{
	OC_ASSERT_MSG(selectedButtonIndex < mActionButtons.size(), "Invalid tool button index");
	if (!mActionButtons[selectedButtonIndex]->isDisabled())
	{
		mActionButtons[selectedButtonIndex]->setSelected(true);
	}
}

void EditorMenu::InitMenu()
{
	// File submenu
	CEGUI::Window* itemFile = CreateMenuItem("Editor/TopMenu/File", TR("file"), "", MI_INVALID);
	mTopMenu->addChildWindow(itemFile);
	CEGUI::Window* fileSubmenu = CreatePopupMenu("Editor/TopMenu/File/AutoPopup");
	itemFile->addChildWindow(fileSubmenu);
	fileSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/File/CreateProject", TR("create_project"), TR("create_project_hint"), MI_FILE_CREATEPROJECT));
	fileSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/File/OpenProject", TR("open_project"), TR("open_project_hint"), MI_FILE_OPENPROJECT));
	fileSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/File/CloseProject", TR("close_project"), TR("close_project_hint"), MI_FILE_CLOSEPROJECT));
	CEGUI::Window* itemDeployProject = CreateMenuItem("Editor/TopMenu/File/DeployProject", TR("deploy_project"), TR("deploy_project_hint"), MI_INVALID, true);
	fileSubmenu->addChildWindow(itemDeployProject);
	mDeploySubmenu = CreatePopupMenu("Editor/TopMenu/File/DeployProject/AutoPopup");
	itemDeployProject->addChildWindow(mDeploySubmenu);
	vector<string> availablePlatforms;
	gApp.GetAvailableDeployPlatforms(availablePlatforms);
	if (availablePlatforms.empty())
	{
		bool enabled = false;
		mDeploySubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/File/DeployProject/Empty", TR("no_platform"), "", MI_INVALID, false, enabled));
	}
	else
	{
		for (vector<string>::iterator it=availablePlatforms.begin(); it!=availablePlatforms.end(); ++it)
		{
			mDeploySubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/File/DeployProject/" + *it, *it, "", MI_INVALID));
		}
	}
	fileSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/File/Quit", TR("quit"), TR("quit_hint"), MI_FILE_QUIT));

	// Scene submenu
	CEGUI::Window* itemScene = CreateMenuItem("Editor/TopMenu/Scene", TR("scene"), "", MI_INVALID);
	mTopMenu->addChildWindow(itemScene);
	CEGUI::Window* sceneSubmenu = CreatePopupMenu("Editor/TopMenu/Scene/AutoPopup");
	itemScene->addChildWindow(sceneSubmenu);
	sceneSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Scene/NewScene", TR("new_scene"), TR("new_scene_hint"), MI_SCENE_NEW));
	CEGUI::Window* itemOpenScene = CreateMenuItem("Editor/TopMenu/Scene/OpenScene", TR("open_scene") + "  ", TR("open_scene_hint"), MI_SCENE_OPEN, true);
	sceneSubmenu->addChildWindow(itemOpenScene);
	sceneSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Scene/SaveScene", TR("save_scene"), TR("save_scene_hint"), MI_SCENE_SAVE));
	sceneSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Scene/CloseScene", TR("close_scene"), TR("close_scene_hint"), MI_SCENE_CLOSE));

	// OpenScene submenu
	mOpenSceneSubmenu = CreatePopupMenu("Editor/TopMenu/Scene/OpenScene/AutoPopup");
	itemOpenScene->addChildWindow(mOpenSceneSubmenu);

	// Edit submenu
	CEGUI::Window* itemEdit = CreateMenuItem("Editor/TopMenu/Edit", TR("edit"), "", MI_INVALID);
	mTopMenu->addChildWindow(itemEdit);
	CEGUI::Window* editSubmenu = CreatePopupMenu("Editor/TopMenu/Edit/AutoPopup");
	itemEdit->addChildWindow(editSubmenu);
	editSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Edit/NewEntity", TR("new_entity"), TR("new_entity_hint"), MI_EDIT_NEWENTITY));
	CEGUI::Window* itemNewComponent = CreateMenuItem("Editor/TopMenu/Edit/NewComponent", TR("new_component"), TR("new_component_hint"), MI_INVALID, true);
	editSubmenu->addChildWindow(itemNewComponent);
	editSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Edit/DuplicateEntity", TR("duplicate_entity"), TR("duplicate_entity_hint"), MI_EDIT_DUPLICATEENTITY));
	editSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Edit/DuplicateSelectedEntities", TR("duplicate_selected_entities"), TR("duplicate_selected_entities_hint"), MI_EDIT_DUPLICATESELECTEDENTITIES));
	editSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Edit/DeleteEntity", TR("delete_entity"), TR("delete_entity_hint"), MI_EDIT_DELETEENTITY));
	editSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Edit/DeleteSelectedEntities", TR("delete_selected_entities"), TR("delete_selected_entities_hint"), MI_EDIT_DELETESELECTEDENTITIES));
	editSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Edit/CreatePrototype", TR("create_prototype"), TR("create_prototype_hint"), MI_EDIT_CREATEPROTOTYPE));

	// Components submenu
	mComponentsSubmenu = CreatePopupMenu("Editor/TopMenu/Edit/NewComponent/AutoPopup");
	itemNewComponent->addChildWindow(mComponentsSubmenu);
	for (size_t i = 0; i < EntitySystem::NUM_COMPONENT_TYPES; ++i)
	{
		const string& componentName = EntitySystem::GetComponentTypeName((EntitySystem::eComponentType)i);
		const CEGUI::String& componentMenuItemName = "Editor/TopMenu/Edit/NewComponent/Component" + Utils::StringConverter::ToString(i);
		mComponentsSubmenu->addChildWindow(CreateMenuItem(componentMenuItemName, componentName, "", i));
	}

	// Help submenu
	CEGUI::Window* itemHelp = CreateMenuItem("Editor/TopMenu/Help", TR("help"), "", MI_INVALID);
	mTopMenu->addChildWindow(itemHelp);
	CEGUI::Window* helpSubmenu = CreatePopupMenu("Editor/TopMenu/Help/AutoPopup");
	itemHelp->addChildWindow(helpSubmenu);
	helpSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Help/About", TR("about"), TR("about_hint"), MI_HELP_ABOUT));
	helpSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Help/UserGuide", TR("user_guide"), TR("user_guide_hint"), MI_HELP_USERGUIDE));
	helpSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Help/Shortcuts", TR("shortcuts"), TR("shortcuts"), MI_HELP_SHORTCUTS));
	helpSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Help/ScriptReference", TR("script_reference"), TR("script_reference_hint"), MI_HELP_SCRIPTREFERENCE));
	helpSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Help/ExtendingOcerus", TR("extending_ocerus"), TR("extending_ocerus_hint"), MI_HELP_EXTENDINGOCERUS));
	helpSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Help/DesignDocumentation", TR("design_documentation"), TR("design_documentation_hint"), MI_HELP_DESIGNDOCUMENTATION));
	helpSubmenu->addChildWindow(CreateMenuItem("Editor/TopMenu/Help/Doxygen", TR("doxygen"), TR("doxygen_hint"), MI_HELP_DOXYGEN));
}

void EditorMenu::InitToolbar()
{
	CEGUI::Window* toolbar = gGUIMgr.GetWindow("Editor/TopMenu/Toolbar");

	mActionButtons.push_back(CreateToolbarItem("Editor/TopMenu/Toolbar/ResumeAction", "btnResumeAction", TR("resume_action_hint"), EditorMgr::AT_RUN, TG_ACTION_TOOLS));
	mActionButtons.push_back(CreateToolbarItem("Editor/TopMenu/Toolbar/PauseAction", "btnPauseAction", TR("pause_action_hint"), EditorMgr::AT_PAUSE, TG_ACTION_TOOLS));
	mActionButtons.push_back(CreateToolbarItem("Editor/TopMenu/Toolbar/RestartAction", "btnRestartAction", TR("restart_action_hint"), EditorMgr::AT_RESTART, TG_ACTION_TOOLS));

	mToolButtons.push_back(CreateToolbarItem("Editor/TopMenu/Toolbar/EditToolMove", "btnEditToolMove", TR("edit_tool_move_hint"), EditorMgr::ET_MOVE, TG_EDIT_TOOLS));
	mToolButtons.push_back(CreateToolbarItem("Editor/TopMenu/Toolbar/EditToolRotateZ", "btnEditToolRotate", TR("edit_tool_rotate_hint"), EditorMgr::ET_ROTATE, TG_EDIT_TOOLS));
	mToolButtons.push_back(CreateToolbarItem("Editor/TopMenu/Toolbar/EditToolRotateY", "btnEditToolRotateZ", TR("edit_tool_rotate_y_hint"), EditorMgr::ET_ROTATE_Y, TG_EDIT_TOOLS));
	mToolButtons.push_back(CreateToolbarItem("Editor/TopMenu/Toolbar/EditToolScale", "btnEditToolScale", TR("edit_tool_scale_hint"), EditorMgr::ET_SCALE, TG_EDIT_TOOLS));

	for (RadioButtonList::const_iterator it = mActionButtons.begin(); it != mActionButtons.end(); ++it)
		toolbar->addChildWindow(*it);

	for (RadioButtonList::const_iterator it = mToolButtons.begin(); it != mToolButtons.end(); ++it)
		toolbar->addChildWindow(*it);

	SwitchActionButton(EditorMgr::AT_RESTART);
	SwitchToolButton(EditorMgr::ET_MOVE);
}

CEGUI::Window* Editor::EditorMenu::CreateMenuItem(const CEGUI::String& name, const CEGUI::String& text, const CEGUI::String& tooltip, size_t tag, bool autoOpenSubmenu, bool enabled)
{
	CEGUI::Window* menuItem = gGUIMgr.CreateWindow("Editor/MenuItem", name);
	menuItem->setID((CEGUI::uint)tag);
	menuItem->setText(text);
	menuItem->setTooltipText(tooltip);
	if (!enabled) menuItem->disable();
	menuItem->subscribeEvent(CEGUI::MenuItem::EventClicked, CEGUI::Event::Subscriber(&Editor::EditorMenu::OnMenuItemClicked, this));
	if (autoOpenSubmenu)
	{
		// When hovered, automatically open submenu
		menuItem->subscribeEvent(CEGUI::MenuItem::EventMouseEnters, CEGUI::Event::Subscriber(&Editor::EditorMenu::OnMenuItemHovered, this));
	}
	return menuItem;
}

CEGUI::Window* Editor::EditorMenu::CreatePopupMenu(const CEGUI::String& name)
{
	CEGUI::PopupMenu* popupMenu = static_cast<CEGUI::PopupMenu*>(gGUIMgr.CreateWindow("Editor/PopupMenu", name));
	popupMenu->setVisible(false);
	popupMenu->setFadeInTime((float)0.1);
	popupMenu->setFadeOutTime((float)0.1);
	popupMenu->setItemSpacing(2);
	popupMenu->setAutoResizeEnabled(true);
	popupMenu->subscribeEvent(CEGUI::PopupMenu::EventHidden,
		CEGUI::Event::Subscriber(&Editor::EditorMenu::OnPopupMenuClosed, this));
	return popupMenu;
}

CEGUI::RadioButton* Editor::EditorMenu::CreateToolbarItem(const CEGUI::String& name, const CEGUI::String& imageName, const CEGUI::String& tooltip, size_t tag, size_t groupID)
{
	static uint32 left = 0;
	CEGUI::RadioButton* toolbarItem = static_cast<CEGUI::RadioButton*>(gGUIMgr.CreateWindow("Editor/ToolbarRadioButton", name));
	toolbarItem->setID((CEGUI::uint)tag);
	toolbarItem->setGroupID(groupID);
	toolbarItem->setProperty("NormalImage", "set:EditorToolbar image:" + imageName + "Normal");
	toolbarItem->setProperty("HoverImage", "set:EditorToolbar image:" + imageName + "Hover");
	toolbarItem->setProperty("PushedImage", "set:EditorToolbar image:" + imageName + "Pushed");
	toolbarItem->setTooltipText(tooltip);
	toolbarItem->setPosition(CEGUI::UVector2(cegui_absdim((float)left), cegui_absdim(0)));
	toolbarItem->setSize(CEGUI::UVector2(cegui_absdim(16), cegui_reldim(1)));
	toolbarItem->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&Editor::EditorMenu::OnToolbarButtonStateChanged, this));
	left += 20;
	return toolbarItem;
}