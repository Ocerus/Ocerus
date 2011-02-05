/// @file
/// Declares an editor for prototypes.

#ifndef _EDITOR_PROTOTYPEEDITOR_H_
#define _EDITOR_PROTOTYPEEDITOR_H_

#include "Base.h"
#include "StringEditor.h"
#include "Models/IValueEditorModel.h"

namespace Editor
{
	/// The PrototypeEditor class provides a value editor for prototypes. This editor only displays
	/// the name of the selected prototype and allows to change the prototype by drag&dropping the
	/// prototype from prototype list.
	/// @see PrototypeWindow
	class PrototypeEditor: public StringEditor
	{
	public:
		typedef ITypedValueEditorModel<string> Model;

		/// Constructs a PrototypeEditor that uses given model.
		PrototypeEditor() {}

		/// Destroys the PrototypeEditor and its model.
		virtual ~PrototypeEditor() {}

		/// Sets the model of the editor.
		void SetModel(Model* newModel);

		/// Submits the value from editor widget to the model.
		virtual void Submit() {};

		/// Returns the type of value editor.
		virtual eValueEditorType GetType() { return VET_PT_PROTOTYPE; }

		static const eValueEditorType Type;

	private:
		void InitWidget();
		bool OnEventDragDropItemDropped(const CEGUI::EventArgs&);
	};
}

#endif // _EDITOR_PROTOTYPEEDITOR_H_
