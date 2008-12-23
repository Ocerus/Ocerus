#ifndef PropertyHolder_h__
#define PropertyHolder_h__

#include "AbstractProperty.h"
#include "../Settings.h"
#include <map>

/// @name  Forward declarations.
//@{
class RTTIBaseClass;
//@}

/// @name This class encapsulates abstract properties to allow easier access to their values.
class PropertyHolder
{
public:
	/// @name Constructors.
	//@{
	PropertyHolder(void): mOwner(0), mProperty(0) {}
	PropertyHolder(const PropertyHolder& rhs): mOwner(rhs.mOwner), mProperty(rhs.mProperty) {}
	PropertyHolder(RTTIBaseClass* owner, AbstractProperty* prop): mOwner(owner), mProperty(prop) {}
	//@}

	PropertyHolder& operator=(const PropertyHolder& rhs)
	{
		mOwner = rhs.mOwner;
		mProperty = rhs.mProperty;
		return *this;
	}

	/// @name Returns the value of this property.
	template<class T>
	T GetValue(void)
	{
		if (!mProperty)
		{
			ReportUndefined();
			return PropertyType<T>::GetDefaultValue();
		}
		return mProperty->GetValue<T>(mOwner);
	}
	
	/// @name Sets the value of this property.
	template<class T>
	void SetValue(const T value)
	{
		if (!mProperty)
		{
			ReportUndefined();
			return;
		}
		mProperty->SetValue<T>(mOwner, value);
	}

private:
	RTTIBaseClass* mOwner;
	AbstractProperty* mProperty;

	/// @name Problem reporting.
	void ReportUndefined(void);
};

#endif // PropertyHolder_h__