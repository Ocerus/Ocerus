#include "Common.h"
#include "AbstractValueEditor.h"

#include "EntityAttributeModel.h"

#include "GUISystem/CEGUITools.h"
#include "CEGUI.h"

using namespace Editor;

template<class Model>
CEGUI::Window* Editor::AbstractValueEditor<Model>::CreateLabelWidget(CEGUI::Window* parent)
{
	OC_ASSERT(mLabelWidget == 0);
	mLabelWidget = gCEGUIWM.createWindow("Editor/StaticText", parent->getName() + "/" + mModel.GetKey() + "Label");
	mLabelWidget->setText(mModel.GetName());
	mLabelWidget->setProperty("FrameEnabled", "False");
	mLabelWidget->setProperty("BackgroundEnabled", "False");
	mLabelWidget->setTooltipText(mModel.GetComment());
	parent->addChildWindow(mLabelWidget);
	return mLabelWidget;
}

/// Explicit Instantiation
template class Editor::AbstractValueEditor<PropertyHolder>;
template class Editor::AbstractValueEditor<EntityAttributeModel>;