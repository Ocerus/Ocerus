/// @file
/// Declares an editor for Point properties.

#ifndef _POINTEDITOR_H_
#define _POINTEDITOR_H_

#include "Base.h"
#include "TwoDimEditor.h"

namespace Editor {

	class PointEditor: public TwoDimEditor
	{
	public:
		typedef ITypedValueEditorModel<GfxSystem::Point> Model;

		/// Constructs a PointEditor that uses given model.
		PointEditor(Model* model): TwoDimEditor(model) { PROFILE_FNC(); }

		/// Destroys the PointEditor and its model.
		virtual ~PointEditor() { }

		/// Submits the value from editor widget to the model.
		virtual void Submit();

		/// Polls the model for current value and updates the editor widget, unless
		/// the editor is locked for updates.
		virtual void Update();
	};
}

#endif // _POINTEDITOR_H_
