/// @file
/// Declares an editor for properties convertible to strings.

#ifndef _STRINGEDITOR_H_
#define _STRINGEDITOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"

namespace Editor {

	class StringEditor: public AbstractValueEditor
	{
	public:
		typedef ITypedValueEditorModel<string> Model;
		
		/// Constructs a StringEditor that uses given model.
		StringEditor(Model* model): mModel(model), mEditboxWidget(0) {}

		/// Destroys the StringEditor and its model.
		~StringEditor();

		/// Creates the main widget of this editor and returns it.
		virtual CEGUI::Window* CreateWidget(const CEGUI::String& namePrefix);

		/// Submits the value from editor widget to the model.
		virtual void Submit();

		/// Polls the model for current value and updates the editor widget, unless
		/// the editor is locked for updates.
		virtual void Update();

		/// @name CEGUI callbacks
		//@{
			bool OnEventActivated(const CEGUI::EventArgs&) { this->LockUpdates(); return true;}
			bool OnEventDeactivated(const CEGUI::EventArgs&) { this->UnlockUpdates(); return true;}
			bool OnEventButtonRemovePressed(const CEGUI::EventArgs&);
			bool OnEventKeyDown(const CEGUI::EventArgs&);
		//@}

	private:
		Model* mModel;
		CEGUI::Window* mEditboxWidget;
	};
}

#endif // _STRINGEDITOR_H_