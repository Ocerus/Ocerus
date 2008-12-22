#ifndef PropertyHolder_h__
#define PropertyHolder_h__

#include "AbstractProperty.h"
#include "../Settings.h"
#include <map>

/// @name  Forward declarations.
//@{
class RTTIBaseClass;
//@}

class PropertyHolder
{
public:
	PropertyHolder(void): mOwner(0), mProperty(0) {}
	PropertyHolder(const PropertyHolder& rhs): mOwner(rhs.mOwner), mProperty(rhs.mProperty) {}
	PropertyHolder(RTTIBaseClass* owner, CAbstractProperty* prop): mOwner(owner), mProperty(prop) {}

	PropertyHolder& operator=(const PropertyHolder& rhs)
	{
		mOwner = rhs.mOwner;
		mProperty = rhs.mProperty;
		return *this;
	}

	template<class T>
	T GetValue(void)
	{
		if (!mProperty)
		{
			ReportUndefined();
			return CPropertyType<T>::GetDefaultValue();
		}
		return mProperty->GetValue<T>(mOwner);
	}
	
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
	CAbstractProperty* mProperty;

	void ReportUndefined(void);
};

#endif // PropertyHolder_h__