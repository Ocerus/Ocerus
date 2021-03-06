#include "Common.h"
#include "PropertyMap.h"
#include "../Properties.h"
#include "../../LogSystem/LogMgr.h"

void PropertyMap::EnumProperties( AbstractPropertyList& out, const PropertyAccessFlags flagMask ) const
{
	for ( AbstractPropertyMap::const_iterator it = mProperties.begin(); it != mProperties.end(); ++it )
	{
		if ((it->second->GetAccessFlags()&flagMask) != 0)
		{
			out.push_back( it->second );
		}
	}
}

void PropertyMap::EnumProperties( RTTIBaseClass* owner, PropertyList& out, const PropertyAccessFlags flagMask ) const
{
	for ( AbstractPropertyMap::const_iterator it = mProperties.begin(); it != mProperties.end(); ++it )
	{
		if ((it->second->GetAccessFlags()&flagMask) != 0)
		{
			out.push_back(PropertyHolder(owner, it->second));
		}
	}
}

AbstractProperty* PropertyMap::GetProperty( const StringKey key, const PropertyAccessFlags flagMask ) const
{
	AbstractPropertyMap::const_iterator it = mProperties.find(key);
	if (it != mProperties.end() && (it->second->GetAccessFlags()&flagMask) != 0)
	{
		return it->second;
	}
	return 0;
}

bool PropertyMap::HasProperty(const StringKey key) const
{
	return mProperties.find(key) != mProperties.end();
}

bool PropertyMap::AddProperty( AbstractProperty* prop )
{
	OC_ASSERT(prop);
	if (HasProperty(prop->GetKey()))
	{
		return false;
	}

	mProperties[prop->GetKey()] = prop;
	return true;
}

bool PropertyMap::DeleteProperty(const StringKey key)
{
	return mProperties.erase(key) > 0;
}

void PropertyMap::ClearProperties()
{
  mProperties.clear();
}