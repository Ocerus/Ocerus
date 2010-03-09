/// @file
/// Declares an editor for properties convertible to strings.

#ifndef _VECTOR2EDITOR_H_
#define _VECTOR2EDITOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"

namespace Editor {

	template<class Model>
	class Vector2Editor: public Editor::AbstractValueEditor<Model>
	{
	public:
		/// Constructs a Vector2Editor that uses given model.
		Vector2Editor(const Model& model): AbstractValueEditor<Model>(model), mEditbox1Widget(0), mEditbox2Widget(0) {}

		virtual CEGUI::Window* CreateWidget(const CEGUI::String& namePrefix);

		/// Submits the value from editor widget to the model.
		virtual void Submit();

		/// Polls the model for current value and updates the editor widget, unless
		/// the editor is locked for updates.
		virtual void Update();

		/// @name CEGUI callbacks
		//@{
		bool OnEventActivated(const CEGUI::EventArgs&) { this->Update(); this->LockUpdates(); return true;}
		bool OnEventDeactivated(const CEGUI::EventArgs&);
		bool OnEventKeyDown(const CEGUI::EventArgs&);
		//@}

	private:
		CEGUI::Window* mEditbox1Widget;
		CEGUI::Window* mEditbox2Widget;
	};
}

#endif // _VECTOR2EDITOR_H_
