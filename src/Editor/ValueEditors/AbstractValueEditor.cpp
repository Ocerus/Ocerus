#include "Common.h"
#include "AbstractValueEditor.h"
#include "GUISystem/CEGUITools.h"

using namespace Editor;

Editor::AbstractValueEditor::~AbstractValueEditor()
{
	gCEGUIWM.destroyWindow(mEditorWidget);
}

CEGUI::Window* Editor::AbstractValueEditor::CreateStaticTextWidget(const CEGUI::String& name, const CEGUI::String& text, const CEGUI::String& tooltip)
{
	CEGUI::Window* labelWidget = gCEGUIWM.createWindow("Editor/StaticText", name);
	labelWidget->setText(text);
	labelWidget->setProperty("FrameEnabled", "False");
	labelWidget->setProperty("BackgroundEnabled", "False");
	labelWidget->setProperty("VertFormatting", "VertCentred");
	labelWidget->setTooltipText(tooltip);
	return labelWidget;
}

CEGUI::Window* Editor::AbstractValueEditor::CreateEditorLabelWidget(const CEGUI::String& name, const IValueEditorModel* model)
{
	CEGUI::Window* widget = CreateStaticTextWidget(name, model->GetName(), model->GetTooltip());
	if (model->IsListElement())
		widget->setProperty("HorzFormatting", "RightAligned");
	return widget;
}
CEGUI::PushButton* Editor::AbstractValueEditor::CreateRemoveElementButtonWidget(const CEGUI::String& name)
{
	CEGUI::PushButton* button = static_cast<CEGUI::PushButton*>(gCEGUIWM.createWindow("Editor/Button", name));
	button->setText("RM");
	button->setSize(CEGUI::UVector2(CEGUI::UDim(0, GetEditboxHeight()), CEGUI::UDim(0, GetEditboxHeight())));
	return button;
}

float Editor::AbstractValueEditor::GetEditboxHeight()
{
	static float result = gCEGUIWM.getWindow("EditorRoot")->getFont(true)->getLineSpacing(1.1f) + 10;
	return result;
}