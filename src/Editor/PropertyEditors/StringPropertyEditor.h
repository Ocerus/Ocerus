/// @file
/// Declares an editor for properties convertable to strings.

#ifndef _STRINGPROPERTYEDITOR_H_
#define _STRINGPROPERTYEDITOR_H_

#include "Base.h"
#include "Editor/PropertyEditors/AbstractPropertyEditor.h"

namespace Editor {

	class StringPropertyEditor: public Editor::AbstractPropertyEditor
	{
	public:
		/// Constructs a StringPropertyEditor that manages given property.
		StringPropertyEditor(const PropertyHolder& property);

		virtual CEGUI::Window* CreateEditorWidget(CEGUI::Window* parent);

		virtual void SubmitEditorWidget();
		virtual void UpdateEditorWidget();

		virtual uint32 GetEditorHeight() const;

		/// This callback method is called whenever user submits the editbox.
		bool OnEditboxSubmit(const CEGUI::EventArgs&) { this->SubmitEditorWidget(); return true; }

	private:
		CEGUI::Window* mEditboxWidget;
	};

}

#endif // _STRINGPROPERTYEDITOR_H_
