#include "Common.h"
#include "Editor/EditorMenu.h"
#include "Editor/EditorMgr.h"
#include "GUISystem/CEGUITools.h"
#include "Core/Application.h"
#include "Core/Project.h"
#include "Core/Game.h"

#include "GUISystem/MessageBox.h"
#include "GUISystem/FolderSelector.h"


using namespace Editor;

namespace Editor
{
	const string menubarPrefix = "EditorRoot/Menubar";
	const string toolbarPrefix = "EditorRoot/Toolbar";
}

Editor::EditorMenu::EditorMenu()
{
}

void Editor::EditorMenu::Init()
{
	CEGUI_EXCEPTION_BEGIN
	CEGUI::Window* menubar = gCEGUIWM.getWindow(menubarPrefix);
	InitComponentMenu();
	ConfigureMenu(menubar);
	UpdateSceneMenu();
	menubar->subscribeEvent(CEGUI::Window::EventDeactivated, CEGUI::Event::Subscriber(&Editor::EditorMenu::OnMouseLeavesMenuItem, this));

	CEGUI::Window* toolbar = gCEGUIWM.getWindow(toolbarPrefix);
	ConfigureToolbar(toolbar);
	CEGUI_EXCEPTION_END_CRITICAL
}

bool Editor::EditorMenu::OnMouseEntersMenuItem(const CEGUI::EventArgs& )
{
	return true;
}

bool Editor::EditorMenu::OnMouseLeavesMenuItem(const CEGUI::EventArgs& e)
{
	// Close a menu
	const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::WindowEventArgs&>(e);
	
	CEGUI::Window* menubar = we.window;
	while(menubar)
	{
		if( menubar->testClassName("Menubar") )
		{
			// We found the root; a menu bar
			CEGUI::MenuItem* popupMenu = static_cast<CEGUI::Menubar*>(menubar)->getPopupMenuItem();
			if(popupMenu)
			{
				// This does not close sub-popup menus, only the current one
				popupMenu->closePopupMenu();
			}
			break;
		}
		menubar = menubar->getParent();
	}
	return true;
}

/// Actions for all menu items are handled here.
bool Editor::EditorMenu::OnMenuItemClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	const CEGUI::String& itemName = args.window->getName();
	const string itemNameStr = itemName.c_str();

	/// ---- File menu ----
	if (itemName == menubarPrefix + "/File")
	{
		return true;
	}

	if (itemName == menubarPrefix + "/File/CreateProject")
	{
		gEditorMgr.ShowCreateProjectDialog();
		return true;
	}

	if (itemName == menubarPrefix + "/File/OpenProject")
	{
		gEditorMgr.ShowOpenProjectDialog();
		return true;
	}

	if (itemName == menubarPrefix + "/File/DeployProject")
	{
		return true;
	}

	if (itemName == menubarPrefix + "/File/CloseProject")
	{
		gEditorMgr.CloseProject();
		return true;
	}

	if (itemName == menubarPrefix + "/File/Quit")
	{
		gEditorMgr.ShowQuitDialog();
		return true;
	}

	/// ---- Scene menu ----
	if (itemName == menubarPrefix + "/Scene")
	{
		return true;
	}

	if (itemName == menubarPrefix + "/Scene/NewScene")
	{
		gEditorMgr.CreateScene();
		return true;
	}

	if (itemName == menubarPrefix + "/Scene/OpenScene")
	{
		return true;
	}

	/// Open scene
	{
		string pattern = menubarPrefix + "/Scene/OpenScene/Scene";
		if (itemNameStr.substr(0, pattern.size()) == pattern)
		{
			uint32 sceneIndex = args.window->getID();
			gEditorMgr.Reset();
			gEditorMgr.GetCurrentProject()->OpenSceneAtIndex(sceneIndex);
			return true;
		}
	}

	if (itemName == menubarPrefix + "/Scene/SaveScene")
	{
		gEditorMgr.SaveOpenedScene();
		return true;			
	}

	if (itemName == menubarPrefix + "/Scene/CloseScene")
	{
		if (gEditorMgr.GetCurrentProject()->IsSceneOpened())
		{
			gEditorMgr.GetCurrentProject()->CloseOpenedScene();
		}
		return true;			
	}

	/// ---- Edit menu ----
	if (itemName == menubarPrefix + "/Edit")
	{
		return true;
	}

	if (itemName == menubarPrefix + "/Edit/NewEntity")
	{
		gEditorMgr.CreateEntity();
		return true;
	}

	if (itemName == menubarPrefix + "/Edit/NewComponent")
	{
		return true;
	}

	// New component
	{
		string pattern = menubarPrefix + "/Edit/NewComponent/Component";
		if (itemNameStr.substr(0, pattern.size()) == pattern)
		{
			OC_DASSERT(itemNameStr.size() > pattern.size());
			int componentType = StringConverter::FromString<int>(itemNameStr.substr(pattern.size()));
			gEditorMgr.AddComponent((EntitySystem::eComponentType)componentType);
			return true;
		}
	}

	if (itemName == menubarPrefix + "/Edit/DuplicateEntity")
	{
		gEditorMgr.DuplicateCurrentEntity();
		return true;
	}
	
	if (itemName == menubarPrefix + "/Edit/DuplicateSelectedEntities")
	{
		gEditorMgr.DuplicateSelectedEntities();
		return true;
	}

	if (itemName == menubarPrefix + "/Edit/DeleteEntity")
	{
		gEditorMgr.DeleteCurrentEntity();
		return true;
	}

	if (itemName == menubarPrefix + "/Edit/DeleteSelectedEntities")
	{
		gEditorMgr.DeleteSelectedEntities();
		return true;
	}

	if (itemName == menubarPrefix + "/Edit/CreatePrototype")
	{
		gEditorMgr.CreatePrototypeFromCurrentEntity();
		return true;
	}

	/// ---- Help ----
	if (itemName == menubarPrefix + "/Help")
	{
		return true;
	}

	if (itemName == menubarPrefix + "/Help/UserDocumentation")
	{
		gApp.OpenPDF("data/docs/Documentation.pdf");
		return true;
	}

	if (itemName == menubarPrefix + "/Help/Shortcuts")
	{
		gApp.OpenPDF("data/docs/Shortcuts.pdf");
		return true;
	}

	if (itemName == menubarPrefix + "/Help/About")
	{
		GUISystem::ShowMessageBox(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "about_message_text"));
		return true;
	}


	ocWarning << "MenuItem " << itemName << " clicked, but no action defined.";
	return true;
}

bool Editor::EditorMenu::OnToolbarButtonStateChanged(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	const CEGUI::String& buttonName = args.window->getName();
	CEGUI::RadioButton* button = static_cast<CEGUI::RadioButton*>(args.window);

	if (!button->isSelected()) return false;

	ocDebug << "Toolbar button " << buttonName << " clicked.";

	/// ---- Resume action ----
	if (buttonName == toolbarPrefix + "/ResumeAction")
	{
		gEditorMgr.ResumeAction();
		return true;
	}

	/// ---- Pause action ----
	if (buttonName == toolbarPrefix + "/PauseAction")
	{
		if (!GlobalProperties::Get<Core::Game>("Game").IsActionRunning())
		{
			gEditorMgr.SwitchActionTool(Editor::EditorMgr::AT_RESTART);
			return true;
		}

		gEditorMgr.PauseAction();
		return true;
	}

	/// ---- Restart action ----
	if (buttonName == toolbarPrefix + "/RestartAction")
	{
		gEditorMgr.RestartAction();
		return true;
	}

	/// ---- Edit tool move ----
	if (buttonName == toolbarPrefix + "/EditToolMove")
	{
		gEditorMgr.SetCurrentEditTool(EditorMgr::ET_MOVE);
		return true;
	}

	/// ---- Edit tool rotate ----
	if (buttonName == toolbarPrefix + "/EditToolRotateZ")
	{
		gEditorMgr.SetCurrentEditTool(EditorMgr::ET_ROTATE);
		return true;
	}

	/// ---- Edit tool rotate-y ----
	if (buttonName == toolbarPrefix + "/EditToolRotateY")
	{
		gEditorMgr.SetCurrentEditTool(EditorMgr::ET_ROTATE_Y);
		return true;
	}

	/// ---- Edit tool scale ----
	if (buttonName == toolbarPrefix + "/EditToolScale")
	{
		gEditorMgr.SetCurrentEditTool(EditorMgr::ET_SCALE);
		return true;
	}

	ocWarning << "Toolbar button " << buttonName << " clicked, but no action defined.";
	return true;
}

void Editor::EditorMenu::OnMessageBoxClicked(GUISystem::MessageBox::eMessageBoxButton button, int32 t)
{
	eMessageBoxTags tag = (eMessageBoxTags)t;
	switch(tag)
	{
	case MBT_QUIT:
		if (button == GUISystem::MessageBox::MBB_YES)
			gApp.Shutdown();
		return;
	}
	ocWarning << "MessageBox with tag " << tag << " clicked, but no action defined.";
}

void Editor::EditorMenu::OnFolderSelected(const string& path, const string& editboxValue, bool canceled, int32 t)
{
	if (canceled) return;
	eFolderSelectorTags tag = (eFolderSelectorTags)t;
	switch(tag)
	{
	case FST_CREATEPROJECT:
		gEditorMgr.Reset();
		gEditorMgr.CreateProject(path + '/' + editboxValue);
		return;
	case FST_OPENPROJECT:
		gEditorMgr.Reset();
		gEditorMgr.OpenProject(path);
		return;
	}
	ocWarning << "Folder through FolderSelector with tag " << tag << " selected, but no action defined.";
}



void Editor::EditorMenu::InitComponentMenu()
{
	CEGUI_EXCEPTION_BEGIN
	CEGUI::Window* addComponentMenu = gCEGUIWM.getWindow(menubarPrefix + "/Edit/NewComponent/AutoPopup");
	for (int32 i = 0; i < EntitySystem::NUM_COMPONENT_TYPES; ++i)
	{
		const string& componentName = EntitySystem::GetComponentTypeName((EntitySystem::eComponentType)i);
		CEGUI::Window* componentMenuItem = gCEGUIWM.createWindow("Editor/MenuItem",
				menubarPrefix + "/Edit/NewComponent/Component" + StringConverter::ToString(i));
		componentMenuItem->setText(componentName);
		addComponentMenu->addChildWindow(componentMenuItem);
	}
	CEGUI_EXCEPTION_END
}

void EditorMenu::UpdateSceneMenu()
{
	CEGUI_EXCEPTION_BEGIN
	Core::SceneInfoList scenes;
	gEditorMgr.GetCurrentProject()->GetSceneList(scenes);

	CEGUI::Window* openSceneMenu = gCEGUIWM.getWindow(menubarPrefix + "/Scene/OpenScene/AutoPopup");
	
	for (int32 i = openSceneMenu->getChildCount() - 1; i >= 0; --i)
	{
		gCEGUIWM.destroyWindow(openSceneMenu->getChildAtIdx(i));
	}

	for (Core::SceneInfoList::const_iterator it = scenes.begin(); it != scenes.end(); ++it)
	{
		CEGUI::Window* sceneMenuItem = gCEGUIWM.createWindow("Editor/MenuItem",
				menubarPrefix + "/Scene/OpenScene/Scene" + it->filename);
		sceneMenuItem->setText(it->name + " (" + it->filename + ")");
		sceneMenuItem->setID(it - scenes.begin());
		openSceneMenu->addChildWindow(sceneMenuItem);
	}
	ConfigureMenu(openSceneMenu);
	CEGUI_EXCEPTION_END
}

void Editor::EditorMenu::ConfigureMenu(CEGUI::Window* parent)
{
	size_t childCount = parent->getChildCount();
	for(size_t childIdx = 0; childIdx < childCount; childIdx++)
	{
		if (parent->getChildAtIdx(childIdx)->testClassName("MenuItem")
				|| parent->getChildAtIdx(childIdx)->testClassName("PopupMenu"))
		{
			parent->getChildAtIdx(childIdx)->subscribeEvent(CEGUI::MenuItem::EventMouseEnters,
					CEGUI::Event::Subscriber(&Editor::EditorMenu::OnMouseEntersMenuItem, this));

			if(parent->getChildAtIdx(childIdx)->testClassName("MenuItem"))
			{
				parent->getChildAtIdx(childIdx)->subscribeEvent(CEGUI::MenuItem::EventClicked,
						CEGUI::Event::Subscriber(&Editor::EditorMenu::OnMenuItemClicked, this));
			}
		}
		ConfigureMenu(parent->getChildAtIdx(childIdx));
	}
}

void Editor::EditorMenu::ConfigureToolbar(CEGUI::Window* parent)
{
	size_t childCount = parent->getChildCount();
	for(size_t childIdx = 0; childIdx < childCount; childIdx++)
	{
		CEGUI::Window* child = parent->getChildAtIdx(childIdx);
		
		if (child->testClassName("RadioButton"))
		{
			CEGUI::RadioButton* radioButton = static_cast<CEGUI::RadioButton*>(child);
			const CEGUI::String& name = radioButton->getName();

			// set radio button groups
			if ((name == (toolbarPrefix + "/ResumeAction")) ||
				(name == (toolbarPrefix + "/PauseAction")) ||
				(name == (toolbarPrefix + "/RestartAction")) )
			{
				radioButton->setGroupID(0);
				mActionButtons.push_back(radioButton);
			}
			else if ((name == (toolbarPrefix + "/EditToolMove")) ||
					 (name == (toolbarPrefix + "/EditToolRotateZ")) ||
					 (name == (toolbarPrefix + "/EditToolRotateY")) ||
					 (name == (toolbarPrefix + "/EditToolScale")) )
			{
				radioButton->setGroupID(1);
				mToolButtons.push_back(radioButton);
			}

			// initially selected radio buttons
			if ((name == (toolbarPrefix + "/RestartAction")) ||
				(name == (toolbarPrefix + "/EditToolMove")) )
			{
				radioButton->setSelected(true);
			}

			child->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged,
					CEGUI::Event::Subscriber(&Editor::EditorMenu::OnToolbarButtonStateChanged, this));
		}
		ConfigureToolbar(child);
	}
}

void Editor::EditorMenu::SwitchToolButton( uint32 selectedButtonIndex )
{
	OC_ASSERT_MSG(selectedButtonIndex < mToolButtons.size(), "Invalid tool button index");
	mToolButtons[selectedButtonIndex]->setSelected(true);
}

void Editor::EditorMenu::SwitchActionButton( uint32 selectedButtonIndex )
{
	OC_ASSERT_MSG(selectedButtonIndex < mActionButtons.size(), "Invalid tool button index");
	mActionButtons[selectedButtonIndex]->setSelected(true);
}