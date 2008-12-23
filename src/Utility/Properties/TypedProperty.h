#ifndef _TYPED_PROPERTY_H
#define _TYPED_PROPERTY_H

#include "../Properties/AbstractProperty.h"

/// @name Forward declarations.
class RTTIBaseClass;

/** This intermediate class defines a property that is typed, but not bound as a member of a particular
	class.
*/
template <class T> class TypedProperty: public AbstractProperty
{
public:

	/// @name Constructor.
	inline TypedProperty(const char* name, const uint8 accessFlags):
		AbstractProperty(name, accessFlags) {}

	/// @name Returns the type of this property.
	virtual ePropertyType GetType(void) const { return PropertyType<T>::GetTypeID(); }

	/// @name Getter and setter for this property's value.
	//@{
	virtual T GetValue( RTTIBaseClass* owner ) = 0;
	virtual void SetValue( RTTIBaseClass* owner, T value ) = 0;
	//@}

};

#endif	// _TYPED_PROPERTY_H