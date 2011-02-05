/// @file
/// Template method implementations of ArrayEditor.

#include "Common.h"
#include "ArrayEditor.h"
#include "BoolEditor.h"
#include "StringEditor.h"
#include "ResourceEditor.h"
#include "TwoDimEditor.h"

#include "ValueEditorFactory.h"
#include "Models/ArrayElementModel.h"

#include "GUISystem/CEGUICommon.h"
#include "GUISystem/GUIMgr.h"
#include "GUISystem/VerticalLayout.h"
#include "Editor/EditorMgr.h"

namespace Editor
{

	template<class ElementType>
	void ArrayEditor<ElementType>::SetModel(Model* newModel)
	{
		if (mEditorWidget == 0)
			InitWidget();

		mModel = newModel;
		SetupWidget(mModel);
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::DestroyModel()
	{
		delete mModel;
		mModel = 0;
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::ResetWidget()
	{
		DeleteEditors();
		DeleteInternalArray();
		mBodyLayout->Clear();
		AbstractValueEditor::ResetWidget();
		UnlockUpdates();
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::InitWidget()
	{
		static unsigned int editorCounter = 0;
		OC_DASSERT(mEditorWidget == 0);

		CEGUI::String editorName = "Editor/EntityWindow/ValueEditors/ArrayEditor" + string(Reflection::PropertyTypes::GetStringName(Reflection::PropertyTypes::GetTypeID<ElementType>())) +  Utils::StringConverter::ToString(editorCounter++);
		
		// Create main editor widget
		mEditorWidget = gGUIMgr.CreateWindow("DefaultWindow", editorName);
		mMainLayout = new GUISystem::VerticalLayout(mEditorWidget, 0, true);
		
		// Create header widget
		mHeaderWidget = gGUIMgr.CreateWindow("DefaultWindow", editorName + "/Header");
		mHeaderWidget->setHeight(CEGUI::UDim(0, GetEditboxHeight()));
		mMainLayout->AddChildWindow(mHeaderWidget);

		// Create label widget
		CEGUI::Window* labelWidget = this->CreateLabelWidget(editorName + "/Header/Label");
		labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(0.5f, -2), CEGUI::UDim(0, GetEditboxHeight())));
		mHeaderWidget->addChildWindow(labelWidget);

		// Create add element button
		mButtonAddElement = static_cast<CEGUI::PushButton*>(gGUIMgr.CreateWindow("Editor/ImageButton", editorName + "/Header/AddElementButton"));
		mButtonAddElement->setProperty("NormalImage", "set:EditorToolbar image:btnAddNormal");
		mButtonAddElement->setProperty("HoverImage", "set:EditorToolbar image:btnAddHover");
		mButtonAddElement->setProperty("PushedImage", "set:EditorToolbar image:btnAddPushed");
		mButtonAddElement->setProperty("DisabledImage", "set:EditorToolbar image:btnAddDisabled");
		mButtonAddElement->setWantsMultiClickEvents(false);
		mButtonAddElement->setTooltipText(TR("entity_editor_add_hint"));
		mButtonAddElement->setVisible(true);
		mButtonAddElement->setSize(CEGUI::UVector2(cegui_absdim(24), cegui_absdim(24)));
		mButtonAddElement->setPosition(CEGUI::UVector2(CEGUI::UDim(1, -80), CEGUI::UDim(0, 0)));
		mButtonAddElement->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::ArrayEditor<ElementType>::OnEventButtonAddPressed, this));
		mHeaderWidget->addChildWindow(mButtonAddElement);

		// Create revert button
		mButtonRevert = static_cast<CEGUI::PushButton*>(gGUIMgr.CreateWindow("Editor/ImageButton", editorName + "/Header/RevertButton"));
		mButtonRevert->setProperty("NormalImage", "set:EditorToolbar image:btnCancelNormal");
		mButtonRevert->setProperty("HoverImage", "set:EditorToolbar image:btnCancelHover");
		mButtonRevert->setProperty("PushedImage", "set:EditorToolbar image:btnCancelPushed");
		mButtonRevert->setProperty("DisabledImage", "set:EditorToolbar image:btnCancelDisabled");
		mButtonRevert->setWantsMultiClickEvents(false);
		mButtonRevert->setTooltipText(TR("entity_editor_revert_hint"));
		mButtonRevert->setVisible(true);
		mButtonRevert->setSize(CEGUI::UVector2(cegui_absdim(24), cegui_absdim(24)));
		mButtonRevert->setPosition(CEGUI::UVector2(CEGUI::UDim(1, -52), CEGUI::UDim(0, 0)));
		mButtonRevert->setEnabled(false);
		mButtonRevert->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::ArrayEditor<ElementType>::OnEventButtonRevertPressed, this));
		mHeaderWidget->addChildWindow(mButtonRevert);

		// Create save button
		mButtonSave = static_cast<CEGUI::PushButton*>(gGUIMgr.CreateWindow("Editor/ImageButton", editorName + "/Header/SaveButton"));
		mButtonSave->setProperty("NormalImage", "set:EditorToolbar image:btnSaveNormal");
		mButtonSave->setProperty("HoverImage", "set:EditorToolbar image:btnSaveHover");
		mButtonSave->setProperty("PushedImage", "set:EditorToolbar image:btnSavePushed");
		mButtonSave->setProperty("DisabledImage", "set:EditorToolbar image:btnSaveDisabled");
		mButtonSave->setWantsMultiClickEvents(false);
		mButtonSave->setTooltipText(TR("entity_editor_save_hint"));
		mButtonSave->setVisible(true);
		mButtonSave->setSize(CEGUI::UVector2(cegui_absdim(24), cegui_absdim(24)));
		mButtonSave->setPosition(CEGUI::UVector2(CEGUI::UDim(1, -24), CEGUI::UDim(0, 0)));
		mButtonSave->setEnabled(false);
		mButtonSave->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::ArrayEditor<ElementType>::OnEventButtonSavePressed, this));
		mHeaderWidget->addChildWindow(mButtonSave);

		// Create is-shared checkbox
		CEGUI::Checkbox* isSharedCheckbox = CreateIsSharedCheckboxWidget(editorName + "/Header/IsSharedCheckbox");
		isSharedCheckbox->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
		isSharedCheckbox->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&ArrayEditor::OnEventIsSharedCheckboxChanged, this));
		mHeaderWidget->addChildWindow(isSharedCheckbox);

		// Create body widget
		CEGUI::Window* bodyWidget = gGUIMgr.CreateWindow("DefaultWindow", editorName + "/Body");
		mBodyLayout = new GUISystem::VerticalLayout(bodyWidget, 0, true);
		mMainLayout->AddChildWindow(bodyWidget);
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::DeinitWidget()
	{
		delete mBodyLayout;
		delete mMainLayout;
		gGUIMgr.DestroyWindow(mEditorWidget);
		mEditorWidget = 0;
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::SetupWidget(Model* model)
	{
		// Set dimensions
		CEGUI::UDim dimLeft = model->IsShareable() ? cegui_absdim(16) : cegui_absdim(0);

		// Setup label widget
		CEGUI::Window* labelWidget = mHeaderWidget->getChildAtIdx(0);
		labelWidget->setArea(CEGUI::URect(dimLeft, cegui_absdim(0), CEGUI::UDim(0.5f, -2), cegui_reldim(1)));
		labelWidget->setText(model->GetName());
		labelWidget->setTooltipText(model->GetTooltip());

		// Setup buttons
		if (model->IsReadOnly())
		{
			mButtonAddElement->setEnabled(false);
			mButtonSave->setEnabled(false);
			mButtonRevert->setEnabled(false);
		}
		else
		{
			mButtonAddElement->setEnabled(true);
			mButtonSave->setEnabled(false);
			mButtonRevert->setEnabled(false);
		}
		
		// Setup is-shared checkbox
		CEGUI::Checkbox* isSharedCheckbox = static_cast<CEGUI::Checkbox*>(mHeaderWidget->getChildAtIdx(4));
		if (model->IsShareable())
		{
			isSharedCheckbox->setVisible(true);
			isSharedCheckbox->setPosition(CEGUI::UVector2(cegui_absdim(0), cegui_absdim(0)));
			isSharedCheckbox->setSelected(model->IsShared());
		}
		else
		{
			isSharedCheckbox->setVisible(false);
		}

		// Setup body widget
		mEditorWidget->getChildAtIdx(1)->setHeight(cegui_absdim(0));
		mBodyLayout->Clear();
	}

	template<class ElementType>
	AbstractValueEditor* ArrayEditor<ElementType>::CreateArrayElementEditor(uint32 index)
	{
		ValueEditorFactory* ValueEditorFactory = GetValueEditorFactory();
		return ValueEditorFactory->GetValueEditorWithModel<StringEditor>(new StringValueEditorModel<ElementType>(new ArrayElementModel<ElementType>(this, index)));
	}

	template<>
	AbstractValueEditor* ArrayEditor<ResourceSystem::ResourcePtr>::CreateArrayElementEditor(uint32 index)
	{
		ValueEditorFactory* ValueEditorFactory = GetValueEditorFactory();
		return ValueEditorFactory->GetValueEditorWithModel<ResourceEditor>(new StringValueEditorModel<ResourceSystem::ResourcePtr>(new ArrayElementModel<ResourceSystem::ResourcePtr>(this, index)));
	}
	
	template<>
	AbstractValueEditor* ArrayEditor<Vector2>::CreateArrayElementEditor(uint32 index)
	{
		ValueEditorFactory* ValueEditorFactory = GetValueEditorFactory();
		return ValueEditorFactory->GetValueEditorWithModel<Vector2Editor>(new ArrayElementModel<Vector2>(this, index));
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::Submit()
	{
		SubmitEditors();
		SubmitInternalArray();
		UpdateInternalArray();
		UnlockUpdates();
		UpdateEditors();
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::Update()
	{
		/// If one of element editors is locked, consider the whole editor as locked.
		for (typename vector<AbstractValueEditor*>::iterator it = mElementEditors.begin(); it != mElementEditors.end(); ++it)
		{
			if ((*it)->UpdatesLocked())
				LockUpdates();
		}

		if (UpdatesLocked()) return;

		UpdateInternalArray();
		UpdateEditors();
	}

	template<class ElementType>
	bool ArrayEditor<ElementType>::OnEventButtonAddPressed(const CEGUI::EventArgs&)
	{
		LockUpdates();
		mArray.push_back(new ElementType(PropertyTypes::GetDefaultValue<ElementType>()));
		UpdateEditors();
		return true;
	}

	template<class ElementType>
	bool ArrayEditor<ElementType>::OnEventButtonRevertPressed(const CEGUI::EventArgs& arg)
	{
		OC_UNUSED(arg);
		UnlockUpdates();
		return true;
	}

	template<class ElementType>
	bool ArrayEditor<ElementType>::OnEventButtonSavePressed(const CEGUI::EventArgs& arg)
	{
		OC_UNUSED(arg);
		Submit();
		return true;
	}

	template<class ElementType>
	bool ArrayEditor<ElementType>::OnEventIsSharedCheckboxChanged(const CEGUI::EventArgs& args)
	{
		const CEGUI::WindowEventArgs& winArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
		bool isSelected = static_cast<CEGUI::Checkbox*>(winArgs.window)->isSelected();
		if (mModel->IsShared() != isSelected)
		{
			mModel->SetShared(isSelected);
		}
		return true;
	}

	template<class ElementType>
	const ElementType& ArrayEditor<ElementType>::GetElement(uint32 index) const
	{
		OC_DASSERT(index < mArray.size());
		return *mArray[index];
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::SetElement(uint32 index, const ElementType& newElement)
	{
		OC_DASSERT(index < mArray.size());
		*mArray[index] = newElement;
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::RemoveElement(uint32 index)
	{
		OC_DASSERT(index < mArray.size());
		LockUpdates();
		SubmitEditors();

		/// Remove given element from internal array.
		delete mArray[index];
		mArray.erase(mArray.begin() + index);

		UpdateEditors();
	}

	template<class ElementType>
	bool ArrayEditor<ElementType>::IsReadOnly() const
	{
		return mModel->IsReadOnly();
	}

	template<class ElementType>
	bool ArrayEditor<ElementType>::IsLocked() const
	{
		return mModel->IsLocked();
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::SubmitEditors()
	{
		for (typename vector<AbstractValueEditor*>::iterator it = mElementEditors.begin(); it != mElementEditors.end(); ++it)
		{
			(*it)->Submit();
		}
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::UpdateEditors()
	{
		if (mArray.size() != mElementEditors.size())
		{
			mBodyLayout->LockUpdates();
			while (mArray.size() < mElementEditors.size())
			{
				mBodyLayout->RemoveChildWindow(mElementEditors.back()->GetWidget());
				GetValueEditorFactory()->StoreValueEditor(mElementEditors.back());
				mElementEditors.pop_back();
			}

			for (uint32 i = mElementEditors.size(); i < mArray.size(); ++i)
			{
				AbstractValueEditor* editor = CreateArrayElementEditor(i);
				mElementEditors.push_back(editor);
				mBodyLayout->AddChildWindow(editor->GetWidget());
			}
			mBodyLayout->UnlockUpdates();
			mBodyLayout->UpdateLayout();
		}

		for (typename vector<AbstractValueEditor*>::iterator it = mElementEditors.begin(); it != mElementEditors.end(); ++it)
		{
			(*it)->Update();
		}
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::DeleteEditors()
	{
		for (typename vector<AbstractValueEditor*>::iterator it = mElementEditors.begin(); it != mElementEditors.end(); ++it)
		{
			GetValueEditorFactory()->StoreValueEditor(*it);
		}
		mElementEditors.clear();
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::SubmitInternalArray()
	{
		ArrayType newArray(mArray.size());
		uint32 i = 0;
		for (typename InternalArray::iterator it = mArray.begin(); it != mArray.end(); ++i, ++it)
		{
			newArray[i] = **it;
		}
		mModel->SetValue(&newArray);
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::UpdateInternalArray()
	{
		const ArrayType& array = *mModel->GetValue();
		uint32 oldSize = mArray.size();
		uint32 newSize = array.GetSize();
		uint32 greaterSize = oldSize > newSize ? oldSize : newSize;

		for (uint32 i = 0; i < greaterSize; ++i)
		{
			if (i < oldSize && i < newSize)
			{
				*mArray[i] = array[i];
			}
			else if (i >= newSize)
			{
				delete mArray.back();
				mArray.pop_back();
			}
			else /* (i >= oldSize) */
			{
				mArray.push_back(new ElementType(array[i]));
			}
		}
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::DeleteInternalArray()
	{
		for (typename InternalArray::iterator it = mArray.begin(); it != mArray.end(); ++it)
		{
			delete (*it);
		}
		mArray.clear();
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::LockUpdates()
	{
		if (mModel->IsReadOnly())
			return;

		AbstractValueEditor::LockUpdates();
		mButtonRevert->setEnabled(true);
		mButtonSave->setEnabled(true);
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::UnlockUpdates()
	{
		AbstractValueEditor::UnlockUpdates();

		mButtonRevert->setEnabled(false);
		mButtonSave->setEnabled(false);
	}


	// Specializations for ArrayEditor<>::GetType()
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) \
		template<> eValueEditorType Editor::ArrayEditor<typeClass>::GetType() { return VET_##typeID##_ARRAY; }
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	// Specializations for ArrayEditor<>::Type
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) \
		template<> const eValueEditorType ArrayEditor<typeClass>::Type(VET_##typeID##_ARRAY);
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	// Explicit instantiation of ArrayEditors
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) \
		template class ArrayEditor<typeClass>;
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE
}