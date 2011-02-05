/// @file
/// Declares an editor for boolean properties.

#ifndef _EDITOR_BOOLEDITOR_H_
#define _EDITOR_BOOLEDITOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"
#include "Models/IValueEditorModel.h"

namespace Editor {

	/// Represents an item in the entity editor working with a boolean value.
	class BoolEditor: public AbstractValueEditor
	{
	public:
		/// Model of the item.
		typedef ITypedValueEditorModel<bool> Model;
		
		/// Constructs a BoolEditor that uses given model.
		BoolEditor(): mModel(0), mCheckboxWidget(0) {}

		/// Destroys the BoolEditor and its model.
		virtual ~BoolEditor();

		/// Sets the model of the editor.
		void SetModel(Model* newModel);

		/// Destroys the model.
		virtual void DestroyModel();	

		/// Submits the value from editor widget to the model.
		virtual void Submit();

		/// Polls the model for current value and updates the editor widget, unless
		/// the editor is locked for updates.
		virtual void Update();

		/// Returns the type of value editor.
		virtual eValueEditorType GetType() { return VET_PT_BOOL; }
		
		static const eValueEditorType Type;


	private:
		void InitWidget();
		void DeinitWidget();
		void SetupWidget(Model* model);

		/// @name CEGUI callbacks
		//@{
			bool OnRemoveButtonClicked(const CEGUI::EventArgs&);
			bool OnEventCheckStateChanged(const CEGUI::EventArgs&);
			bool OnEventIsSharedCheckboxChanged(const CEGUI::EventArgs&);
		//@}

		Model* mModel;
		CEGUI::Checkbox* mCheckboxWidget;
	};
}

#endif // _EDITOR_BOOLEDITOR_H_
