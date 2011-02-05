/// @file
/// Declares editors for properties with two dimensions.

#ifndef _EDITOR_TWODIMEDITOR_H_
#define _EDITOR_TWODIMEDITOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"
#include "Models/IValueEditorModel.h"

namespace Editor {

	/// Represents an item in the entity editor working with two values of the same type.
	class TwoDimEditor: public AbstractValueEditor
	{
	public:

		/// Constructs a TwoDimEditor.
		TwoDimEditor() {}

		/// Destroys the TwoDimEditor.
		virtual ~TwoDimEditor();

		/// Submits the value from editor widget to the model.
		virtual void Submit();
		
		/// Polls the model for current value and updates the editor widget, unless
		/// the editor is locked for updates.
		virtual void Update();
		
	protected:

		/// Returns the model handling the data.
		virtual IValueEditorModel* GetModel() = 0;

		/// Sets new value into the model.
		virtual void SetValue(float32 x, float32 y) = 0;

		/// Gets a value from the model.
		virtual void GetValue(float32& x, float32& y) = 0;
	
		/// Creates the GUI widget.
		void InitWidget();

		/// Destroys the GUI widget.
		void DeinitWidget();
		
		/// True if the GUI widget is created and inited.
		bool IsWidgetInited() const;

		/// Sets attributes of the widget according to the given value model.
		void SetupWidget(IValueEditorModel* model);

		/// @name CEGUI callbacks
		//@{
			/// Called when an item widget is activated.
			bool OnEventActivated(const CEGUI::EventArgs&) { this->LockUpdates(); return true;}
			/// Called when an item widget is deactivated.
			bool OnEventDeactivated(const CEGUI::EventArgs&);
			/// Called when the mouse clicks on an item widget.
			bool OnRemoveButtonClicked(const CEGUI::EventArgs&);
			/// Called when a key is pressed while an item widget has focus.
			bool OnEventKeyDown(const CEGUI::EventArgs&);
			/// Called when the checkbox designating a property is shared is changed.
			bool OnEventIsSharedCheckboxChanged(const CEGUI::EventArgs&);
		//@}

	private:
		inline CEGUI::Window* GetEditbox1Widget();
		inline CEGUI::Window* GetEditbox2Widget();
	};


	/// Represents an item in the entity editor working with a vector value. The elements of the vector are
	/// specified by the template.
	template<class Vec>
	class VectorEditor: public TwoDimEditor
	{
	public:
		typedef ITypedValueEditorModel<Vec> Model;

		/// Constructs a VectorEditor that uses given model.
		VectorEditor(): mModel(0) {}

		/// Destroys the Vector2Editor and its model.
		virtual ~VectorEditor()
		{
			DestroyModel();
		}

		/// Changes the value model of the editor.
		void SetModel(Model* newModel)
		{
			mModel = newModel;
			SetupWidget(mModel);
		}
		
		/// Destroys the model.
		virtual void DestroyModel()
		{
			delete mModel;
			mModel = 0;
		}

	protected:
		virtual IValueEditorModel* GetModel() { return mModel; }

		virtual void GetValue(float32& x, float32& y)
		{
			if (mModel->IsValid())
			{
				Vec value = mModel->GetValue();
				x = (float32)value.x;
				y = (float32)value.y;
			}
		}
		
		virtual void SetValue(float32 x, float32 y)
		{
			Vec value(x, y);
			mModel->SetValue(value);
		}

	private:
		Model* mModel;
	};


	/// Represents an item in the entity editor working with a Vector2 value.
	class Vector2Editor: public VectorEditor<Vector2>
	{
	public:
		virtual eValueEditorType GetType() { return VET_PT_VECTOR2; }
		static const eValueEditorType Type; ///< Static type of the editor.
	};


	/// Represents an item in the entity editor working with a Point value.
	class PointEditor: public VectorEditor<GfxSystem::Point>
	{
	public:
		virtual eValueEditorType GetType() { return VET_PT_POINT; }
		static const eValueEditorType Type; ///< Static type of the editor.
	};
}

#endif // _EDITOR_TWODIMEDITOR_H_