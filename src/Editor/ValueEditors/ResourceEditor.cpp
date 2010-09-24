#include "Common.h"
#include "ResourceEditor.h"
#include "Editor/ResourceWindow.h"
#include "Editor/EditorMgr.h"
#include "GUISystem/CEGUICommon.h"

using namespace Editor;

const eValueEditorType Editor::ResourceEditor::Type(VET_PT_RESOURCE);

void ResourceEditor::SetModel(ResourceEditor::Model* newModel)
{
	if (mEditorWidget == 0)
		InitWidget();

	StringEditor::SetModel(newModel);
	mEditboxWidget->setProperty("ReadOnly", "True");
}

void ResourceEditor::InitWidget()
{
	StringEditor::InitWidget();
	mEditboxWidget->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&Editor::ResourceEditor::OnEventDragDropItemDropped, this));
}

bool Editor::ResourceEditor::OnEventDragDropItemDropped(const CEGUI::EventArgs& e)
{
	const CEGUI::DragDropEventArgs& args = static_cast<const CEGUI::DragDropEventArgs&>(e);

	Model* model = static_cast<Model*>(mModel);
	
	if (args.dragDropItem->getUserString("DragDataType") != "Resource") { return false; }
	if (model->IsReadOnly())
	{ 
		/// @todo error message
		return true;
	}

	ResourceWindow* resourceWindow = static_cast<ResourceWindow*>(args.dragDropItem->getUserData());
	if (resourceWindow == 0) return true;

	ResourceSystem::ResourcePtr resource = resourceWindow->ItemEntryToResourcePtr(args.dragDropItem);
	if (resource.get())	model->GetWrappedModel()->SetValue(resource);
	Update();

	gEditorMgr.PropertyValueChanged();

	return true;
}
