//
// PropertySystem.cpp
//

#include "Common.h"
#include "PropertySystem.h"

PropertySystem gPropertySystem;

PropertySystem::~PropertySystem()
{
	std::list<CAbstractProperty*>* propertyList = GetProperties();
	// Destroy properties
	for ( std::list<CAbstractProperty*>::iterator it = propertyList->begin(); it != propertyList->end(); ++it )
		delete *it;
	propertyList->clear();
	DYN_DELETE propertyList;
}

std::list<CAbstractProperty*>* PropertySystem::GetProperties(void)
{
	static std::list<CAbstractProperty*>* propertyList = DYN_NEW std::list<CAbstractProperty*>();
	return propertyList;
}
