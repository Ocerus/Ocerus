#include "Common.h"
#include "RTTI.h"
#include "../Properties.h"
#include "../../LogSystem/LogMgr.h"
#include <cstring>

RTTI::RTTI(	uint8 dwStub, ClassID CLID, const char* szClassName, RTTI* pBaseClassRTTI,
				ClassFactoryFunc pFactory, RegisterReflectionFunc pReflectionFunc ) :
	mCLID			( CLID				),
	mBaseRTTI		( pBaseClassRTTI	),
	mClassFactory( pFactory			),
	mComponentDependencies(0),
	mTransient(false)
{
	OC_UNUSED(dwStub);
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
	if (mBaseRTTI)
		mBaseRTTI->EnumProperties(out, flagMask);
	mProperties.EnumProperties(out, flagMask);
}

void RTTI::EnumProperties( RTTIBaseClass* owner, PropertyList& out, const PropertyAccessFlags flagMask ) const
{
	if (mBaseRTTI)
		mBaseRTTI->EnumProperties(owner, out, flagMask);
	mProperties.EnumProperties(owner, out, flagMask);
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
	AbstractProperty* prop = mProperties.GetProperty(key, flagMask);
	if (!prop && mBaseRTTI) return mBaseRTTI->GetProperty(key, flagMask);
	return prop;
}

void RTTI::AddComponentDependency( const EntitySystem::eComponentType dep )
{
	mComponentDependencies.push_back(dep);
}

bool RTTI::HasProperty(const StringKey key)
{
	return mProperties.HasProperty(key);
}