/// @file
/// Declares editors for properties with two dimensions.

#ifndef _EDITOR_TWODIMEDITOR_H_
#define _EDITOR_TWODIMEDITOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"
#include "Models/IValueEditorModel.h"

namespace Editor {

	/// An abstract editor for editing properties with two dimensions.
	class TwoDimEditor: public AbstractValueEditor
	{
	public:

		/// Constructs a TwoDimEditor.
		TwoDimEditor() {}

		/// Destroys the TwoDimEditor.
		virtual ~TwoDimEditor();

		/// Returns the main widget of this editor.
		virtual CEGUI::Window* GetWidget();

		virtual void Submit();
		
		virtual void Update();
		
	protected:
		virtual IValueEditorModel* GetModel() = 0;
		virtual void SetValue(float32 x, float32 y) = 0;
		virtual void GetValue(float32& x, float32& y) = 0;
	
		void InitWidget();
		void DeinitWidget();
		bool IsWidgetInited() const;
		void SetupWidget(IValueEditorModel* model);

		/// @name CEGUI callbacks
		//@{
			bool OnEventActivated(const CEGUI::EventArgs&) { this->LockUpdates(); return true;}
			bool OnEventDeactivated(const CEGUI::EventArgs&);
			bool OnRemoveButtonClicked(const CEGUI::EventArgs&);
			bool OnEventKeyDown(const CEGUI::EventArgs&);
			bool OnEventIsSharedCheckboxChanged(const CEGUI::EventArgs&);
		//@}

	private:
		inline CEGUI::Window* GetEditbox1Widget();
		inline CEGUI::Window* GetEditbox2Widget();
	};

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

		void SetModel(Model* newModel)
		{
			mModel = newModel;
			SetupWidget(mModel);
		}
		
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

	class Vector2Editor: public VectorEditor<Vector2>
	{
	public:
		virtual eValueEditorType GetType() { return VET_PT_VECTOR2; }
		static const eValueEditorType Type;
	};

	class PointEditor: public VectorEditor<GfxSystem::Point>
	{
	public:
		virtual eValueEditorType GetType() { return VET_PT_POINT; }
		static const eValueEditorType Type;
	};
}

#endif // _EDITOR_TWODIMEDITOR_H_