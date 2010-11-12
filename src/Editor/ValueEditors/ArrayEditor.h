/// @file
/// Declares an editor for array properties convertible to strings.
/// Because ArrayEditor is template class, don't forget to include ArrayEditorImpl.h as well.

#ifndef _EDITOR_ARRAYEDITOR_H_
#define _EDITOR_ARRAYEDITOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"
#include "Models/ArrayElementModel.h"
#include "Utils/Array.h"

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
			ArrayEditor(): mModel(0), mHeaderWidget(0), mMainLayout(0), mBodyLayout(0), mButtonAddElement(0), mButtonRevert(0), mButtonSave(0) {}

			/// Destroys the ArrayEditor and its model.
			virtual ~ArrayEditor() { DestroyModel(); ResetWidget(); DeinitWidget(); }

			/// Assigns new model to the editor.
			void SetModel(Model* model);
			
			virtual void DestroyModel();

			virtual void ResetWidget();

			virtual CEGUI::Window* GetWidget();

			/// Submits the value from editor widget to the model.
			virtual void Submit();

			/// Polls the model for current value and updates the editor widget, unless
			/// the editor is locked for updates.
			virtual void Update();

			eValueEditorType GetType();

			static const eValueEditorType Type;

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
				
				/// Returns whether the model of the editor is read only.
				bool IsReadOnly() const;

				/// Returns whether the model of the editor is locked.
				bool IsLocked() const;
			//@}

		private:
			void InitWidget();
			
			void DeinitWidget();
			
			void SetupWidget(Model* model);
			
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

			/// Creates new value editor for array element on given index.
			AbstractValueEditor* CreateArrayElementEditor(uint32 index);

			/// @name CEGUI callbacks
			//@{
				bool OnEventButtonAddPressed(const CEGUI::EventArgs&);
				bool OnEventButtonRevertPressed(const CEGUI::EventArgs&);
				bool OnEventButtonSavePressed(const CEGUI::EventArgs&);
				bool OnEventIsSharedCheckboxChanged(const CEGUI::EventArgs&);
			//@}

			typedef vector<ElementType*> InternalArray;

			InternalArray mArray;
			Model* mModel;
			vector<AbstractValueEditor*> mElementEditors;
			CEGUI::String mNamePrefix;
			CEGUI::Window* mHeaderWidget;
			GUISystem::VerticalLayout* mMainLayout;
			GUISystem::VerticalLayout* mBodyLayout;
			CEGUI::PushButton* mButtonAddElement;
			CEGUI::PushButton* mButtonRevert;
			CEGUI::PushButton* mButtonSave;
	};
}

#endif // _ARRAYEDITOR_H_
