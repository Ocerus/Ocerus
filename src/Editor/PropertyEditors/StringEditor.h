/// @file
/// Declares an editor for properties convertible to strings.

#ifndef _STRINGEDITOR_H_
#define _STRINGEDITOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"

namespace Editor {

	template<class Model>
	class StringEditor: public Editor::AbstractValueEditor<Model>
	{
	public:
		/// Constructs a StringEditor that uses given model.
		StringEditor(const Model& model): AbstractValueEditor<Model>(model), mEditboxWidget(0) {}

		/// Creates an editbox and returns it. Caller must set its position
		/// and dimensions afterward. It's recommended to use GetEditorHeight()
		/// for the height.of the widget.
		virtual CEGUI::Window* CreateEditorWidget(CEGUI::Window* parent);

		/// Submits the value from editor widget to the model.
		virtual void Submit();

		/// Polls the model for current value and updates the editor widget, unless
		/// the editor is locked for updates.
		virtual void Update();

		/// Returns the recommended height for editor widget.
		virtual uint32 GetEditorHeightHint() const;

		/// @name CEGUI callbacks
		//@{
		bool OnEventActivated(const CEGUI::EventArgs&) { this->Update(); this->LockUpdates(); return true;}
		bool OnEventDeactivated(const CEGUI::EventArgs&) { this->UnlockUpdates(); this->Update(); return true;}
		bool OnEventKeyDown(const CEGUI::EventArgs&);
		//@}

	private:
		CEGUI::Window* mEditboxWidget;
	};
}

#endif // _STRINGEDITOR_H_
