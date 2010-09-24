#include "Common.h"
#include "AbstractValueEditor.h"
#include "Editor/EditorMgr.h"
#include "Editor/EntityWindow.h"
#include "GUISystem/CEGUICommon.h"

using namespace Editor;

void AbstractValueEditor::ResetWidget()
{
	CEGUI::Window* parent = mEditorWidget->getParent();
	if (parent != 0)
		parent->removeChildWindow(mEditorWidget);
}

CEGUI::Window* AbstractValueEditor::CreateLabelWidget(const CEGUI::String& name)
{
	CEGUI::Window* labelWidget = gGUIMgr.CreateWindowDirectly("Editor/StaticText", name + "/Label");
	labelWidget->setProperty("FrameEnabled", "False");
	labelWidget->setProperty("BackgroundEnabled", "False");
	labelWidget->setProperty("VertFormatting", "VertCentred");
	return labelWidget;
}

CEGUI::Window* Editor::AbstractValueEditor::CreateRemoveButtonWidget(const CEGUI::String& name)
{
	CEGUI::Window* removeButton = gGUIMgr.CreateWindowDirectly("Editor/Button", name);
	removeButton->setText(TR("entity_editor_remove"));
	removeButton->setSize(CEGUI::UVector2(CEGUI::UDim(0, GetEditboxHeight()), CEGUI::UDim(0, GetEditboxHeight())));
	return removeButton;
}

CEGUI::Checkbox* AbstractValueEditor::CreateIsSharedCheckboxWidget(const CEGUI::String& name)
{
	CEGUI::Checkbox* checkbox = static_cast<CEGUI::Checkbox*>(gGUIMgr.CreateWindowDirectly("Editor/Checkbox", name));
	checkbox->setTooltipText(TR("prototype_shared_checkbox_hint"));
	checkbox->setSize(CEGUI::UVector2(CEGUI::UDim(0, GetEditboxHeight()), CEGUI::UDim(0, GetEditboxHeight())));
	return checkbox;
}

CEGUI::Window* AbstractValueEditor::CreateIsLockedImageWidget(const CEGUI::String& name)
{
	CEGUI::Window* lockWidget = gGUIMgr.CreateWindowDirectly("Editor/StaticImage", name);
	lockWidget->setProperty("FrameEnabled", "False");
	lockWidget->setProperty("BackgroundEnabled", "False");
	lockWidget->setProperty("Image", "set:EditorToolbar image:imgLock");
	lockWidget->setTooltipText(TR("entity_property_locked_hint"));
	lockWidget->setSize(CEGUI::UVector2(cegui_absdim(16), cegui_absdim(16)));
	lockWidget->setPosition(CEGUI::UVector2(CEGUI::UDim(1, -16), cegui_absdim(5)));
	return lockWidget;
}

float Editor::AbstractValueEditor::GetEditboxHeight()
{
	static float result = gGUIMgr.GetGUISheet()->getFont(true)->getLineSpacing(1.1f) + 10;
	return result;
}

void Editor::AbstractValueEditor::AddWidgetToTabNavigation(CEGUI::Window* widget)
{
	gEditorMgr.GetEntityWindow()->AddWidgetToTabNavigation(widget);
}

ValueEditorFactory* AbstractValueEditor::GetValueEditorFactory()
{
	return gEditorMgr.GetEntityWindow()->GetValueEditorFactory();
}

