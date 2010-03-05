/// @file
/// Declares an abstract value editor with model.

#ifndef _ABSTRACTVALUEEDITOR_H_
#define _ABSTRACTVALUEEDITOR_H_

#include "Base.h"
#include "IValueEditor.h"

namespace CEGUI
{
	class EventArgs;
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
		AbstractValueEditor(const Model& model): mModel(model), mLabelWidget(0) {}

		/// Creates the label widget and returns it. Caller must set its position
		/// and dimensions afterward. It's recommended to use GetEditorHeight()
		/// for the height.of the widget.
		CEGUI::Window* CreateLabelWidget(CEGUI::Window* parent);

	protected:
		Model mModel;

	private:
		CEGUI::Window* mLabelWidget;
	};
}


#endif // _ABSTRACTVALUEEDITOR_H_