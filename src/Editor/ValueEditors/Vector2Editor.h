/// @file
/// Declares an editor for Vector2 properties.

#ifndef _VECTOR2EDITOR_H_
#define _VECTOR2EDITOR_H_

#include "Base.h"
#include "TwoDimEditor.h"

namespace Editor {

	class Vector2Editor: public TwoDimEditor
	{
	public:
		typedef ITypedValueEditorModel<Vector2> Model;

		/// Constructs a Vector2Editor that uses given model.
		Vector2Editor(Model* model): TwoDimEditor(model) { PROFILE_FNC(); }

		/// Destroys the Vector2Editor and its model.
		virtual ~Vector2Editor() { }

		/// Submits the value from editor widget to the model.
		virtual void Submit();

		/// Polls the model for current value and updates the editor widget, unless
		/// the editor is locked for updates.
		virtual void Update();
	};
}

#endif // _VECTOR2EDITOR_H_
