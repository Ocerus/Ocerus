#include "Common.h"
#include "AbstractValueEditor.h"
#include "GUISystem/CEGUITools.h"

using namespace Editor;

Editor::AbstractValueEditor::~AbstractValueEditor()
{
	gGUIMgr.DestroyWindow(mEditorWidget);
}

CEGUI::Window* Editor::AbstractValueEditor::CreateStaticTextWidget(const CEGUI::String& name, const CEGUI::String& text, const CEGUI::String& tooltip)
{
	PROFILE_FNC();

	CEGUI::Window* labelWidget;
	labelWidget = gGUIMgr.CreateWindow("Editor/StaticText", true);
	labelWidget->setText(text);
	labelWidget->setProperty("FrameEnabled", "False");
	labelWidget->setProperty("BackgroundEnabled", "False");
	labelWidget->setProperty("VertFormatting", "VertCentred");
	labelWidget->setTooltipText(tooltip);
	return labelWidget;
}

CEGUI::Window* Editor::AbstractValueEditor::CreateEditorLabelWidget(const CEGUI::String& name, const IValueEditorModel* model)
{
	PROFILE_FNC();

	CEGUI::Window* widget = CreateStaticTextWidget(name, model->GetName(), model->GetTooltip());
	if (model->IsListElement())
		widget->setProperty("HorzFormatting", "RightAligned");
	return widget;
}
CEGUI::PushButton* Editor::AbstractValueEditor::CreateRemoveElementButtonWidget(const CEGUI::String& name)
{
	CEGUI::PushButton* button = static_cast<CEGUI::PushButton*>(gGUIMgr.CreateWindow("Editor/Button"));
	button->setText(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "entity_editor_remove"));
	button->setSize(CEGUI::UVector2(CEGUI::UDim(0, GetEditboxHeight()), CEGUI::UDim(0, GetEditboxHeight())));
	return button;
}

float Editor::AbstractValueEditor::GetEditboxHeight()
{
	static float result = gGUIMgr.GetWindow("EditorRoot")->getFont(true)->getLineSpacing(1.1f) + 10;
	return result;
}