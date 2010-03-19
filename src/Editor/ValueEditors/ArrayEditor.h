/// @file
/// Declares an editor for array properties convertible to strings.

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

	template<class ElementType>
	class ArrayEditor: public Editor::AbstractValueEditor
	{
		public:
			typedef Array<ElementType> ArrayType;
			typedef ITypedValueEditorModel<ArrayType*> Model;

			ArrayEditor(Model* model): mModel(model), mWidget(0), mHeaderWidget(0), mLayout(0) {}

			virtual ~ArrayEditor() { ClearArray(); delete mModel; }

			virtual CEGUI::Window* CreateWidget(const CEGUI::String& namePrefix);

			/// Submits the value from editor widget to the model.
			virtual void Submit() {} /// @TODO

			/// Polls the model for current value and updates the editor widget, unless
			/// the editor is locked for updates.
			virtual void Update();

			/// @name CEGUI callbacks
			//@{
				bool OnEventActivated(const CEGUI::EventArgs&);
				bool OnEventDeactivated(const CEGUI::EventArgs&) { this->UnlockUpdates(); this->Update(); return true;}
				bool OnEventKeyDown(const CEGUI::EventArgs&);
			//@}

		private:
			

			void UpdateArray();
			void SubmitArray();
			void ClearArray();


			typedef vector<ElementType*> InternalArray;

			InternalArray mArray;
			Model* mModel;
	
			vector<AbstractValueEditor*> mElementEditors;

			CEGUI::String mNamePrefix;
			CEGUI::Window* mWidget;
			CEGUI::Window* mHeaderWidget;
			GUISystem::VerticalLayout* mLayout;
	};

}

#include "ArrayEditorImpl.h"

#endif // _ARRAYEDITOR_H_
