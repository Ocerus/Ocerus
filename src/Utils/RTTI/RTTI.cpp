#include "Common.h"
#include "RTTI.h"
#include "../Properties.h"

RTTI::RTTI(	uint8 dwStub, ClassID CLID, const char* szClassName, RTTI* pBaseClassRTTI,
				ClassFactoryFunc pFactory, RegisterReflectionFunc pReflectionFunc ) :
	mCLID			( CLID				),
	mBaseRTTI		( pBaseClassRTTI	),
	mClassFactory( pFactory			),
	mComponentDependencies(0)
{
	OC_ASSERT( CLID != 0 );
	OC_ASSERT( strlen(szClassName) <= CLASSNAME_LENGTH && "RTTI:Class name too long" );
#ifdef __WIN__
	#pragma warning(disable: 4996)
#endif
	strcpy( mClassName, szClassName );

	if ( pReflectionFunc )
		pReflectionFunc();
}

void RTTI::EnumProperties( AbstractPropertyList& out, const PropertyAccessFlags flagMask ) const
{
	if ( mBaseRTTI )
		mBaseRTTI->EnumProperties( out, flagMask );
	for ( AbstractPropertyMap::const_iterator it = mProperties.begin(); it != mProperties.end(); ++it )
		if ((it->second->GetAccessFlags()&flagMask) != 0)
			out.push_back( it->second );
}

void RTTI::EnumProperties( RTTIBaseClass* owner, PropertyList& out, const PropertyAccessFlags flagMask ) const
{
	if ( mBaseRTTI )
		mBaseRTTI->EnumProperties( owner, out, flagMask );
	for ( AbstractPropertyMap::const_iterator it = mProperties.begin(); it != mProperties.end(); ++it )
		if ((it->second->GetAccessFlags()&flagMask) != 0)
		{
			StringKey key = it->second->GetKey();
			if (out.find(key) != out.end())
			{
				ocWarning << "EnumProperties:Duplicate property name '" << it->second->GetName() << "' -> overwriting";
			}
			out[key] = PropertyHolder(owner, it->second);
		}
}

void RTTI::EnumComponentDependencies( ComponentDependencyList& out ) const
{
	if ( mBaseRTTI )
		mBaseRTTI->EnumComponentDependencies(out);
	for (ComponentDependencyList::const_iterator it=mComponentDependencies.begin(); it!=mComponentDependencies.end(); ++it )
		out.push_back(*it);
}

AbstractProperty* RTTI::GetProperty( const StringKey key, const PropertyAccessFlags flagMask ) const
{
	AbstractPropertyMap::const_iterator it = mProperties.find(key);
	if (it != mProperties.end())
	{
		if ((it->second->GetAccessFlags()&flagMask) != 0)
			return it->second;
		return 0;
	}
	if (mBaseRTTI)
		return mBaseRTTI->GetProperty(key);
	return 0;
}

void RTTI::AddProperty( AbstractProperty* prop )
{
	mProperties[prop->GetKey()] = prop;
}

void RTTI::AddComponentDependency( const EntitySystem::eComponentType dep )
{
	mComponentDependencies.push_back(dep);
}