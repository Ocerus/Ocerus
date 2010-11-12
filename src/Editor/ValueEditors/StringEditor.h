/// @file
/// Declares an editor for properties convertible to strings.

#ifndef _EDITOR_STRINGEDITOR_H_
#define _EDITOR_STRINGEDITOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"
#include "Models/IValueEditorModel.h"

namespace Editor {

	/// Represents an item in the entity editor working with a string value.
	class StringEditor: public AbstractValueEditor
	{
	public:
		/// Model of editable string values.
		typedef ITypedValueEditorModel<string> Model;
		
		/// Constructs a StringEditor that uses given model.
		StringEditor(): mModel(0), mEditboxWidget(0) { }

		/// Destroys the StringEditor and its model.
		~StringEditor();

		/// Sets the model of the editor.
		void SetModel(Model* newModel);

		/// Destroys the model.
		virtual void DestroyModel();

		/// Creates the main widget of this editor and returns it.
		virtual CEGUI::Window* GetWidget();

		
		/// Submits the value from editor widget to the model.
		virtual void Submit();

		/// Polls the model for current value and updates the editor widget, unless
		/// the editor is locked for updates.
		virtual void Update();

		virtual eValueEditorType GetType() { return VET_PT_STRING; }

		/// Static type of the editor.
		static const eValueEditorType Type;

	protected:
		/// @name CEGUI callbacks
		//@{
			bool OnEventActivated(const CEGUI::EventArgs&) { this->LockUpdates(); return true;}
			bool OnEventDeactivated(const CEGUI::EventArgs&) { this->UnlockUpdates(); return true;}
			bool OnRemoveButtonClicked(const CEGUI::EventArgs&);
			bool OnEventIsSharedCheckboxChanged(const CEGUI::EventArgs&);
			bool OnEventKeyDown(const CEGUI::EventArgs&);
		//@}

		void InitWidget();
		void DeinitWidget();
		void SetupWidget(Model* model);

		CEGUI::Window* GetEditboxWidget();

		Model* mModel;
		CEGUI::Window* mEditboxWidget;
	};
}

#endif // _STRINGEDITOR_H_
