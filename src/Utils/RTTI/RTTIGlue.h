/// @file
/// System for connecting the RTTI to custom classes.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef _RTTICLASS_H
#define _RTTICLASS_H

#include "Base.h"
#include "RTTI.h"
#include "../Properties/PropertySystem.h"
#include "../Properties/Property.h"

namespace Reflection
{
	/// @brief This is the %RTTI "sandwich class" being used to augment a class with %RTTI support. Classes
	///	supporting %RTTI need to derive from this class, with their ancestor specified as the BaseClass
	///	template parameter.
	template <class T, class BaseClass> class RTTIGlue : public BaseClass
	{
	public :

		/// Constructor.
		RTTIGlue(void) {}

		/// @brief Default factory function. Creates an instance of T.
		/// @remarks The factory function is called by the system to dynamically create
		///	class instances from class IDs.
		static T* Create(void) { return DYN_NEW T(); }

		/// Default reflection registration function. Registers nothing by default.
		static void	RegisterReflection(void) {}

		/// @brief Registers a property. Takes in the property name, its getter and setter functions, and the property
		///	type as a template parameter.
		/// @remarks This function should be called only from within a user-defined RegisterReflection function.
		template <class PropertyType> 
		static void RegisterProperty(const char* name, typename Property<T, PropertyType>::GetterType getter, 
			typename Property<T, PropertyType>::SetterType setter, const PropertyAccessFlags accessFlags)
		{
			Property<T, PropertyType>* pProperty = DYN_NEW Property<T, PropertyType>( name, getter, setter, accessFlags );
			T::GetClassRTTI()->AddProperty(pProperty);
			PropertySystem::GetProperties()->push_back( pProperty );
		}

		/// @brief Registers a function. Takes in the function name and its address.
		/// @remarks This function should be called only from within a user-defined RegisterReflection function.
		inline static void RegisterFunction(const char* name, typename Property<T, PropertyFunctionParameters&>::SetterType function, const PropertyAccessFlags accessFlags)
		{
			RegisterProperty<PropertyFunctionParameters&>(name, 0, function, accessFlags);
		}

		/// @brief Registers an entity component dependant on the owner of this class.
		/// @remarks Component dependencies solve the issue of defining the correct order of entity components creation
		/// and initialization. For example, components handling physics should be usually inited before the components
		/// handling logic.
		/// @remarks This function should be called only from within a user-defined RegisterReflection function.
		static void AddComponentDependency(const EntitySystem::eComponentType cmp)
		{
			T::GetClassRTTI()->AddComponentDependency(cmp);
		}

		/// Returns the ID of this class.
		static ClassID& GetClassID(void)
		{
			static ClassID classID = Hash::HashString(typeid(T).name());
			return classID;
		}

		/// Returns the RTTI info associated with this class.
		static inline RTTI* GetClassRTTI(void) { return &mRTTI; }

		/// Returns the RTTI info associated with this class instance.
		virtual RTTI* GetRTTI(void) const { return &mRTTI; }

	protected :

		static RTTI	mRTTI;

	};

	/// @brief Creates an RTTI structure for a specified class pair.
	/// @remarks The first parameter (0) is a stub. See RTTI constructor for details.
	/// Note that this is the place where the RegisterReflection function is being called when the program boots up.
	template <class T, class BaseClass>
	RTTI RTTIGlue<T, BaseClass>::mRTTI
		( 0, RTTIGlue<T, BaseClass>::GetClassID(), typeid(T).name(), BaseClass::GetClassRTTI(), (ClassFactoryFunc)T::Create, 
		(RegisterReflectionFunc)T::RegisterReflection );
}


#endif	// _RTTICLASS_H