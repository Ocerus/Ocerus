#include "Common.h"
#include "AbstractValueEditor.h"

#include "EntityAttributeModel.h"

#include "GUISystem/CEGUITools.h"
#include "CEGUI.h"

using namespace Editor;

template<class Model>
CEGUI::Window* Editor::AbstractValueEditor<Model>::CreateLabelWidget(const CEGUI::String& namePrefix)
{
	CEGUI::Window* labelWidget = gCEGUIWM.createWindow("Editor/StaticText", namePrefix + "/EditorLabel");
	labelWidget->setText(mModel.GetName());
	labelWidget->setProperty("FrameEnabled", "False");
	labelWidget->setProperty("BackgroundEnabled", "False");
	labelWidget->setTooltipText(mModel.GetComment());
	return labelWidget;
}

/// Explicit Instantiation
template class Editor::AbstractValueEditor<PropertyHolder>;
template class Editor::AbstractValueEditor<EntityAttributeModel>;