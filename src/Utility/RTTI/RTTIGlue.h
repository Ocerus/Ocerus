#ifndef _RTTICLASS_H
#define _RTTICLASS_H

#include "RTTI.h"
#include "../Properties/PropertySystem.h"
#include "../Properties/Property.h"
#include "../Hash.h"
#include "../../EntitySystem/ComponentMgr/ComponentEnums.h"

/** @name RTTIGlue is the RTTI "sandwich class" being used to augment a class with RTTI support. Classes
	supporting RTTI need to derive from this class, with their ancestor specified as the BaseClass
	template parameter.
*/
template <class T, class BaseClass> class RTTIGlue : public BaseClass
{
public :

	/// @name Constructor.
	RTTIGlue(void) {}
	
	/** @name Default factory function. Creates an instance of T. Called by the system to dynamically create
		class instances from class IDs.
	*/
	static T* Create(void) { return new T(); }

	/// @name Default reflection registration function. Does nothing by default.
	static void	RegisterReflection(void) {}

	/** @name Registers a property. Takes in the property name, its getter and setter functions, and the property
		type as a template parameter. Should be called from within a user-defined RegisterReflection function.
	*/
	template <class PropertyType> 
	static void RegisterProperty(const char* name, typename Property<T, PropertyType>::GetterType getter, 
								 typename Property<T, PropertyType>::SetterType setter, const uint8 accessFlags)
	{
		Property<T, PropertyType>* pProperty = new Property<T, PropertyType>( name, getter, setter, accessFlags );
		T::GetClassRTTI()->GetProperties()->push_back( pProperty );
		PropertySystem::GetProperties()->push_back( pProperty );
	}

	/// @name Adds a component dependant on the owner into the list.
	static void AddComponentDependency(const EntitySystem::eComponentType cmp)
	{
		T::GetClassRTTI()->GetComponentDependencies()->push_back(cmp);
	}

	/// @name ID of the T class.
	static ClassID& GetClassID(void)
	{
		static ClassID classID = HashString(typeid(T).name());
		return classID;
	}

	/// @name Returns RTTI info associated with this class type.
	static inline RTTI* GetClassRTTI(void) { return &mRTTI; }

	/// Returns RTTI info associated with this class instance.
	virtual RTTI* GetRTTI(void) const { return &mRTTI; }

protected :

	static RTTI	mRTTI;
																		
};

/** @name Creates an RTTI structure for a specified class pair.
	The first parameter (0) is a stub. See RTTI constructor for details.
*/
template <class T, class BaseClass>
RTTI RTTIGlue<T, BaseClass>::mRTTI
( 0, RTTIGlue<T, BaseClass>::GetClassID(), typeid(T).name(), BaseClass::GetClassRTTI(), (ClassFactoryFunc)T::Create, 
	(RegisterReflectionFunc)T::RegisterReflection );


#endif	// _RTTICLASS_H