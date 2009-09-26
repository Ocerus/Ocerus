#include "Common.h"
#include "PropertySystem.h"

PropertySystem gPropertySystem;

PropertySystem::~PropertySystem()
{
	std::list<AbstractProperty*>* propertyList = GetProperties();
	// Destroy properties
	for ( std::list<AbstractProperty*>::iterator it = propertyList->begin(); it != propertyList->end(); ++it )
		DYN_DELETE *it;
	propertyList->clear();
	DYN_DELETE propertyList;
}

std::list<AbstractProperty*>* PropertySystem::GetProperties(void)
{
	static std::list<AbstractProperty*>* propertyList = DYN_NEW std::list<AbstractProperty*>();
	return propertyList;
}
