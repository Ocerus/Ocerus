/// @file
/// Declares an abstract editor for properties with two dimensions.

#ifndef _2DEDITOR_H_
#define _2DEDITOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"

namespace Editor {

	class TwoDimEditor: public AbstractValueEditor
	{
	public:

		/// Constructs a 2DEditor that uses given model.
		TwoDimEditor(IValueEditorModel* model): mModel(model), mEditbox1Widget(0), mEditbox2Widget(0) { PROFILE_FNC(); }

		/// Destroys the 2DEditor and its model.
		virtual ~TwoDimEditor();

		/// Creates the main widget of this editor and returns it.
		virtual CEGUI::Window* CreateWidget(const CEGUI::String& namePrefix);

		/// @name CEGUI callbacks
		//@{
			bool OnEventActivated(const CEGUI::EventArgs&) { this->LockUpdates(); return true;}
			bool OnEventDeactivated(const CEGUI::EventArgs&);
			bool OnEventButtonRemovePressed(const CEGUI::EventArgs&);
			bool OnEventKeyDown(const CEGUI::EventArgs&);
		//@}

	protected:
		IValueEditorModel* mModel;
		CEGUI::Window* mEditbox1Widget;
		CEGUI::Window* mEditbox2Widget;
	};
}

#endif // _2DEDITOR_H_