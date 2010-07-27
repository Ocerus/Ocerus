/// @file
/// Declares an editor for resources.

#ifndef _RESOURCEEDITOR_H_
#define _RESOURCEEDITOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"
#include "GUISystem/CEGUIForwards.h"

namespace Editor
{
	/// The ResourceEditor class provides a value editor for resources. This editor only displays
	/// the name of the selected resource and allows to change the resource by drag&dropping the
	/// resource from resource list.
	/// @see ResourceWindow
	class ResourceEditor: public AbstractValueEditor
	{
	public:
		typedef ITypedValueEditorModel<ResourceSystem::ResourcePtr> Model;

		/// Constructs a ResourceEditor that uses given model.
		ResourceEditor(Model* model): mModel(model), mEditboxWidget(0) { }

		/// Destroys the ResourceEditor and its model.
		~ResourceEditor();

		/// Creates the main widget of this editor and returns it.
		virtual CEGUI::Window* CreateWidget(const CEGUI::String& namePrefix);

		/// Does nothing. ResourceEditor is submitted automatically when drag&dropped from resource list.
		virtual void Submit();

		/// Polls the model for current value and updates the editor widget, unless
		/// the editor is locked for updates.
		virtual void Update();

		/// @name CEGUI callbacks
		//@{
			bool OnEventDragDropItemDropped(const CEGUI::EventArgs&);
			bool OnEventButtonRemovePressed(const CEGUI::EventArgs&);
		//@}

	private:
		Model* mModel;
		CEGUI::Window* mEditboxWidget;
	};
}

#endif // _RESOURCEEDITOR_H_
