#include "Common.h"
#include "Editor/EditorMenu.h"
#include "Editor/EditorMgr.h"
#include "GUISystem/CEGUITools.h"
#include "Core/Application.h"

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
	CEGUI::Window* menubar = gCEGUIWM.getWindow(menubarPrefix);
	InitComponentMenu();
	ConfigureMenu(menubar);
	menubar->subscribeEvent(CEGUI::Window::EventDeactivated,
			CEGUI::Event::Subscriber(&Editor::EditorMenu::OnMouseLeavesMenuItem, this));

	CEGUI::Window* toolbar = gCEGUIWM.getWindow(toolbarPrefix);
	ConfigureToolbar(toolbar);
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
		return true;
	}

	if (itemName == menubarPrefix + "/File/OpenProject")
	{
		return true;
	}

	if (itemName == menubarPrefix + "/File/ExportGame")
	{
		return true;
	}

	if (itemName == menubarPrefix + "/File/Quit")
	{
		gApp.Shutdown();
		return true;
	}

	/// ---- Scene menu ----
	if (itemName == menubarPrefix + "/Scene")
	{
		return true;
	}

	if (itemName == menubarPrefix + "/Scene/NewScene")
	{
		return true;
	}

	if (itemName == menubarPrefix + "/Scene/OpenScene")
	{
		return true;
	}

	if (itemName == menubarPrefix + "/Scene/SaveScene")
	{
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

	/// New component
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
		gEditorMgr.DuplicateEntity();
		return true;
	}

	if (itemName == menubarPrefix + "/Edit/DeleteEntity")
	{
		gEditorMgr.DeleteEntity();
		return true;
	}

	if (itemName == menubarPrefix + "/Edit/CreatePrototype")
	{
		return true;
	}

	ocWarning << "MenuItem " << itemName << " clicked, but no action defined.";
	return true;
}

bool Editor::EditorMenu::OnToolbarButtonClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	const CEGUI::String& buttonName = args.window->getName();

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
		gEditorMgr.SetEditTool(EditorMgr::ET_MOVE);
		return true;
	}

	/// ---- Edit tool rotate ----
	if (buttonName == toolbarPrefix + "/EditToolRotate")
	{
		gEditorMgr.SetEditTool(EditorMgr::ET_ROTATE);
		return true;
	}

	/// ---- Edit tool rotate-z ----
	if (buttonName == toolbarPrefix + "/EditToolRotateZ")
	{
		gEditorMgr.SetEditTool(EditorMgr::ET_ROTATE_Z);
		return true;
	}

	/// ---- Edit tool scale ----
	if (buttonName == toolbarPrefix + "/EditToolScale")
	{
		gEditorMgr.SetEditTool(EditorMgr::ET_SCALE);
		return true;
	}

	ocWarning << "Toolbar button " << buttonName << " clicked, but no action defined.";
	return true;
}

void Editor::EditorMenu::InitComponentMenu()
{
	CEGUI::Window* addComponentMenu = gCEGUIWM.getWindow(menubarPrefix + "/Edit/NewComponent/AutoPopup");
	for (int i = 0; i < EntitySystem::NUM_COMPONENT_TYPES; ++i)
	{
		const string& componentName = EntitySystem::GetComponentTypeName((EntitySystem::eComponentType)i);
		CEGUI::Window* componentMenuItem = gCEGUIWM.createWindow("Editor/MenuItem",
				menubarPrefix + "/Edit/NewComponent/Component" + StringConverter::ToString(i));
		componentMenuItem->setText(componentName);
		addComponentMenu->addChildWindow(componentMenuItem);
	}
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

			//parent->getChildAtIdx(childIdx)->subscribeEvent(CEGUI::MenuItem::EventMouseLeaves,
					//CEGUI::Event::Subscriber(&Editor::EditorMenu::OnMouseLeavesMenuItem, this));

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
		if (child->testClassName("PushButton"))
		{
			child->subscribeEvent(CEGUI::PushButton::EventClicked,
					CEGUI::Event::Subscriber(&Editor::EditorMenu::OnToolbarButtonClicked, this));
		}
		ConfigureToolbar(child);
	}
}
