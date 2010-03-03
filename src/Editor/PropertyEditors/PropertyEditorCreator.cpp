#include "Common.h"
#include "PropertyEditorCreator.h"

#include "AbstractPropertyEditor.h"
#include "StringPropertyEditor.h"

using namespace Editor;

AbstractPropertyEditor* Editor::CreatePropertyEditor(const PropertyHolder& property)
{
	// Hardwire: neslo zkompilovat
	/*switch (property.GetType())
	{
		default: return new StringPropertyEditor(property);
	}*/
	OC_NOT_REACHED();
	return 0;
}