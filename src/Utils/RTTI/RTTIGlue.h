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
#include "../Properties/PropertyMap.h"
#include "../Properties/ValuedProperty.h"

namespace Reflection
{
	/// This is the %RTTI "sandwich class" being used to augment a class with %RTTI support. Classes
	///	supporting %RTTI need to derive from this class, with their ancestor specified as the BaseClass
	///	template parameter.
	template <class T, class BaseClass> class RTTIGlue : public BaseClass
	{
	public :

		/// Constructor.
		RTTIGlue(void)
		{
		}

		/// Destructor.
		~RTTIGlue(void)
		{
		}

		/// Default factory function. Creates an instance of T.
		/// The factory function is called by the system to dynamically create
		///	class instances from class IDs.
		static T* CreateInstance(void) { return new T(); }

		/// Default reflection registration function. Registers nothing by default.
		static void	RegisterReflection(void) {}

		/// Registers a property. Takes in the property name, its getter and setter functions, and the property
		///	type as a template parameter.
		/// This function should be called only from within a user-defined RegisterReflection function.
		template <class PropertyTypes>
		static void RegisterProperty(StringKey name, typename Property<T, PropertyTypes>::GetterType getter,
			typename Property<T, PropertyTypes>::SetterType setter, const PropertyAccessFlags accessFlags,
			const string& comment)
		{
			Property<T, PropertyTypes>* pProperty = new Property<T, PropertyTypes>( name, getter, setter, accessFlags, comment );
			if (T::GetClassRTTI()->AddProperty(pProperty))
			{
				PropertySystem::GetProperties()->push_back( pProperty );
			}
			else
			{
				delete pProperty;
			}
		}

		/// Registers a function. Takes in the function name and its address.
		/// This function should be called only from within a user-defined RegisterReflection function.
		inline static void RegisterFunction(StringKey name, typename Property<T, PropertyFunctionParameters>::SetterType function, const PropertyAccessFlags accessFlags, const string& comment)
		{
			RegisterProperty<PropertyFunctionParameters>(name, 0, function, accessFlags | PA_TRANSIENT, comment);
		}

		/// Registers an entity component dependant on the owner of this class.
		/// Component dependencies solve the issue of defining the correct order of entity components creation
		/// and initialization. For example, components handling physics should be usually inited before the components
		/// handling logic.
		/// @remarks This function should be called only from within a user-defined RegisterReflection function.
		static void AddComponentDependency(const EntitySystem::eComponentType cmp)
		{
			T::GetClassRTTI()->AddComponentDependency(cmp);
		}
		
		/// Returns whether the component is transient.
		static inline bool IsTransient(void) { return mRTTI.IsTransient(); }
		
		/// Sets whether the component is transient.
		static inline void SetTransient(bool transient) { mRTTI.SetTransient(transient); }

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

		/// Returns a property identified by it's string key. Access restriction filter can be defined.
		AbstractProperty* GetPropertyPointer(const StringKey key, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const
		{
			if (BaseClass::mDynamicProperties)
			{
				AbstractProperty* prop = BaseClass::mDynamicProperties->GetProperty(key, flagMask);
				if (prop) return prop;
			}

			return GetRTTI()->GetProperty(key, flagMask);
		}

		/// Fills a data structure with all properties.
		void EnumProperties( RTTIBaseClass* owner, PropertyList& out, const PropertyAccessFlags flagMask = PA_FULL_ACCESS ) const
		{
			GetRTTI()->EnumProperties(owner, out, flagMask);
			if (BaseClass::mDynamicProperties) BaseClass::mDynamicProperties->EnumProperties(owner, out, flagMask);
		}

	protected :

		static RTTI	mRTTI;

	};

	/// Creates an RTTI structure for a specified class pair.
	/// The first parameter (0) is a stub. See RTTI constructor for details.
	/// Note that this is the place where the RegisterReflection function is being called when the program boots up.
	template <class T, class BaseClass>
	RTTI RTTIGlue<T, BaseClass>::mRTTI(
		0,
		RTTIGlue<T, BaseClass>::GetClassID(),
		typeid(T).name(),
		BaseClass::GetClassRTTI(),
		(ClassFactoryFunc)T::CreateInstance,
		(RegisterReflectionFunc)T::RegisterReflection
	);
}


#endif	// _RTTICLASS_H