/// @file
/// Provides creating and caching of value editors.

#ifndef _EDITOR_VALUEEDITORFACTORY_H_
#define _EDITOR_VALUEEDITORFACTORY_H_

#include "Common.h"
#include "ValueEditorTypes.h"
#include "Models/EntityAttributeModel.h"

namespace Editor
{

	class ValueEditorFactory
	{
	public:
		ValueEditorFactory();
		~ValueEditorFactory();

		void StoreValueEditor(AbstractValueEditor* editor);

		template<class ValueEditor> ValueEditor* RestoreValueEditor();

		AbstractValueEditor* GetValueEditorForProperty(const Reflection::PropertyHolder& property, const EntitySystem::EntityHandle& entity);

		AbstractValueEditor* GetValueEditorForEntityAttribute(EntitySystem::EntityHandle entity, EntityAttributeModel::eAttributeType);

		template<class ValueEditor>
		ValueEditor* GetValueEditorWithModel(typename ValueEditor::Model* model);

	private:
		typedef vector<AbstractValueEditor*> ValueEditorList;
		vector<ValueEditorList> mEditorCache;
		
	};

	template<class ValueEditor>
	ValueEditor* ValueEditorFactory::RestoreValueEditor()
	{
		eValueEditorType type = ValueEditor::Type;
		ValueEditor* valueEditor = 0;

		if (mEditorCache[type].empty())
		{
			valueEditor = new ValueEditor();
		}
		else
		{
			valueEditor = static_cast<ValueEditor*>(mEditorCache[type].back());
			mEditorCache[type].pop_back();
		}
		return valueEditor;
	}

	template<class ValueEditor>
	ValueEditor* ValueEditorFactory::GetValueEditorWithModel(typename ValueEditor::Model* model)
	{
		ValueEditor* valueEditor = RestoreValueEditor<ValueEditor>();
		valueEditor->SetModel(model);
		valueEditor->Update();
		return valueEditor;
	}
}

#endif // _EDITOR_VALUEEDITORFACTORY_H_
