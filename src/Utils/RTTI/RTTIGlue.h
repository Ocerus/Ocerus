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
			mDynamicProperties = 0;
		}

		/// Destructor.
		~RTTIGlue(void)
		{
			if (mDynamicProperties) delete mDynamicProperties;
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
			if (mDynamicProperties)
			{
				AbstractProperty* prop = mDynamicProperties->GetProperty(key, flagMask);
				if (prop) return prop;
			}

			return GetRTTI()->GetProperty(key, flagMask);
		}

		/// Fills a data structure with all properties.
		void EnumProperties( RTTIBaseClass* owner, PropertyList& out, const PropertyAccessFlags flagMask = PA_FULL_ACCESS ) const
		{
			GetRTTI()->EnumProperties(owner, out, flagMask);
			if (mDynamicProperties) mDynamicProperties->EnumProperties(owner, out, flagMask);
		}

		/// Registers a dynamic property. Takes in the property name, its getter and setter functions, and the property
		///	type as a template parameter.
		template <class PropertyTypes>
		bool RegisterDynamicProperty(StringKey name, const PropertyAccessFlags accessFlags,
			const string& comment)
		{
			if (!mDynamicProperties) mDynamicProperties = new PropertyMap();
			ValuedProperty<PropertyTypes>* pProperty = new ValuedProperty<PropertyTypes>(name, accessFlags, comment);
			if (mDynamicProperties->AddProperty(pProperty))
			{
				PropertySystem::GetProperties()->push_back(pProperty);
				return true;
			}
			else
			{
				delete pProperty;
				return false;
			}
		}

		/// Unregisters a dynamic property of a certain name.
		inline bool UnregisterDynamicProperty(const StringKey name)
		{
			if (!mDynamicProperties) return false;  
			return mDynamicProperties->DeleteProperty(name);
		}

	protected :

		static RTTI	mRTTI;
		PropertyMap* mDynamicProperties;

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