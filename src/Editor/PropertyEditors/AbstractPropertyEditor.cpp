#include "Common.h"
#include "Editor/PropertyEditors/AbstractPropertyEditor.h"
#include "GUISystem/CEGUITools.h"

#include "CEGUI.h"

using namespace Editor;

Editor::AbstractPropertyEditor::AbstractPropertyEditor(const PropertyHolder& property): mProperty(property), mLabelWidget(0), mWasUpdated(0)
{
}

Editor::AbstractPropertyEditor::~AbstractPropertyEditor()
{
}

CEGUI::Window* Editor::AbstractPropertyEditor::CreateLabelWidget(CEGUI::Window* parent)
{
	OC_ASSERT(mLabelWidget == 0);
	mLabelWidget = gCEGUIWM.createWindow("Editor/StaticText", parent->getName() + "/" + mProperty.GetKey().ToString() + "Label");
	mLabelWidget->setText(mProperty.GetName());
	mLabelWidget->setProperty("FrameEnabled", "False");
	mLabelWidget->setProperty("BackgroundEnabled", "False");
	mLabelWidget->setTooltipText(mProperty.GetComment());
	parent->addChildWindow(mLabelWidget);
	return mLabelWidget;
}