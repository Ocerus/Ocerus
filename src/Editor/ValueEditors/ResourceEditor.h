/// @file
/// Declares an editor for resources.

#ifndef _EDITOR_RESOURCEEDITOR_H_
#define _EDITOR_RESOURCEEDITOR_H_

#include "Base.h"
#include "StringEditor.h"
#include "Models/StringValueEditorModel.h"

namespace Editor
{
	/// The ResourceEditor class provides a value editor for resources. This editor only displays
	/// the name of the selected resource and allows to change the resource by drag&dropping the
	/// resource from resource list.
	/// @see ResourceWindow
	class ResourceEditor: public StringEditor
	{
	public:
		typedef StringValueEditorModel<ResourceSystem::ResourcePtr> Model;

		/// Constructs a ResourceEditor that uses given model.
		ResourceEditor() {}

		/// Destroys the ResourceEditor and its model.
		virtual ~ResourceEditor() {}

		/// Sets the model of the editor.
		void SetModel(Model* newModel);

		/// Submits the value from editor widget to the model.
		virtual void Submit() {};

		/// Returns the type of value editor.
		virtual eValueEditorType GetType() { return VET_PT_RESOURCE; }

		static const eValueEditorType Type;

	private:
		void InitWidget();
		bool OnEventDragDropItemDropped(const CEGUI::EventArgs&);
	};
}

#endif // _RESOURCEEDITOR_H_
