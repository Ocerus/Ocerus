/// @file
/// Declares an editor for array properties convertible to strings.

#ifndef _ARRAYSTRINGEDITOR_H_
#define _ARRAYSTRINGEDITOR_H_


#include "Base.h"
#include "AbstractValueEditor.h"

namespace CEGUI
{
	class Listbox;
	class Window;
	class Editbox;
}

namespace Editor
{

	template<class Model, class ElementType>
	class ArrayStringEditor: public Editor::AbstractValueEditor<Model>
	{
		public:
			/// Constructs a ArrayStringEditor that uses given model.
			ArrayStringEditor(const Model& model): AbstractValueEditor<Model>(model), mEditboxWidget(0), mArrayEditorWidget(0) {}

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
				bool OnEventActivated(const CEGUI::EventArgs&);
				bool OnEventDeactivated(const CEGUI::EventArgs&) { this->UnlockUpdates(); this->Update(); return true;}
				bool OnEventKeyDown(const CEGUI::EventArgs&);
			//@}

		private:
			typedef Array<ElementType>* ArrayType;
			
			string mNamePrefix;
			CEGUI::Window* mEditboxWidget;
			CEGUI::Window* mArrayEditorWidget;
			CEGUI::Listbox* mArrayEditorListWidget;
	};
}

#include "ArrayStringEditorImpl.h"

#endif // _ARRAYSTRINGEDITOR_H_