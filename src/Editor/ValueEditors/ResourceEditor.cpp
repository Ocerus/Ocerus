#include "Common.h"
#include "ResourceEditor.h"
#include "Editor/ResourceWindow.h"
#include "Editor/EditorMgr.h"
#include "GUISystem/CEGUICommon.h"

using namespace Editor;

Editor::ResourceEditor::~ResourceEditor()
{
	delete mModel;
}

CEGUI::Window* Editor::ResourceEditor::CreateWidget(const CEGUI::String& namePrefix)
{
	PROFILE_FNC();

	OC_DASSERT(mEditorWidget == 0);
	OC_DASSERT(mEditboxWidget == 0);

	/// Create main editor widget
	mEditorWidget = gGUIMgr.CreateWindow("DefaultWindow", true);
	mEditorWidget->setHeight(CEGUI::UDim(0, GetEditboxHeight()));

	CEGUI::UDim dimMiddle = mModel->IsListElement() ? CEGUI::UDim(0, 32) : CEGUI::UDim(0.5f, 0);
	CEGUI::UDim dimRight = mModel->IsRemovable() ? CEGUI::UDim(1, - GetEditboxHeight() - 2) : CEGUI::UDim(1, 0);

	/// Create label widget of the editor
	CEGUI::Window* labelWidget = this->CreateEditorLabelWidget(namePrefix + "/Label", mModel);
	labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), dimMiddle + CEGUI::UDim(0, -2), CEGUI::UDim(1, 0)));
	mEditorWidget->addChildWindow(labelWidget);

	/// Create editbox widget of the editor
	mEditboxWidget = static_cast<CEGUI::Editbox*>(gGUIMgr.CreateWindow("Editor/Editbox", true));
	mEditboxWidget->setArea(CEGUI::URect(dimMiddle + CEGUI::UDim(0, 2), CEGUI::UDim(0, 0), dimRight, CEGUI::UDim(0, GetEditboxHeight())));
	mEditboxWidget->setProperty("ReadOnly", "True");
	mEditorWidget->addChildWindow(mEditboxWidget);

	/// Create remove button, if needed
	if (mModel->IsRemovable())
	{
		CEGUI::Window* removeButton = CreateRemoveElementButtonWidget(namePrefix + "/RemoveButton");
		removeButton->setPosition(CEGUI::UVector2(dimRight + CEGUI::UDim(0, 2), CEGUI::UDim(0, 0)));
		removeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::ResourceEditor::OnEventButtonRemovePressed, this));

		mEditorWidget->addChildWindow(removeButton);
	}

	/// Create isShared checkbox, if needed
	if (mModel->IsShareable())
	{
		labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 16), CEGUI::UDim(0, 0), dimMiddle + CEGUI::UDim(0, -2), CEGUI::UDim(1, 0)));
		CEGUI::Checkbox* isSharedCheckbox = CreateIsSharedCheckboxWidget(namePrefix + "/IsSharedCheckbox");
		isSharedCheckbox->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
		isSharedCheckbox->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&ResourceEditor::OnEventIsSharedCheckboxChanged, this));
		isSharedCheckbox->setSelected(mModel->IsShared());
		mEditorWidget->addChildWindow(isSharedCheckbox);
	}

	/// Subscribe to editbox events
	mEditboxWidget->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&Editor::ResourceEditor::OnEventDragDropItemDropped, this));

	/// Update editor and return main widget
	Update();
	return mEditorWidget;
}

void Editor::ResourceEditor::Submit()
{
}

void Editor::ResourceEditor::Update()
{
	OC_DASSERT(mEditboxWidget != 0);
	mEditboxWidget->setText((mModel->IsValid() && mModel->GetValue()) ? mModel->GetValue()->GetName() : "");
	mEditorWidget->setTooltipText((mModel->IsValid() && mModel->GetValue()) ? mModel->GetValue()->GetFilePath() : "");
}
bool Editor::ResourceEditor::OnEventButtonRemovePressed(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	mModel->Remove();
	return true;
}

bool Editor::ResourceEditor::OnEventDragDropItemDropped(const CEGUI::EventArgs& e)
{
	const CEGUI::DragDropEventArgs& args = static_cast<const CEGUI::DragDropEventArgs&>(e);

	if (args.dragDropItem->getUserString("DragDataType") != "Resource") { return false; }
	if (mModel->IsReadOnly()) { return true; }

	ResourceWindow* resourceWindow = static_cast<ResourceWindow*>(args.dragDropItem->getUserData());
	if (resourceWindow == 0) return true;

	ResourceSystem::ResourcePtr resource = resourceWindow->ItemEntryToResourcePtr(args.dragDropItem);
	if (resource.get())	mModel->SetValue(resource);
	Update();

	gEditorMgr.PropertyValueChanged();

	return true;
}

bool Editor::ResourceEditor::OnEventIsSharedCheckboxChanged(const CEGUI::EventArgs& args)
{
	const CEGUI::WindowEventArgs& winArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
	mModel->SetShared(static_cast<CEGUI::Checkbox*>(winArgs.window)->isSelected());
	return true;
}
