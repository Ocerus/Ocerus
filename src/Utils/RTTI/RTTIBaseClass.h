/// @file
/// Lowest level classes for custom classes using the RTTI.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef _BASEOBJECT_H_
#define _BASEOBJECT_H_

#include "RTTIGlue.h"

namespace Reflection
{
	/// This is an empty class with no attached %RTTI information.
	///	RTTINullClass itself has no RTTI support. This class is used for implementing dynamic properties.
	class RTTINullClass
	{
	public:
	
	  /// Constructor.
		RTTINullClass(void)
		{
			mDynamicProperties = 0;
		}
		
		/// Destructor.
		~RTTINullClass(void)
		{
			if (mDynamicProperties) delete mDynamicProperties;
		}
	  
	  /// Registers a dynamic property. Takes in the property name, the access flags, the comment and the property
		///	type as a template parameter.
		template <class PropertyTypes>
		bool RegisterDynamicProperty(StringKey name, const PropertyAccessFlags accessFlags, const string& comment)
		{
			if (!mDynamicProperties) mDynamicProperties = new PropertyMap();
			ValuedProperty<PropertyTypes>* pProperty = new ValuedProperty<PropertyTypes>(name, accessFlags, comment);
			bool result = mDynamicProperties->AddProperty(pProperty);
			
			if (result) PropertySystem::GetProperties()->push_back(pProperty);
			else delete pProperty;
			
			DynamicPropertyChanged(name, true, result);
			return result;
		}

		/// Unregisters a dynamic property of a certain name.
		inline bool UnregisterDynamicProperty(const StringKey name)
		{
			bool result = mDynamicProperties && mDynamicProperties->DeleteProperty(name);
			DynamicPropertyChanged(name, false, result);
			return result;
		}
		
		/// This function serves as overwritable callback for child classes indicating registering/unregistering properties.
		/// @param propertyName The name of registered/unregistered property.
		/// @param reg True if the property is registered, False if the property is unregistered.
		/// @param success True if the registration/unregistration was successful.
		virtual void DynamicPropertyChanged(const StringKey propertyName, bool reg, bool success) { OC_UNUSED(propertyName); OC_UNUSED(reg); OC_UNUSED(success); }
	
	protected :

		/// It is necessary to implement this to avoid compilation errors in the templatized RTTI code
		///	In any case no RTTI support is provided by this class.
		static inline RTTI* GetClassRTTI(void) { return NULL; }
		
		PropertyMap* mDynamicProperties;

	};

	/// Ultimate ancestor for all classes.
	/// RTTIBaseClass implements no specific functionality, apart from
	///	the fact it provides the ultimate ancestor RTTI for all other classes.
	class RTTIBaseClass : public RTTIGlue<RTTIBaseClass, RTTINullClass>
	{

	};
}

#endif	// _BASEOBJECT_H_