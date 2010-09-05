#include "Common.h"
#include "EntityWindow.h"
#include "EditorMgr.h"
#include "ValueEditors/AbstractValueEditor.h"
#include "ValueEditors/PropertyEditorCreator.h"
#include "GUISystem/CEGUICommon.h"
#include "GUISystem/TabNavigation.h"
#include "GUISystem/VerticalLayout.h"

using namespace Editor;

const float32 PROPERTY_UPDATE_INTERVAL = 0.3f;

EntityWindow::EntityWindow(): mWindow(0), mScrollablePane(0), mVerticalLayout(0), mTabNavigation(0), mPropertyUpdateTimer(0), mNeedsRebuild(false)
{
	mTabNavigation = new GUISystem::TabNavigation();
}

EntityWindow::~EntityWindow()
{
	gGUIMgr.DestroyWindow(mWindow);
	delete mVerticalLayout;
	delete mTabNavigation;
}

void EntityWindow::Init()
{
	mWindow = gGUIMgr.LoadSystemLayout("EntityWindow.layout", "Editor");
	if (!mWindow)
	{
		ocError << "Can't load EntityWindow.";
		return;
	}
	mWindow->setUserString("WantsMouseWheel", "True");
	gGUIMgr.GetGUISheet()->addChildWindow(mWindow);
	mScrollablePane = static_cast<CEGUI::ScrollablePane*>(mWindow->getChild(mWindow->getName() + "/Scrollable"));
	mScrollablePane->setUserString("WantsMouseWheel", "True");
	mVerticalLayout = new GUISystem::VerticalLayout(mScrollablePane, const_cast<CEGUI::ScrolledContainer*>(mScrollablePane->getContentPane()));
}

void EntityWindow::Update(float32 delta)
{
	if (!mWindow) return;
	mPropertyUpdateTimer += delta;
	if (mPropertyUpdateTimer > PROPERTY_UPDATE_INTERVAL && gEditorMgr.GetCurrentEntity().IsValid())
	{
		mPropertyUpdateTimer = 0;
		if (!gEditorMgr.GetCurrentEntity().Exists() || mNeedsRebuild)
		{
			Rebuild();
		}
		else
		{
			for (PropertyEditors::const_iterator it = mPropertyEditors.begin(); it != mPropertyEditors.end(); ++it)
			{
				(*it)->Update();
			}
		}
	}
}

void EntityWindow::Rebuild()
{
	if (!mWindow) return;

	mNeedsRebuild = false;
	float verticalScrollPosition = mScrollablePane->getVerticalScrollPosition() * mScrollablePane->getContentPaneArea().getHeight();

	Clear();

	// There is no entity to be selected.
	EntitySystem::EntityHandle currentEntity = gEditorMgr.GetCurrentEntity();
	if (!currentEntity.Exists()) return;

	mVerticalLayout->LockUpdates();

	// First "component" is general entity info
	{
		const CEGUI::String namePrefix = mWindow->getName() +"/ComponentGeneralInfo";
		CEGUI::GroupBox* componentGroup = static_cast<CEGUI::GroupBox*>(gGUIMgr.CreateWindow("Editor/GroupBox", true, "EntityInfo"));
		componentGroup->setText(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "general_info"));
		mVerticalLayout->AddChildWindow(componentGroup);
		GUISystem::VerticalLayout* layout = new GUISystem::VerticalLayout(componentGroup, componentGroup->getContentPane(), true);
		mVerticalLayouts.push_back(layout);

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
		{
			AbstractValueEditor* editor = CreateEntityTagEditor(currentEntity);
			mPropertyEditors.push_back(editor);
			layout->AddChildWindow(editor->CreateWidget(namePrefix + "/EntityTagEditor"));
		}
		{
			AbstractValueEditor* editor = CreateEntityPrototypeEditor(currentEntity);
			mPropertyEditors.push_back(editor);
			layout->AddChildWindow(editor->CreateWidget(namePrefix + "/EntityPrototypeEditor"));
		}

		layout->UpdateLayout();
	}

	int32 componentCount = EntitySystem::EntityMgr::GetSingleton().GetNumberOfEntityComponents(currentEntity);

	for (int componentID = 0; componentID < componentCount; ++componentID)
	{
		const CEGUI::String namePrefix = mWindow->getName() + "/" + StringConverter::ToString(componentID);
		const string componentName = EntitySystem::GetComponentTypeName(gEntityMgr.GetEntityComponentType(currentEntity, componentID));

		// Create component window
		CEGUI::GroupBox* componentGroup = static_cast<CEGUI::GroupBox*>(gGUIMgr.CreateWindow("Editor/GroupBox", true, namePrefix));
		componentGroup->setText(componentName);
		mVerticalLayout->AddChildWindow(componentGroup);

		// Create remove component button
		CEGUI::PushButton* removeComponentButton = static_cast<CEGUI::PushButton*>(gGUIMgr.CreateWindow("Editor/Button", true, namePrefix + "/RemoveButton"));
		removeComponentButton->setArea(CEGUI::URect(CEGUI::UDim(1, -8), CEGUI::UDim(0, -24), CEGUI::UDim(1, 12), CEGUI::UDim(0, -4)));
		removeComponentButton->setText("x");
		removeComponentButton->setClippedByParent(false);

		if (!gEntityMgr.IsEntityLinkedToPrototype(currentEntity))
		{
			if (gEntityMgr.CanDestroyEntityComponent(currentEntity, componentID))
			{
				removeComponentButton->setTooltipText(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "remove_component_hint"));
				removeComponentButton->setID(componentID);
				removeComponentButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::EntityWindow::OnRemoveComponentButtonClicked, this));
			}
			else
			{
				removeComponentButton->setTooltipText(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "remove_component_error_hint"));
				removeComponentButton->setEnabled(false);
			}
		}
		else
		{
			removeComponentButton->setTooltipText(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "remove_component_prototype_error_hint"));
			removeComponentButton->setEnabled(false);
		}

		componentGroup->addChildWindow(removeComponentButton);

		// Create property editors
		GUISystem::VerticalLayout* layout = new GUISystem::VerticalLayout(componentGroup, componentGroup->getContentPane(), true);
		mVerticalLayouts.push_back(layout);

		PropertyList propertyList;
		gEntityMgr.GetEntityComponentProperties(currentEntity, componentID, propertyList, Reflection::PA_EDIT_READ);
		
		set<PropertyHolder> sortedPropertyList;
		for (PropertyList::iterator it = propertyList.begin(); it != propertyList.end(); ++it)
		{
		  sortedPropertyList.insert(*it);
		}

		for (set<PropertyHolder>::iterator it = sortedPropertyList.begin(); it != sortedPropertyList.end(); ++it)
		{
			AbstractValueEditor* editor = CreatePropertyEditor(*it, currentEntity);
			mPropertyEditors.push_back(editor);
			layout->AddChildWindow(editor->CreateWidget(namePrefix + "/" + it->GetKey().ToString() + "Editor"));
		}
		layout->UpdateLayout();
	}

	mVerticalLayout->UnlockUpdates();
	mVerticalLayout->UpdateLayout();

	// restore the scrollbar position
	mScrollablePane->setVerticalScrollPosition(verticalScrollPosition / mScrollablePane->getContentPaneArea().getHeight());
	
	// nasty hack to solve scrollbar issue
	{
		const CEGUI::UDim height = mScrollablePane->getHeight();
		mScrollablePane->setHeight(height + CEGUI::UDim(0, 1));
		mScrollablePane->setHeight(height);
	}
}

void EntityWindow::Clear()
{
	if (!mWindow) return;

	mTabNavigation->Clear();

	// Clear all property editors. The editor windows are destroyed during the process.
	for (PropertyEditors::const_iterator it = mPropertyEditors.begin(); it != mPropertyEditors.end(); ++it)
	{
		delete (*it);
	}
	mPropertyEditors.clear();

	// Clear all layouts.
	for (VerticalLayouts::iterator it=mVerticalLayouts.begin(); it!=mVerticalLayouts.end(); ++it)
	{
		delete (*it);
	}
	mVerticalLayouts.clear();

	// Clear the pane - groupboxes get destroyed here.
	mVerticalLayout->Clear();

	// Everything should be clear by now.
	OC_ASSERT(mVerticalLayout->GetChildCount() == 0);
}

void Editor::EntityWindow::AddWidgetToTabNavigation(CEGUI::Window* widget)
{
	mTabNavigation->AddWidget(widget);
}

bool Editor::EntityWindow::OnRemoveComponentButtonClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	EntitySystem::ComponentID componentID = args.window->getID();
	gEditorMgr.RemoveComponentFromCurrentEntity(componentID);
	return true;
}
