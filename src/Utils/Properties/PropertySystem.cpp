#include "Common.h"
#include "PropertySystem.h"

void Reflection::PropertySystem::DestroyProperties(void)
{
	std::list<AbstractProperty*>* propertyList = GetProperties();
	// Destroy properties
	for ( std::list<AbstractProperty*>::iterator it = propertyList->begin(); it != propertyList->end(); ++it )
		delete (*it);
	propertyList->clear();
	delete propertyList;
}

std::list<AbstractProperty*>* Reflection::PropertySystem::GetProperties(void)
{
	static std::list<AbstractProperty*>* propertyList = new std::list<AbstractProperty*>();
	return propertyList;
}
