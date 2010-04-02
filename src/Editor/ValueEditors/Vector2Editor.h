/// @file
/// Declares an editor for properties convertible to strings.

#ifndef _VECTOR2EDITOR_H_
#define _VECTOR2EDITOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"

namespace Editor {

	class Vector2Editor: public AbstractValueEditor
	{
	public:
		typedef ITypedValueEditorModel<Vector2> Model;

		/// Constructs a Vector2Editor that uses given model.
		Vector2Editor(Model* model): mModel(model), mEditbox1Widget(0), mEditbox2Widget(0) { PROFILE_FNC(); }

		/// Destroys the Vector2Editor and its model.
		virtual ~Vector2Editor();

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
			bool OnEventDeactivated(const CEGUI::EventArgs&);
			bool OnEventButtonRemovePressed(const CEGUI::EventArgs&);
			bool OnEventKeyDown(const CEGUI::EventArgs&);
		//@}

	private:
		Model* mModel;
		CEGUI::Window* mEditbox1Widget;
		CEGUI::Window* mEditbox2Widget;
	};
}

#endif // _VECTOR2EDITOR_H_
