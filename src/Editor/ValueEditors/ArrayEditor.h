/// @file
/// Declares an editor for array properties convertible to strings.
/// Because ArrayEditor is template class, don't forget to include ArrayEditorImpl.h as well.

#ifndef _ARRAYEDITOR_H_
#define _ARRAYEDITOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"
#include "Utils/Array.h"
#include "GUISystem/CEGUITools.h"

namespace CEGUI
{
	class Listbox;
	class Window;
	class Editbox;
}

namespace Editor
{
	template<class ElementType> class ArrayElementModel;
	template<class ElementType> class ArrayStringElementModel;

	/// The ArrayEditor class provides a value editor for array properties. It manages the array by
	/// creating value editors for each element in array and some other buttons for creating new elements, etc.
	template<class ElementType>
	class ArrayEditor: public Editor::AbstractValueEditor
	{
		public:
			/// Array class that this model manages.
			typedef Array<ElementType> ArrayType;

			/// Type of model that the editor manages.
			typedef ITypedValueEditorModel<ArrayType*> Model;

			/// Constructs an ArrayEditor on given model.
			ArrayEditor(Model* model): mModel(model), mHeaderWidget(0), mLayout(0), mButtonAddElement(0), mButtonRevert(0), mButtonSave(0) { PROFILE_FNC(); }

			/// Destroys the ArrayEditor and its model.
			virtual ~ArrayEditor() { DeleteEditors(); DeleteInternalArray(); delete mModel; }

			/// Creates the main widget of this editor and returns it.
			virtual CEGUI::Window* CreateWidget(const CEGUI::String& namePrefix);

			/// Submits the value from editor widget to the model.
			virtual void Submit();

			/// Polls the model for current value and updates the editor widget, unless
			/// the editor is locked for updates.
			virtual void Update();

			
			/// @name CEGUI callbacks
			//@{
				bool OnEventButtonAddPressed(const CEGUI::EventArgs&);
				bool OnEventButtonRevertPressed(const CEGUI::EventArgs&);
				bool OnEventButtonSavePressed(const CEGUI::EventArgs&);
			//@}

		protected:
			friend class ArrayElementModel<ElementType>;
			friend class ArrayStringElementModel<ElementType>;

			/// @name Methods for array manipulation from ArrayElementModel and ArrayStringElementModel.
			//@{
				/// Returns the element from internal array on given index.
				const ElementType& GetElement(uint32 index) const;

				/// Sets the element in internal array on given index to given value.
				void SetElement(uint32 index, const ElementType& newValue);

				/// Remove the element on given index in internal array.
				void RemoveElement(uint32 index);
			//@}

		private:
			/// Submits all element editors, so mArray will be in sync with Editors.
			void SubmitEditors();

			/// Updates all element editors (if they are not locked for updates).
			void UpdateEditors();

			/// Deletes all element editors.
			void DeleteEditors();

			/// Submits the elements from internal array.
			void SubmitInternalArray();

			/// Updates the elements from internal array.
			void UpdateInternalArray();

			/// Deletes all elements from internal array.
			void DeleteInternalArray();

			/// Locks updates.
			void LockUpdates();

			/// Unlocks updates.
			void UnlockUpdates();

			typedef vector<ElementType*> InternalArray;

			InternalArray mArray;
			Model* mModel;
			vector<AbstractValueEditor*> mElementEditors;
			CEGUI::String mNamePrefix;
			CEGUI::Window* mHeaderWidget;
			GUISystem::VerticalLayout* mLayout;
			CEGUI::PushButton* mButtonAddElement;
			CEGUI::PushButton* mButtonRevert;
			CEGUI::PushButton* mButtonSave;
	};

}

#endif // _ARRAYEDITOR_H_
