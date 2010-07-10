/// @file
/// Declares an editor for boolean properties.

#ifndef _BOOLEDITOR_H_
#define _BOOLEDITOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"

namespace Editor {

	class BoolEditor: public AbstractValueEditor
	{
	public:
		typedef ITypedValueEditorModel<bool> Model;
		
		/// Constructs a BoolEditor that uses given model.
		BoolEditor(Model* model): mModel(model), mCheckboxWidget(0) { PROFILE_FNC(); }

		/// Destroys the BoolEditor and its model.
		~BoolEditor();

		/// Creates the main widget of this editor and returns it.
		virtual CEGUI::Window* CreateWidget(const CEGUI::String& namePrefix);

		/// Submits the value from editor widget to the model.
		virtual void Submit();

		/// Polls the model for current value and updates the editor widget, unless
		/// the editor is locked for updates.
		virtual void Update();

		/// @name CEGUI callbacks
		//@{
			bool OnEventButtonRemovePressed(const CEGUI::EventArgs&);
			bool OnEventCheckStateChanged(const CEGUI::EventArgs&);
		//@}

	private:
		Model* mModel;
		CEGUI::Checkbox* mCheckboxWidget;
	};
}

#endif // _BOOLEDITOR_H_
