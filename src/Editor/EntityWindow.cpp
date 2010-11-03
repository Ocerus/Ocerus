#include "Common.h"
#include "EntityWindow.h"
#include "EditorMgr.h"
#include "ValueEditors/AbstractValueEditor.h"
#include "ValueEditors/ValueEditorFactory.h"
#include "GUISystem/CEGUICommon.h"
#include "GUISystem/TabNavigation.h"
#include "GUISystem/VerticalLayout.h"
#include "ValueEditors/StringEditor.h"


using namespace Editor;

const float32 PROPERTY_UPDATE_INTERVAL = 0.3f;

EntityWindow::EntityWindow(): mWindow(0), mScrollablePane(0), mVerticalLayout(0), mTabNavigation(0), mValueEditorFactory(0), mPropertyUpdateTimer(0), mNeedsRebuild(false)
{
}

EntityWindow::~EntityWindow()
{
	gGUIMgr.DestroyWindow(mWindow);
	delete mVerticalLayout;
	delete mTabNavigation;
	delete mValueEditorFactory;
}

void EntityWindow::Init()
{
	mValueEditorFactory = new ValueEditorFactory();

	CEGUI_TRY;
	{
		mWindow = gGUIMgr.GetWindow("Editor/EntityWindow");
		mWindow->setUserString("WantsMouseWheel", "True");

		//mScrollablePane = static_cast<CEGUI::ScrollablePane*>(gGUIMgr.CreateWindow("Editor/ScrollablePane", "Editor/EntityWindow/Scrollable"));
		//mScrollablePane->setArea(CEGUI::URect(cegui_absdim(0), cegui_absdim(0), cegui_absdim(1), cegui_absdim(1)));

		mScrollablePane = static_cast<CEGUI::ScrollablePane*>(mWindow->getChild("Editor/EntityWindow/Scrollable"));

		mScrollablePane->setUserString("WantsMouseWheel", "True");

		mVerticalLayout = new GUISystem::VerticalLayout(mScrollablePane, const_cast<CEGUI::ScrolledContainer*>(mScrollablePane->getContentPane()));

		mWindow->addChildWindow(mScrollablePane);
		mTabNavigation = new GUISystem::TabNavigation(mScrollablePane);
	}
	CEGUI_CATCH_CRITICAL;
}

void EntityWindow::Deinit()
{
	gGUIMgr.DestroyWindow(mWindow);
	delete mValueEditorFactory;
	mValueEditorFactory = 0;
	DestroyComponentGroupCache();
}

void EntityWindow::Update(float32 delta)
{
	if (!mWindow) return;
	mPropertyUpdateTimer += delta;
	if (mPropertyUpdateTimer > PROPERTY_UPDATE_INTERVAL && gEditorMgr.GetSelectedEntity().IsValid())
	{
		mPropertyUpdateTimer = 0;
		if (!gEditorMgr.GetSelectedEntity().Exists() || mNeedsRebuild)
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
	EntitySystem::EntityHandle currentEntity = gEditorMgr.GetSelectedEntity();

	if (!currentEntity.Exists())
	{
		// Clear all groups
		for (size_t idx = 0; idx < mComponentGroupCache.size(); ++idx)
		{
			ClearComponentGroup(mComponentGroupCache[idx]);
			mComponentGroupCache[idx].widget->setVisible(false);
		}
		return;
	}

	mVerticalLayout->LockUpdates();

	int32 componentCount = EntitySystem::EntityMgr::GetSingleton().GetNumberOfEntityComponents(currentEntity);
	while ((int32)mComponentGroupCache.size() < componentCount + 1)
	{
		mComponentGroupCache.push_back(CreateComponentGroup());
	}

	// Clear all groups
	for (size_t idx = 0; idx < mComponentGroupCache.size(); ++idx)
	{
		ClearComponentGroup(mComponentGroupCache[idx]);
	}

	// First "component" is general entity info
	{
		ComponentGroup generalInfo = mComponentGroupCache[0];
		SetupComponentGroup(generalInfo);
		{
			AbstractValueEditor* editor = mValueEditorFactory->GetValueEditorForEntityAttribute(currentEntity, EntityAttributeModel::TYPE_ID);
			editor->GetWidget()->setVisible(true);
			mPropertyEditors.push_back(editor);
			generalInfo.layout->AddChildWindow(editor->GetWidget());
		}
		{
			AbstractValueEditor* editor = mValueEditorFactory->GetValueEditorForEntityAttribute(currentEntity, EntityAttributeModel::TYPE_NAME);
			editor->GetWidget()->setVisible(true);
			mPropertyEditors.push_back(editor);
			generalInfo.layout->AddChildWindow(editor->GetWidget());
		}
		{
			AbstractValueEditor* editor = mValueEditorFactory->GetValueEditorForEntityAttribute(currentEntity, EntityAttributeModel::TYPE_TAG);
			editor->GetWidget()->setVisible(true);
			mPropertyEditors.push_back(editor);
			generalInfo.layout->AddChildWindow(editor->GetWidget());
		}
		{
			AbstractValueEditor* editor = mValueEditorFactory->GetValueEditorForEntityAttribute(currentEntity, EntityAttributeModel::TYPE_PROTOTYPE);
			editor->GetWidget()->setVisible(true);
			mPropertyEditors.push_back(editor);
			generalInfo.layout->AddChildWindow(editor->GetWidget());
		}
		generalInfo.layout->UpdateLayout();
	}

	int componentID = 0;
	for (size_t idx = 1; idx < mComponentGroupCache.size(); ++idx)
	{
		ComponentGroup group = mComponentGroupCache[idx];
		if (componentID < componentCount)
		{
			SetupComponentGroup(group, currentEntity, componentID);

			PropertyList propertyList;
			gEntityMgr.GetEntityComponentProperties(currentEntity, componentID, propertyList, Reflection::PA_EDIT_READ);
		
			set<PropertyHolder> sortedPropertyList;
			for (PropertyList::iterator it = propertyList.begin(); it != propertyList.end(); ++it)
			{
				sortedPropertyList.insert(*it);
			}

			for (set<PropertyHolder>::iterator it = sortedPropertyList.begin(); it != sortedPropertyList.end(); ++it)
			{
				AbstractValueEditor* editor = mValueEditorFactory->GetValueEditorForProperty(*it, currentEntity);
				mPropertyEditors.push_back(editor);
				group.layout->AddChildWindow(editor->GetWidget());
			}
			group.layout->UpdateLayout();
		}
		else
		{
			group.widget->setVisible(false);
		}
		++componentID;
	}

	mVerticalLayout->UnlockUpdates();
	mVerticalLayout->UpdateLayout();

	// restore the scrollbar position
	mScrollablePane->setVerticalScrollPosition(verticalScrollPosition / mScrollablePane->getContentPaneArea().getHeight());
	
	// nasty hack to solve scrollbar issue
	{
		mVerticalLayout->LockUpdates();
		const CEGUI::UDim height = mScrollablePane->getHeight();
		mScrollablePane->setHeight(height + CEGUI::UDim(0, 1));
		mScrollablePane->setHeight(height);
		mVerticalLayout->UnlockUpdates();
	}
}

void EntityWindow::Clear()
{
	if (!mWindow) return;

	mTabNavigation->Clear();

	// Clear all property editors. The editor windows are destroyed during the process.
	for (PropertyEditors::const_iterator it = mPropertyEditors.begin(); it != mPropertyEditors.end(); ++it)
	{
		mValueEditorFactory->StoreValueEditor(*it);
	}
	mPropertyEditors.clear();

/*
	// Clear all layouts.
	for (VerticalLayouts::iterator it=mVerticalLayouts.begin(); it!=mVerticalLayouts.end(); ++it)
	{
		delete (*it);
	}
	mVerticalLayouts.clear();
*/
}

void Editor::EntityWindow::AddWidgetToTabNavigation(CEGUI::Window* widget)
{
	mTabNavigation->AddWidget(widget);
}

bool Editor::EntityWindow::OnRemoveComponentButtonClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	EntitySystem::ComponentID componentID = args.window->getID();
	gEditorMgr.RemoveComponentFromSelectedEntity(componentID);
	return true;
}

EntityWindow::ComponentGroup Editor::EntityWindow::CreateComponentGroup()
{
	static uint32 componentGroupCounter = 0;
	const CEGUI::String& windowName = "Editor/EntityWindow/ComponentGroup" + StringConverter::ToString(componentGroupCounter++);

	CEGUI::GroupBox* componentGroupWidget = static_cast<CEGUI::GroupBox*>(gGUIMgr.CreateWindow("Editor/GroupBox", windowName));
	componentGroupWidget->setWidth(cegui_reldim(1));

	// Create remove component button
	CEGUI::Window* removeComponentButton = gGUIMgr.CreateWindow("Editor/ImageButton", windowName + "/RemoveButton");
	removeComponentButton->setProperty("NormalImage", "set:EditorToolbar image:btnRemoveComponentNormal");
	removeComponentButton->setProperty("HoverImage", "set:EditorToolbar image:btnRemoveComponentHover");
	removeComponentButton->setProperty("PushedImage", "set:EditorToolbar image:btnRemoveComponentPushed");
	removeComponentButton->setProperty("DisabledImage", "set:EditorToolbar image:btnRemoveComponentDisabled");

	removeComponentButton->setVisible(true);
	removeComponentButton->setSize(CEGUI::UVector2(cegui_absdim(14), cegui_absdim(14)));
	removeComponentButton->setPosition(CEGUI::UVector2(CEGUI::UDim(1, -6), CEGUI::UDim(0, -20)));
	removeComponentButton->setClippedByParent(false);
	removeComponentButton->subscribeEvent(CEGUI::PushButton::EventClicked,
	CEGUI::Event::Subscriber(&Editor::EntityWindow::OnRemoveComponentButtonClicked, this));
	componentGroupWidget->addChildWindow(removeComponentButton);

	GUISystem::VerticalLayout* layout = new GUISystem::VerticalLayout(componentGroupWidget, componentGroupWidget->getContentPane(), true);

	ComponentGroup result;
	result.widget = componentGroupWidget;
	result.layout = layout;

	mVerticalLayout->AddChildWindow(componentGroupWidget);

	return result;
}

void EntityWindow::SetupComponentGroup(const Editor::EntityWindow::ComponentGroup& componentGroup)
{
	// Setup label
	componentGroup.widget->setVisible(true);
	componentGroup.widget->setText(TR("general_info"));

	// Setup remove component button
	CEGUI::Window* removeComponentButton = componentGroup.widget->getChildAtIdx(0)->getChildAtIdx(0);

	removeComponentButton->setVisible(false);
}

void EntityWindow::SetupComponentGroup(const Editor::EntityWindow::ComponentGroup& componentGroup, EntitySystem::EntityHandle entity, int componentID)
{
	const string componentName = EntitySystem::GetComponentTypeName(gEntityMgr.GetEntityComponentType(entity, componentID));

	// Setup label
	componentGroup.widget->setVisible(true);
	componentGroup.widget->setText(componentName);


	// Setup remove component button
	CEGUI::Window* removeComponentButton = componentGroup.widget->getChildAtIdx(0)->getChildAtIdx(0);
	removeComponentButton->setVisible(true);
	if (!gEntityMgr.IsEntityLinkedToPrototype(entity))
	{
		if (gEntityMgr.CanDestroyEntityComponent(entity, componentID))
		{
			removeComponentButton->setEnabled(true);
			removeComponentButton->setTooltipText(TR("remove_component_hint"));
			removeComponentButton->setID(componentID);
		}
		else
		{
			removeComponentButton->setTooltipText(TR("remove_component_error_hint"));
			removeComponentButton->setEnabled(false);
		}
	}
	else
	{
		removeComponentButton->setTooltipText(TR("remove_component_prototype_error_hint"));
		removeComponentButton->setEnabled(false);
	}
}

void EntityWindow::ClearComponentGroup(const Editor::EntityWindow::ComponentGroup& componentGroup)
{
	// Clear layout
	componentGroup.layout->Clear();
}


void EntityWindow::DestroyComponentGroupCache()
{
	for (ComponentGroupCache::iterator it = mComponentGroupCache.begin(); it != mComponentGroupCache.end(); ++it)
	{
		gGUIMgr.DestroyWindow(it->widget);
		delete it->layout;
	}
	mComponentGroupCache.clear();
}