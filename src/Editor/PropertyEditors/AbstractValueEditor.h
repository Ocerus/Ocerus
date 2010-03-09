/// @file
/// Declares an abstract value editor with model.

#ifndef _ABSTRACTVALUEEDITOR_H_
#define _ABSTRACTVALUEEDITOR_H_

#include "Base.h"
#include "IValueEditor.h"

namespace CEGUI
{
	class EventArgs;
	class String;
}

namespace Editor
{
	/// The AbstractValueEditor class is an abstract value editor with attached model. The used Model class
	/// must have these methods:
	///  - StringKey GetKey() - Returns the key of managed variable; this is used for names of the widgets.
	///  - string GetName() - Returns the name of managed variable; this is used as the caption of label widget.
	///  - string GetComment() - Returns the comment of managed variable; this is used as the tool-tip of label widget.
	template<class Model>
	class AbstractValueEditor: public IValueEditor
	{
	public:
		/// Constructs a AbstractValueEditor that operates on given model.
		AbstractValueEditor(const Model& model): mModel(model) {}


	protected:
		CEGUI::Window* CreateLabelWidget(const CEGUI::String& name);

		Model mModel;
	};
}


#endif // _ABSTRACTVALUEEDITOR_H_