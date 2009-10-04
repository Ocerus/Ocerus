#include "Common.h"
#include "PropertySystem.h"

PropertySystem gPropertySystem;

PropertySystem::~PropertySystem()
{
	std::list<AbstractProperty*>* propertyList = GetProperties();
	// Destroy properties
	for ( std::list<AbstractProperty*>::iterator it = propertyList->begin(); it != propertyList->end(); ++it )
		delete (*it);
	propertyList->clear();
	delete propertyList;
}

std::list<AbstractProperty*>* PropertySystem::GetProperties(void)
{
	static std::list<AbstractProperty*>* propertyList = new std::list<AbstractProperty*>();
	return propertyList;
}
