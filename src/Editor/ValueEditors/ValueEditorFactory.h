/// @file
/// Provides creating and caching of value editors.

#ifndef _EDITOR_VALUEEDITORFACTORY_H_
#define _EDITOR_VALUEEDITORFACTORY_H_

#include "Common.h"
#include "ValueEditorTypes.h"
#include "Models/EntityAttributeModel.h"

namespace Editor
{

	/// Factory pattern implementation for creating value editors like BoolEditor or StringEditor.
	class ValueEditorFactory
	{
	public:

		/// Default constructor.
		ValueEditorFactory();

		/// Default destructor.
		~ValueEditorFactory();

		/// Stores unused value editor into a cache for later use.
		void StoreValueEditor(AbstractValueEditor* editor);

		/// Reuses a value editor from the cache. Creates a new one if none is cached.
		template<class ValueEditor> ValueEditor* RestoreValueEditor();

		/// Creates a value editor of the correct type for the given propery.
		AbstractValueEditor* GetValueEditorForProperty(const Reflection::PropertyHolder& property, const EntitySystem::EntityHandle& entity);

		/// Creates a value editor of the correct type for the given entity attribute.
		AbstractValueEditor* GetValueEditorForEntityAttribute(EntitySystem::EntityHandle entity, EntityAttributeModel::eAttributeType);

		/// Creates a value editor of the correct type based on the given value model.
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
