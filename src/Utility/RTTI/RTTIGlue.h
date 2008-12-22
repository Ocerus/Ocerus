//
// RTTIClass.h
//

#ifndef _RTTICLASS_H
#define _RTTICLASS_H

#include "RTTI.h"
#include "../Properties/PropertySystem.h"
#include "../Properties/Property.h"
#include "../Hash.h"
#include "../../EntitySystem/ComponentMgr/ComponentEnums.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// RTTIGlue
//
// RTTIGlue is the RTTI "sandwich class" being used to augment a class with RTTI support. Classes
// supporting RTTI need to derive from this class, with their ancestor specified as the BaseClass
// template parameter.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class BaseClass> class RTTIGlue : public BaseClass
{

public :

	static	ClassID	ClassCLID;
	
	//----------------------------------------------------------------------------------------------
	// Constructor
	RTTIGlue();
	
	//----------------------------------------------------------------------------------------------
	// Default factory function. Creates an instance of T. Called by the system to dynamically create
	// class instances from class IDs.
	static T*	Create();

	//----------------------------------------------------------------------------------------------
	// Default reflection registration function. Does nothing by default.
	static void	RegisterReflection();

	//----------------------------------------------------------------------------------------------
	// Registers a property. Takes in the property name, its getter and setter functions, and the property
	// type as a template parameter. Should be called from within a user-defined RegisterReflection function.
	template <class PropertyType> 
	static	void RegisterProperty(	const char* szName, typename CProperty<T, PropertyType>::GetterType Getter, 
									typename CProperty<T, PropertyType>::SetterType Setter, const uint8 accessFlags )
	{
		CProperty<T, PropertyType>* pProperty = new CProperty<T, PropertyType>( szName, Getter, Setter, accessFlags );
		T::GetClassRTTI()->GetProperties()->push_back( pProperty );
		PropertySystem::GetProperties()->push_back( pProperty );
	}

	static void AddComponentDependency(const EntitySystem::eComponentType cmp)
	{
		T::GetClassRTTI()->GetComponentDependencies()->push_back(cmp);
	}

	//----------------------------------------------------------------------------------------------
	// Returns RTTI info associated with this class type.
	static inline CRTTI* GetClassRTTI()
	{
		return &ms_RTTI;
	}

	//----------------------------------------------------------------------------------------------
	// Returns RTTI info associated with this class instance.
	virtual CRTTI* GetRTTI()
	{
		return &ms_RTTI;
	}

protected :

	static CRTTI	ms_RTTI;				// RTTI structure
																		
};

// The first parameter (0) is a stub. See CRTTI constructor for details.
template <class T, class BaseClass>
CRTTI RTTIGlue<T, BaseClass>::ms_RTTI
	( 0, HashString(typeid(T).name()), typeid(T).name(), BaseClass::GetClassRTTI(), (ClassFactoryFunc)T::Create, 
	(RegisterReflectionFunc)T::RegisterReflection );

template <class T, class BaseClass>
CRTTI RTTIGlue<T, BaseClass>::ClassCLID
	( HashString(typeid(T).name()) );

template <class T, class BaseClass> 
RTTIGlue<T, BaseClass>::RTTIGlue()
{
}

template <class T, class BaseClass> 
T* RTTIGlue<T, BaseClass>::Create()
{
	return new T();
}

template <class T, class BaseClass> 
void RTTIGlue<T, BaseClass>::RegisterReflection()
{
}


#endif	// _RTTICLASS_H